//
//  kk-websocket.h
//  app
//
//  Created by 张海龙 on 2018/5/5.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef kk_websocket_h
#define kk_websocket_h

#include "kk-event.h"
#include "kk-script.h"
#include <libwebsockets.h>
#include <uv.h>

namespace kk {
    
    class WebSocketContext :  public Object {
    public:
        WebSocketContext(uv_loop_t * loop);
        virtual ~WebSocketContext();
        virtual struct lws_context * lwsContext();
        virtual void openlibs(kk::script::Context * ctx);
        static WebSocketContext * GetContext(kk::script::Context * ctx);
    
    protected:
        struct lws_context * _lwsContext;
        uv_idle_t _idle;
    };
    

    struct WebSocketData;
    
    class WebSocket : public EventEmitter {
    public:
        WebSocket();
        virtual ~WebSocket();
        virtual Boolean open(struct lws_context * lwsContext,CString url,CString protocol);
        virtual void close();
        virtual void send(CString text);
        virtual void send(void * data, size_t size);
        virtual CString errmsg();
        virtual Boolean isClosed();
        virtual size_t size();
        virtual void * data();
        
        virtual duk_ret_t duk_on(duk_context *ctx);
        virtual duk_ret_t duk_close(duk_context *ctx);
        virtual duk_ret_t duk_send(duk_context *ctx);
        
        DEF_SCRIPT_CLASS
    protected:
        struct lws * _wsi;
        kk::String _errmsg;
        kk::Weak _context;
        void * _data;
        size_t _size;
        std::list<WebSocketData *> _datas;
        size_t _sn;
        
        virtual void onOpen();
        virtual void onError(kk::CString errmsg);
        virtual void onClose();
        virtual void onData(void * data,size_t size);
        virtual Boolean onSend();
        
        friend int WebSocketContextCB(struct lws *wsi, enum lws_callback_reasons reason,
                                      void *user, void *in, size_t len);
        
    };
    
}

#endif /* kk_websocket_hpp */
