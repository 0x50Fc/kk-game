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

#define Sec_WebSocket_Key "RCfYMqhgCo4N4E+cIZ0iPg=="
#define MAX_BUF_SIZE 204800

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
            
            vv->open(ev_base(ctx),ev_dns(ctx),url,protocol);
            
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
    
    WebSocket::WebSocket():_bev(nullptr) {
        
    }
    
    WebSocket::~WebSocket() {
        
    }
    
    void WebSocket::close() {
        if(_bev != nullptr) {
            int fd = bufferevent_getfd(_bev);
            if(fd != -1) {
                evutil_closesocket(fd);
            }
            bufferevent_free(_bev);
            _bev = nullptr;
        }
    }

    void WebSocket_data_rd(struct bufferevent *bev, void *ctx) {
        WebSocket * v = (WebSocket *) ctx;
        v->onReading();
    }
    
    void WebSocket_data_wd(struct bufferevent *bev, void *ctx) {
        WebSocket * v = (WebSocket *) ctx;
        v->onWritting();
    }
    
    void WebSocket_event_cb(struct bufferevent *bev, short what, void *ctx) {
        WebSocket * v = (WebSocket *) ctx;
        if(what & BEV_EVENT_CONNECTED) {
            bufferevent_enable(bev, EV_WRITE);
        } else {
            v->onClose(nullptr);
        }
    }
    
    void WebSocket_evdns_cb (int result, char type, int count, int ttl, void *addresses, void *arg) {
        
        WebSocket * v = (WebSocket *) arg;
        
        struct in_addr * addr = (struct in_addr *) addresses;
        
        if(result == DNS_ERR_NONE && count > 0) {
            v->onResolve(addr);
        } else {
            v->onClose("域名解析错误");
        }
    }

    void WebSocket::onResolve(struct in_addr * addr){
        if(_state == WebSocketStateNone) {
            _state = WebSocketStateResolve;
            _addr.sin_addr = * addr;
            bufferevent_socket_connect(_bev, (struct sockaddr *) &_addr, sizeof(struct sockaddr_in));
        }
    }
    
    void WebSocket::onConnected() {
        if(_state == WebSocketStateResolve) {
            _state = WebSocketStateConnected;
            bufferevent_enable(_bev, EV_WRITE);
        }
    }
    
    void WebSocket::onWritting() {
        if(_state == WebSocketStateOpened) {
            
        }
    }
    
    void WebSocket::onReading() {
        
        if(_state == WebSocketStateConnected) {
            
            evbuffer * data = bufferevent_get_input(_bev);
            
            char * s = (char *) EVBUFFER_DATA(data);
            char * e = s + EVBUFFER_LENGTH(data);
            char * p = s;
            int n = 0;
            
            while(p != e) {
                if(*p == '\r') {
                    
                } else if(*p == '\n') {
                    n++;
                    if(n == 2) {
                        p ++;
                        break;
                    }
                } else {
                    n = 0;
                }
                p ++ ;
            }
            
            if(n == 2) {
                
                int code = 0;
                
                sscanf(s, "HTTP/1.1 %d",&code);
                
                evbuffer_drain(data, p - s);
                
                if(code == 101) {
                    onOpen();
                    return;
                } else {
                    onClose("服务错误");
                    return ;
                }

            }
            
            bufferevent_enable(_bev, EV_READ);
            
        } else if(_state == WebSocketStateOpened) {
            
            
            bufferevent_enable(_bev, EV_READ);
        }
        
    }
    
    void WebSocket::onOpen() {
        if(_state == WebSocketStateConnected) {
            _state = WebSocketStateOpened;
            onReading();
            if(EVBUFFER_LENGTH(bufferevent_get_output(_bev)) > 0) {
                bufferevent_enable(_bev, EV_WRITE);
            }
        }
    }
    
    void WebSocket::onClose(kk::CString errmsg) {
        if(_state != WebSocketStateClosed) {
            _state = WebSocketStateClosed;
            if(_bev != nullptr) {
                int fd = bufferevent_getfd(_bev);
                if(fd != -1) {
                    evutil_closesocket(fd);
                }
                bufferevent_free(_bev);
                _bev = nullptr;
            }
        }
    }
    
 
    void WebSocket::send(void * data,size_t n) {
        
        /*
        uint64_t offset = 2; //how many bytes do we need to skip for the header
        uint8_t *bytes = (uint8_t*)[data bytes];
        uint64_t dataLength = data.length;
        NSMutableData *frame = [[NSMutableData alloc] initWithLength:(NSInteger)(dataLength + KKMaxFrameSize)];
        uint8_t *buffer = (uint8_t*)[frame mutableBytes];
        buffer[0] = KKFinMask | code;
        if(dataLength < 126) {
            buffer[1] |= dataLength;
        } else if(dataLength <= UINT16_MAX) {
            buffer[1] |= 126;
            *((uint16_t *)(buffer + offset)) = CFSwapInt16BigToHost((uint16_t)dataLength);
            offset += sizeof(uint16_t);
        } else {
            buffer[1] |= 127;
            *((uint64_t *)(buffer + offset)) = CFSwapInt64BigToHost((uint64_t)dataLength);
            offset += sizeof(uint64_t);
        }
        BOOL isMask = YES;
        if(isMask) {
            buffer[1] |= KKMaskMask;
            uint8_t *mask_key = (buffer + offset);
            (void)SecRandomCopyBytes(kSecRandomDefault, sizeof(uint32_t), (uint8_t *)mask_key);
            offset += sizeof(uint32_t);
            
            for (size_t i = 0; i < dataLength; i++) {
                buffer[offset] = bytes[i] ^ mask_key[i % sizeof(uint32_t)];
                offset += 1;
            }
        } else {
            for(size_t i = 0; i < dataLength; i++) {
                buffer[offset] = bytes[i];
                offset += 1;
            }
        }
        */
        
    }
    
    void WebSocket::send(kk::CString text) {
        
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
        
        memset(&_addr, 0, sizeof(_addr));
        
        _addr.sin_family = AF_INET;
        _addr.sin_port = htons(port);
        
        _bev = bufferevent_new(-1, WebSocket_data_rd, WebSocket_data_wd, WebSocket_event_cb, this);
        
        bufferevent_setwatermark(_bev, EV_READ, 0, MAX_BUF_SIZE);
        
        evbuffer * data = bufferevent_get_output(_bev);
        
        evbuffer_add_printf(data, "GET %s HTTP/1.1\r\n", path.c_str());
        evbuffer_add_printf(data, "Host: %s\r\n",host.c_str());
        evbuffer_add_printf(data, "Upgrade: %s\r\n","websocket");
        evbuffer_add_printf(data, "Connection: %s\r\n","Upgrade");
        evbuffer_add_printf(data, "Origin: %s\r\n",origin.c_str());
        evbuffer_add_printf(data, "Sec-WebSocket-Key: %s\r\n",Sec_WebSocket_Key);
        evbuffer_add_printf(data, "Sec-WebSocket-Version: %s\r\n","13");
        evbuffer_add_printf(data, "\r\n");
    
        evdns_base_resolve_ipv4(dns, evhttp_uri_get_host(uri), 0, WebSocket_evdns_cb, this);
        
        return true;
    }
    
    duk_ret_t WebSocket::duk_on(duk_context * ctx) {
        
        return 0;
    }
    
    duk_ret_t WebSocket::duk_close(duk_context * ctx) {
        
        return 0;
    }
    
}

