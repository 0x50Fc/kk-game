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
        
        virtual event_base * base();
        virtual evdns_base * dns();
        
        DEF_SCRIPT_CLASS
    protected:
        Http();
        event_base * _base;
        evdns_base * _dns;
        std::map<kk::Uint64,HttpTask *> _tasks;
        kk::Uint64 _autoId;
        
        virtual void remove(HttpTask * httpTask);
        friend class HttpTask;
    };
    
}


#endif /* kk_http_h */
