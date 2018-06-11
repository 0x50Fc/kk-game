//
//  kk-http.h
//  app
//
//  Created by zhanghailong on 2018/6/11.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef kk_http_h
#define kk_http_h


#include "kk-script.h"
#include <event.h>
#include <evhttp.h>
#include <evdns.h>
#include <list>

namespace kk {

    class HttpTask;
    
    class Http : public kk::script::HeapObject, public kk::script::IObject {
    public:
        Http(event_base * base,evdns_base * dns);
        virtual ~Http();
        virtual duk_ret_t duk_send(duk_context * ctx);
        DEF_SCRIPT_CLASS
    protected:
        Http();
        event_base * _base;
        evdns_base * _dns;
        std::map<kk::String,std::list<evhttp_connection *>> _conns;
        std::map<kk::Uint64,HttpTask *> _tasks;
        kk::Uint64 _autoId;
        
        virtual evhttp_connection * getConnection(kk::CString host);
        virtual void keepliveConnection(evhttp_connection * conn,kk::CString host);
        virtual void removeConnection(evhttp_connection * conn);
        
        virtual void remove(HttpTask * httpTask);
        friend class HttpTask;
        friend void Http_conn_cb(struct evhttp_connection * conn, void * data);
    };
    
}


#endif /* kk_http_h */
