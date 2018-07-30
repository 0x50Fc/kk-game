//
//  kk-dispatch.h
//  app
//
//  Created by hailong11 on 2018/7/30.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef kk_dispatch_h
#define kk_dispatch_h

#include "kk-object.h"
#include "kk-chan.h"
#include "kk-block.h"
#include <pthread.h>

struct event_base;
struct event;

namespace kk {
    
    class DispatchQueue;
    
    typedef void (*DispatchFunc)(DispatchQueue * queue,BK_DEF_ARG);
    
    class DispatchQueue : public kk::Object {
    public:
        DispatchQueue();
        DispatchQueue(event_base * base);
        virtual ~DispatchQueue();
        virtual struct event_base * base();
        virtual void async(DispatchFunc func, BK_DEF_ARG);
        virtual void sync(DispatchFunc func, BK_DEF_ARG);
        virtual void loopbreak();
        static DispatchQueue * main();
    protected:
        virtual void run();
        kk::Chan * _chan;
        struct event_base * _base;
        struct event * _event;
        bool _loopbreak;
        bool _main;
        pthread_t _pid;
        friend void DispatchQueueCB(evutil_socket_t fd, short ev, void * ctx);
    };
    
}



#endif /* kk_dispatch_h */
