//
//  kk-http.h
//  KKGame
//
//  Created by 张海龙 on 2018/5/6.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef kk_http_h
#define kk_http_h

#include "kk-event.h"
#include "kk-script.h"
#include <libwebsockets.h>
#include <uv.h>

namespace kk {
    
    class IHttpTask : public Object , public kk::script::IObject {
    public:
        virtual void cancel() = 0;
    };
    
    class HttpResponse : public Object {
    public:
        HttpResponse(kk::Int status);
        virtual ~HttpResponse();
        virtual kk::CString get(kk::CString key);
        virtual void set(kk::CString key,kk::CString value);
        virtual std::map<kk::String,kk::String> & headers();
        virtual void * body(size_t * size);
        virtual void setBody(void * body,size_t size);
    protected:
        kk::Int _status;
        std::map<kk::String,kk::String> _headers;
        void * _body;
        size_t _size;
    };
    
    class HttpOptions : public EventEmitter {
    public:
        HttpOptions();
        virtual ~HttpOptions();
        virtual kk::CString url();
        virtual void setUrl(kk::CString url);
        virtual kk::CString method();
        virtual void setMethod(kk::CString method);
        virtual kk::CString get(kk::CString key);
        virtual void set(kk::CString key,kk::CString value);
        virtual std::map<kk::String,kk::String> & headers();
        virtual void * body(size_t * size);
        virtual void setBody(void * body,size_t size);
        virtual HttpResponse * response();
        virtual void setResponse(HttpResponse * response);
        virtual kk::CString errmsg();
        virtual void setErrmsg(kk::CString errmsg);
    protected:
        kk::Strong _response;
        kk::String _url;
        kk::String _method;
        kk::String _errmsg;
        std::map<kk::String,kk::String> _headers;
        unsigned char * _body;
        size_t _size;
    };
    
    class Http :  public Object ,public kk::script::IObject {
    public:
        Http(uv_loop_t * loop,kk::CString proxy);
        virtual ~Http();
        virtual struct lws_context * lwsContext();
        virtual kk::Strong send(HttpOptions * options);
        virtual void openlibs(kk::script::Context * ctx);
        static Http * GetContext(kk::script::Context * ctx);
        
        DEF_SCRIPT_CLASS
        
        virtual duk_ret_t duk_send(duk_context * ctx);
        
        static CString userAgent();
        static void setUserAgent(CString userAgent);
        
    protected:
        Http();
        struct lws_context * _lwsContext;
        uv_idle_t _idle;
    };
    
}

#endif /* kk_http_hpp */
