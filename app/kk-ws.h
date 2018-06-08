//
//  kk-ws.h
//  app
//
//  Created by hailong11 on 2018/6/8.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef kk_ws_h
#define kk_ws_h

#include "kk-script.h"
#include <event.h>
#include <evhttp.h>
#include <evdns.h>

namespace kk {
    
    
    class WebSocket : public Object {
    public:
        WebSocket();
        virtual ~WebSocket();
        
        virtual kk::Boolean open(event_base * base,evdns_base * dns, kk::CString url,kk::CString protocol);
        virtual void close();
        
        virtual void onClose();
        
        virtual duk_ret_t duk_on(duk_context * ctx);
        virtual duk_ret_t duk_close(duk_context * ctx);
        
        DEF_SCRIPT_CLASS
        
    protected:
        kk::String _errmsg;
        evhttp_connection * _conn;
    };
    
}

#endif /* kk_ws_h */
