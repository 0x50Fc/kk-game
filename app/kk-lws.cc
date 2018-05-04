//
//  kk-lws.cc
//  KKGame
//
//  Created by hailong11 on 2018/5/4.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include "kk-config.h"
#include "kk-lws.h"
#include "kk-string.h"
#include <libwebsockets.h>

namespace kk {
    
    static uv_loop_t * lws_loop_get(duk_context * ctx) {
    
        uv_loop_t * v = nullptr;
        
        duk_push_global_object(ctx);
        
        duk_push_string(ctx, "WebSocket");
        duk_get_prop(ctx, -2);
        
        if(duk_is_object(ctx, -1)) {
            
            duk_push_string(ctx, "loop");
            duk_get_prop(ctx, -2);
            
            if(duk_is_pointer(ctx, -1)) {
                v = (uv_loop_t *) duk_to_pointer(ctx, -1);
            }
            
            duk_pop(ctx);
            
        }
        
        duk_pop_2(ctx);
        
        return v;
    }
    
    static int lws_cb(struct lws *wsi, enum lws_callback_reasons reason,
                      void *user, void *in, size_t len) {
        
        return 0;
    }
    
    static const struct lws_protocols protocols[] = {
        {
            "ws",
            lws_cb,
            0,
            0,
        },
        { NULL, NULL, 0, 0 }
    };
    
    static duk_ret_t lws_dealloc(duk_context * ctx) {
        
        duk_get_prop_string(ctx, -1, "__object");
        
        if(duk_is_pointer(ctx, -1)) {
            lws_context * context = (lws_context *) duk_to_pointer(ctx, -1);
            lws_libuv_stop_without_kill(context, 0);
            lws_context_destroy(context);
        }
        
        duk_pop(ctx);
        
        return 0;
    }
    
    static duk_ret_t lws_alloc(duk_context * ctx) {
        
        int top = duk_get_top(ctx);
        
        kk::String url;
        
        if(top >0 && duk_is_string(ctx, -top)) {
            url = duk_to_string(ctx, -top);
        } else {
            return 0;
        }
        
        lws_context_creation_info info;
        
        memset(&info, 0, sizeof(lws_context_creation_info));
        
        info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
        info.port = CONTEXT_PORT_NO_LISTEN;
        info.protocols = protocols;
        
        lws_context * context = lws_create_context(&info);
        
        if (!context) {
            kk::Log("lws init failed\n");
            return 0;
        }
        
        struct lws_client_connect_info i;
        
        kk::CString proto= nullptr;
        kk::CString addr = nullptr;
        kk::CString path = nullptr;
        kk::Int port = 0;
        
        lws_parse_uri((char *) url.c_str(), &proto, &addr, &port, &path);
        
        if(port == 0) {
            
            if(kk::CStringEqual(proto, "wss") || kk::CStringEqual(proto, "https")) {
                port = 443;
            } else {
                port = 80;
            }
            
        }
        
        memset(&i, 0, sizeof i);
        
        i.context = context;
        i.port = 443;
        i.address = addr;
        i.path = path;
        i.host = addr;
        i.origin = addr;
        
        if(kk::CStringEqual(proto, "wss") || kk::CStringEqual(proto, "https")) {
            i.ssl_connection = LCCSCF_USE_SSL;
        }
        
        i.protocol = protocols[0].name;
        
        lws_client_connect_via_info(&i);

        duk_push_object(ctx);
        
        duk_push_pointer(ctx, context);
        duk_put_prop_string(ctx, -2, "__object");
        
        duk_push_c_function(ctx, lws_dealloc, 1);
        duk_set_finalizer(ctx, -2);
        
        duk_push_global_object(ctx);
        
        duk_get_prop_string(ctx, -1, "WebSocket");
        
        duk_set_prototype(ctx, -3);
        
        duk_pop(ctx);
    
        return 1;
    }
    
    static duk_ret_t lws_on_func(duk_context * ctx);
    static duk_ret_t lws_connect_func(duk_context * ctx);
    static duk_ret_t lws_disconnect_func(duk_context * ctx);
    static duk_ret_t lws_write_func(duk_context * ctx);
    
    void lws_openlibs(duk_context * ctx, uv_loop_t * loop) {
        

        duk_push_global_object(ctx);
        
        duk_push_string(ctx, "WebSocket");
        duk_push_object(ctx);
        
        duk_push_string(ctx, "loop");
        duk_push_pointer(ctx, loop);
        duk_put_prop(ctx, -3);
        
        duk_push_string(ctx, "alloc");
        duk_push_c_function(ctx, lws_alloc, 1);
        duk_put_prop(ctx, -3);
        
        duk_push_string(ctx, "on");
        duk_push_c_function(ctx, lws_on_func, 2);
        duk_put_prop(ctx, -3);
        
        duk_push_string(ctx, "connect");
        duk_push_c_function(ctx, lws_connect_func, 0);
        duk_put_prop(ctx, -3);
        
        duk_push_string(ctx, "disconnect");
        duk_push_c_function(ctx, lws_disconnect_func, 0);
        duk_put_prop(ctx, -3);
        
        duk_push_string(ctx, "write");
        duk_push_c_function(ctx, lws_write_func, 1);
        duk_put_prop(ctx, -3);
        
        duk_put_prop(ctx, -3);
        
        duk_pop(ctx);
        
    }
    
    static duk_ret_t lws_on_func(duk_context * ctx) {
        return 0;
    }
    
    static duk_ret_t lws_connect_func(duk_context * ctx) {
        return 0;
    }
    
    static duk_ret_t lws_disconnect_func(duk_context * ctx) {
        return 0;
    }
    
    static duk_ret_t lws_write_func(duk_context * ctx) {
        return 0;
    }
    
}
