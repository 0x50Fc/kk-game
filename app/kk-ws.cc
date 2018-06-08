//
//  kk-ws.cc
//  app
//
//  Created by hailong11 on 2018/6/8.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include "kk-config.h"
#include "kk-ws.h"
#include "kk-ev.h"
#include <sys/queue.h>

#define Sec_WebSocket_Key "KJYIOPKLJH==="

namespace kk {
    
    
    static duk_ret_t WebSocketAlloc(duk_context * ctx) {
        int top = duk_get_top(ctx);
        
        kk::CString url = nullptr;
        kk::CString protocol = nullptr;
        
        if(top >0 && duk_is_string(ctx, -top)) {
            url = duk_to_string(ctx, -top);
        }
        
        if(top >1 && duk_is_string(ctx, -top + 1)) {
            protocol = duk_to_string(ctx, -top + 1);
        }
        
        if(url) {
            
            kk::Strong v = new WebSocket();
            
            WebSocket * vv = v.as<WebSocket>();
            
            vv->open(ev_base(ctx),nullptr,url,protocol);
            
            kk::script::PushObject(ctx, vv);
            
            return 1;
        }
        
        return 0;
    }
    
    IMP_SCRIPT_CLASS_BEGIN(nullptr, WebSocket, WebSocket)
    
    duk_push_string(ctx, "alloc");
    duk_push_c_function(ctx, WebSocketAlloc, 2);
    duk_put_prop(ctx, -3);
   
    IMP_SCRIPT_CLASS_END
    
    WebSocket::WebSocket():_conn(nullptr) {
        
    }
    
    WebSocket::~WebSocket() {
        
    }
    
    void WebSocket::close() {
        if(_conn != nullptr) {
            evhttp_connection_free(_conn);
            _conn = nullptr;
        }
    }
    
    static void WebSocket_conn_cb(struct evhttp_connection * conn, void * data) {
        WebSocket * v = (WebSocket *) data;
        v->onClose();
    }
    
    static void WebSocket_req_cb(struct evhttp_request * req, void * data) {
       
    }
    
    static void WebSocket_data_rd(struct bufferevent *bev, void *ctx) {
        
    }
    
    static void WebSocket_data_wd(struct bufferevent *bev, void *ctx) {
        
    }
    
    static void WebSocket_event_cb(struct bufferevent *bev, short what, void *ctx) {
        
    }

    
    static int WebSocket_req_header_cb(struct evhttp_request * req, void * data) {
     
        fprintf(stdout, "< HTTP/1.1 %d %s\n", evhttp_request_get_response_code(req), evhttp_request_get_response_code_line(req));
        struct evkeyvalq* headers = evhttp_request_get_input_headers(req);
        struct evkeyval* header;
        TAILQ_FOREACH(header, headers, next)
        {
            fprintf(stdout, "< %s: %s\n", header->key, header->value);
        }
        fprintf(stdout, "< \n");
        
        evhttp_connection * conn = evhttp_request_get_connection(req);
        
        bufferevent * bev = evhttp_connection_get_bufferevent(conn);
        
        bufferevent_setcb(bev, WebSocket_data_rd, WebSocket_data_wd, WebSocket_event_cb, data);
        
        bufferevent_enable(bev, EV_READ);
        
        evhttp_connection_set_timeout(conn, 0);
        
        return 0;
    }
 
    kk::Boolean WebSocket::open(event_base * base,evdns_base * dns, kk::CString url,kk::CString protocol) {
        
        evhttp_uri * uri = evhttp_uri_parse(url);
        
        if(uri == nullptr) {
            _errmsg = "错误的URL";
            return false;
        }
        
        kk::String host = evhttp_uri_get_host(uri);
        kk::String origin = evhttp_uri_get_scheme(uri);
        kk::String path = evhttp_uri_get_path(uri);
        
        if(path == "") {
            path = "/";
        }
        
        kk::String query = evhttp_uri_get_query(uri);
        
        if(query != "") {
            path.append("?");
            path.append(query);
        }
        
        if(origin == "ws") {
            origin = "http";
        }
        
        if(origin == "wss") {
            origin = "https";
        }
        
        origin.append("://");
        origin.append(host);
        
        char fmt[64];
        
        int port = evhttp_uri_get_port(uri);
        
        if(port == 0) {
            port = 80;
        } else {
            host.append(":");
            snprintf(fmt, sizeof(fmt), "%d",port);
            host.append(fmt);
            
            origin.append(":");
            origin.append(fmt);
        }
        
        _conn = evhttp_connection_base_new(base, dns, evhttp_uri_get_host(uri), port);
        
        evhttp_connection_set_timeout(_conn, 10);
        evhttp_connection_set_closecb(_conn, WebSocket_conn_cb, this);

        evhttp_request * req = evhttp_request_new(WebSocket_req_cb, this);
        
        evhttp_add_header(req->output_headers, "Host", host.c_str());
        evhttp_add_header(req->output_headers, "Upgrade", "websocket");
        evhttp_add_header(req->output_headers, "Connection", "Upgrade");
        evhttp_add_header(req->output_headers, "Sec-WebSocket-Key", Sec_WebSocket_Key);
        evhttp_add_header(req->output_headers, "Sec-WebSocket-Version", "13");
        evhttp_add_header(req->output_headers, "Origin", origin.c_str());
        
        evhttp_request_set_header_cb(req, WebSocket_req_header_cb);
        
        evhttp_make_request(_conn, req, EVHTTP_REQ_GET, path.c_str());
        
        evhttp_uri_free(uri);
        
        return true;
    }
    
    void WebSocket::onClose() {
        
    }
    
    duk_ret_t WebSocket::duk_on(duk_context * ctx) {
        
        return 0;
    }
    
    duk_ret_t WebSocket::duk_close(duk_context * ctx) {
        
        return 0;
    }
    
}

