//
//  kk-dispatch.cpp
//  app
//
//  Created by hailong11 on 2018/7/30.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include <event.h>
#include <evdns.h>
#include <pthread.h>

#include "kk-config.h"
#include "kk-dispatch.h"

namespace kk {
    
    static struct timeval tv = {0,60000};
    
    void DispatchQueueCB(evutil_socket_t fd, short ev, void * ctx) {
        DispatchQueue * queue = (DispatchQueue *) ctx;
        queue->run();
    }
    
    
    class DispatchObject {
    public:
        DispatchObject(DispatchFunc func,BK_DEF_ARG,bool synced):func(func),context(__BK_CTX),synced(synced) {
            if(synced) {
                pthread_mutex_init(&_lock, nullptr);
                pthread_cond_init(&_cond, nullptr);
            }
        }
        virtual ~DispatchObject() {
            if(synced) {
                pthread_cond_destroy(&_cond);
                pthread_mutex_destroy(&_lock);
            }
        }
        virtual void wait(kk::Chan * chan) {
            pthread_mutex_lock(&_lock);
            chan->push(this);
            pthread_cond_wait(&_cond, &_lock);
            pthread_mutex_unlock(&_lock);
        }
        
        virtual void join() {
            pthread_mutex_lock(&_lock);
            pthread_cond_broadcast(&_cond);
            pthread_mutex_unlock(&_lock);
        }
        
        DispatchFunc func;
        kk::Strong context;
        bool synced;
    protected:
        pthread_cond_t _cond;
        pthread_mutex_t _lock;
        event * _event;
    };
    
    void DispatchObjectRelease(kk::Chan * chan,kk::ChanObject object) {
        DispatchObject * v = (DispatchObject *) object;
        delete v;
    }
    

    void * DispatchQueueRun(void * data) {
        DispatchQueue * queue = (DispatchQueue *) data;
        event_base_loop(queue->base(), 0);
        if(queue->_pid != 0) {
            pthread_exit(nullptr);
        }
        return nullptr;
    }
    
    void DispatchQueue::run() {
        
        do {
            
            DispatchObject * object =  (DispatchObject *) _chan->pop();
            
            if(object == nullptr) {
                break;
            }
            
            if(object->func) {
                (*object->func)(this,object->context.as<BlockContext>());
            }
            
            if(object->synced) {
                object->join();
            }
            
            _chan->releaseObject((ChanObject) object);
            
        } while (1);
        
        
        if(!_loopbreak) {
            evtimer_add(_event, &tv);
        }
        
    }
    
    static void DispatchQueueBreak(DispatchQueue * queue,BK_DEF_ARG) {
        queue->loopbreak();
    }
        
    DispatchQueue::DispatchQueue() {
        _attach = false;
        _joined = false;
        _loopbreak = false;
        _chan = new kk::Chan();
        _chan->retain();
        _base = event_base_new();
        _event = evtimer_new(_base, DispatchQueueCB, this);
        evtimer_add(_event, &tv);
        pthread_create(&_pid, nullptr, DispatchQueueRun, this);
    }
    
    DispatchQueue::DispatchQueue(event_base * base) {
        _attach = true;
        _joined = false;
        _loopbreak = false;
        _chan = new kk::Chan();
        _chan->retain();
        _base = base;
        _event = evtimer_new(_base, DispatchQueueCB, this);
        evtimer_add(_event, &tv);
        _pid = pthread_self();
    }
    
    
    DispatchQueue::~DispatchQueue() {
        
        if(_attach) {
            _chan->release();
            event_free(_event);
        } else {
            if(!_joined) {
                _joined = true;
                evtimer_del(_event);
                async(DispatchQueueBreak, nullptr);
                pthread_join(_pid, nullptr);
            }
            _chan->release();
            event_free(_event);
            event_base_free(_base);
        }
        
    }
    
    void DispatchQueue::join() {
        if(!_attach && !_joined) {
            _joined = true;
            async(DispatchQueueBreak, nullptr);
            pthread_join(_pid, nullptr);
            evtimer_del(_event);
        }
    }
    
    void DispatchQueue::loopbreak() {
        if(!_loopbreak) {
            if(pthread_self() != _pid) {
                async(DispatchQueueBreak, nullptr);
            } else {
                _loopbreak = true;
                event_base_loopbreak(_base);
            }
        }
    }
    
    struct event_base * DispatchQueue::base() {
        return _base;
    }
    
    void DispatchQueue::async(DispatchFunc func,BK_DEF_ARG) {
        DispatchObject * v = new DispatchObject(func,__BK_CTX,false);
        _chan->push(v);
    }
    
    void DispatchQueue::sync(DispatchFunc func, BK_DEF_ARG) {
        DispatchObject * v = new DispatchObject(func,__BK_CTX,true);
        v->wait(_chan);
    }
    
    
}
