//
//  kk-websocket.cc
//  app
//
//  Created by 张海龙 on 2018/5/5.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include "kk-config.h"
#include "kk-websocket.h"
#include "kk-string.h"

namespace kk {
    
    int WebSocketContextCB(struct lws *wsi, enum lws_callback_reasons reason,
                      void *user, void *in, size_t len) {
        
        WebSocket * v = (WebSocket *) user;
        WebSocketContext * context = (WebSocketContext *) lws_context_user(lws_get_context(wsi));
        
        if(v && context) {
            
            switch (reason) {
                case LWS_CALLBACK_WSI_CREATE:
                    v->retain();
                    break;
                case LWS_CALLBACK_WSI_DESTROY:
                    v->release();
                    break;
                case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
                {
                    if(!v->isClosed()) {
                        if(in == nullptr) {
                            v->onError("无法连接到服务器");
                        } else {
                            v->onError((kk::CString) in);
                        }
                    }
                }
                    break;
                case LWS_CALLBACK_CLOSED_CLIENT_HTTP:
                {
                    if(!v->isClosed()) {
                        v->onClose();
                    }
                }
                    break;
                case LWS_CALLBACK_CLIENT_ESTABLISHED:
                {
                    if(!v->isClosed()) {
                        v->onOpen();
                        lws_callback_on_writable(wsi);
                    }
                }
                    break;
                case LWS_CALLBACK_CLIENT_RECEIVE:
                {
                    if(!v->isClosed()) {
                        v->onData(in,len);
                    }
                }
                    break;
                case LWS_CALLBACK_CLIENT_WRITEABLE:
                {
                    if(!v->isClosed()) {
                        if(v->onSend()) {
                            lws_callback_on_writable(wsi);
                        }
                    }
                }
                    break;
                default:
                    break;
            }
        }
        
        return 0;
    }
    
    static const struct lws_protocols protocols[] = {
        {
            "_",
            WebSocketContextCB,
            0,
            0,
        },
        { NULL, NULL, 0, 0 }
    };
    
    static void WebSocketContext_idle_cb(uv_idle_t* handle){
        WebSocketContext * ctx = (WebSocketContext *) handle->data;
        lws_service(ctx->lwsContext(), 30);
    }
    
    WebSocketContext::WebSocketContext(uv_loop_t * loop):_lwsContext(nullptr) {
        
        lws_context_creation_info info;
        
        memset(&info, 0, sizeof(lws_context_creation_info));
        
        info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
        info.port = CONTEXT_PORT_NO_LISTEN;
        info.protocols = protocols;
        info.user = this;
        info.uid = -1;
        info.gid = -1;
        
        _lwsContext = lws_create_context(&info);
    
        if(_lwsContext){
            uv_idle_init(loop, &_idle);
            _idle.data = this;
            uv_idle_start(&_idle, WebSocketContext_idle_cb);
        }
        
    }
    
    WebSocketContext::~WebSocketContext() {
        if(_lwsContext) {
            uv_idle_stop(&_idle);
            lws_context_destroy(_lwsContext);
        }
    }
    
    struct lws_context * WebSocketContext::lwsContext() {
        return _lwsContext;
    }
    
    void WebSocketContext::openlibs(kk::script::Context * ctx) {
        ctx->setObject("__WebSocketContext", this);
    }
    
    WebSocketContext * WebSocketContext::GetContext(kk::script::Context * ctx) {
        return (WebSocketContext *) ctx->object("__WebSocketContext");
    }
    
    
    static duk_ret_t WebSocketAllocFunc(duk_context * ctx) {
        
        int top = duk_get_top(ctx);
        
        kk::CString url = nullptr;
        kk::CString protocol = nullptr;
        
        if(top >0 && duk_is_string(ctx, -top)) {
            url = duk_to_string(ctx, -top);
        }
        
        if(top >1 && duk_is_string(ctx, -top+1)) {
            protocol = duk_to_string(ctx, -top+1);
        }
        
        if(url) {
            
            kk::Strong v = new WebSocket();
            
            WebSocket * s = v.as<WebSocket>();
            
            WebSocketContext * wsContext = WebSocketContext::GetContext(kk::script::GetContext(ctx));
            
            if(s->open(wsContext->lwsContext(), url, protocol)) {
                
                kk::script::PushObject(ctx, s);
                
                return 1;
            }
        
        }
        
        return 0;
    }
    
    IMP_SCRIPT_CLASS_BEGIN(nullptr, WebSocket, WebSocket)
    
    static kk::script::Method methods[] = {
        {"close",(kk::script::Function) &WebSocket::duk_close},
        {"on",(kk::script::Function) &WebSocket::duk_on},
        {"send",(kk::script::Function) &WebSocket::duk_send},
    };
    
    kk::script::SetMethod(ctx, -1, methods, sizeof(methods) / sizeof(kk::script::Method));
    
    duk_push_string(ctx, "alloc");
    duk_push_c_function(ctx, WebSocketAllocFunc, 2);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE |
                 DUK_DEFPROP_HAVE_WRITABLE | DUK_DEFPROP_CLEAR_WRITABLE |
                 DUK_DEFPROP_HAVE_ENUMERABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    
    IMP_SCRIPT_CLASS_END
    
    
    struct WebSocketData {
        unsigned char * data;
        size_t size;
        lws_write_protocol type;
        WebSocketData(CString text) {
            type = LWS_WRITE_TEXT;
            size = strlen(text) + 1;
            data = new unsigned char[size + LWS_SEND_BUFFER_PRE_PADDING + LWS_SEND_BUFFER_POST_PADDING];
            memcpy(data + LWS_SEND_BUFFER_PRE_PADDING ,text,size);
        }
        WebSocketData(void * data, size_t size) {
            type = LWS_WRITE_BINARY;
            this->size = size;
            this->data = new unsigned char[size + LWS_SEND_BUFFER_PRE_PADDING + LWS_SEND_BUFFER_POST_PADDING];
            memcpy(this->data + LWS_SEND_BUFFER_PRE_PADDING,data,size);
        }
        ~WebSocketData() {
            delete [] data;
        }
    };
    
    WebSocket::WebSocket(): _wsi(nullptr),_data(nullptr),_size(0),_sn(0) {
        
    }
    
    WebSocket::~WebSocket() {
        std::list<WebSocketData *>::iterator i = _datas.begin();
        while(i != _datas.end()) {
            WebSocketData * v = *i;
            delete v;
            i ++;
        }
    }
    
    CString WebSocket::errmsg() {
        return _errmsg.c_str();
    }
    
    enum WebSocketOnType {
        WebSocketOnTypeData,WebSocketOnTypeError,WebSocketOnTypeClose,WebSocketOnTypeOpen
    };
    
    class WebSocketOnFunc : public EventFunction {
    public:
        WebSocketOnFunc(WebSocketOnType type,kk::script::Object * fn){
            _type = type;
            _fn = fn;
        }
        void call(EventEmitter * emitter,String & name,Event * event,void * context) {
            
            WebSocket * v = dynamic_cast<WebSocket *>(emitter);
            kk::script::Object * fn = _fn.as<kk::script::Object>();
            duk_context * ctx = fn->jsContext();
            
            if(ctx == nullptr) {
                return;
            }
            
            switch (_type) {
                    
                case WebSocketOnTypeData:
                {
                    duk_push_heapptr(ctx, fn->heapptr());
                    
                    void * d = duk_push_fixed_buffer(ctx, v->size());
                    
                    memcpy(d, v->data(), v->size());
                    
                    duk_push_buffer_object(ctx, -1, 0, v->size(), DUK_BUFOBJ_UINT8ARRAY);
                    
                    duk_remove(ctx, -2);
                    
                    if(DUK_EXEC_SUCCESS != duk_pcall(ctx, 1)) {
                        kk::script::Error(ctx, -1);
                    }
                    
                    duk_pop(ctx);
                }
                    break;
                case WebSocketOnTypeClose:
                {
                    duk_push_heapptr(ctx, fn->heapptr());
            
                    duk_push_string(ctx, v->errmsg());
                    
                    if(DUK_EXEC_SUCCESS != duk_pcall(ctx, 1)) {
                        kk::script::Error(ctx, -1);
                    }
                    
                    duk_pop(ctx);
                }
                    break;
                case WebSocketOnTypeError:
                {
                    duk_push_heapptr(ctx, fn->heapptr());
                    
                    duk_push_string(ctx, v->errmsg());
                    
                    if(DUK_EXEC_SUCCESS != duk_pcall(ctx, 1)) {
                        kk::script::Error(ctx, -1);
                    }
                    
                    duk_pop(ctx);
                }
                    break;
                case WebSocketOnTypeOpen:
                {
                    duk_push_heapptr(ctx, fn->heapptr());
   
                    if(DUK_EXEC_SUCCESS != duk_pcall(ctx, 0)) {
                        kk::script::Error(ctx, -1);
                    }
                    
                    duk_pop(ctx);
                }
                    break;
                default:
                    break;
            }
        }
    protected:
        WebSocketOnType _type;
        kk::Strong _fn;
    };
    
    
    duk_ret_t WebSocket::duk_on(duk_context * ctx) {
    
        int top = duk_get_top(ctx);
        
        kk::CString name = nullptr;
        kk::Strong fn;
        
        if(top >0 && duk_is_string(ctx, -top)) {
            name = duk_to_string(ctx, -top);
        }
        
        if(top >1 && duk_is_function(ctx, -top +1)) {
            fn = new kk::script::Object(kk::script::GetContext(ctx),-top+1);
        }
        
        if(name) {
            off(name, (EventFunction *) nullptr, ctx);
            if(fn.get()) {
                if(kk::CStringEqual(name, "data")) {
                    kk::Strong vv = new WebSocketOnFunc(WebSocketOnTypeData,fn.as<kk::script::Object>());
                    on(name, vv.as<EventFunction>(), ctx);
                } else if(kk::CStringEqual(name, "close")) {
                    kk::Strong vv = new WebSocketOnFunc(WebSocketOnTypeClose,fn.as<kk::script::Object>());
                    on(name, vv.as<EventFunction>(), ctx);
                } else if(kk::CStringEqual(name, "error")) {
                    kk::Strong vv = new WebSocketOnFunc(WebSocketOnTypeError,fn.as<kk::script::Object>());
                    on(name, vv.as<EventFunction>(), ctx);
                } else if(kk::CStringEqual(name, "open")) {
                    kk::Strong vv = new WebSocketOnFunc(WebSocketOnTypeOpen,fn.as<kk::script::Object>());
                    on(name, vv.as<EventFunction>(), ctx);
                }
            }
        }
        
        return 0;
    }
    
    duk_ret_t WebSocket::duk_close(duk_context * ctx) {
        close();
        return 0;
    }
    
    duk_ret_t WebSocket::duk_send(duk_context *ctx) {
        
        int top = duk_get_top(ctx);
        
        if(top >0 ) {
            if(duk_is_string(ctx, - top)) {
                send(duk_to_string(ctx, -top));
            } else if(duk_is_buffer_data(ctx, -top)) {
                duk_size_t n;
                void * bytes = duk_get_buffer_data(ctx, - top, &n);
                send(bytes, n);
            }
        }
        
        
        return 0;
    }
    
    Boolean WebSocket::open(struct lws_context * lwsContext,CString url,CString protocol) {

        kk::String u = url;
        
        kk::CString proto= nullptr;
        kk::CString addr = nullptr;
        kk::CString cpath = nullptr;
        kk::Int port = 0;
        kk::String host;
        kk::String path = "/";
        kk::String origin;
        
        if(0 != lws_parse_uri((char *) u.c_str(), &proto, &addr, &port, &cpath)) {
            return false;
        }
        
        path.append(cpath);
        
        origin.append(proto);
        origin.append("://");
        
        host.append(addr);
        
        if(port == 0) {
            
            if(kk::CStringEqual(proto, "wss") || kk::CStringEqual(proto, "https")) {
                port = 443;
            } else {
                port = 80;
            }
            
        } else{
            
            host.append(":");
            char fmt[64];
            snprintf(fmt, sizeof(fmt), "%d",port);
            host.append(fmt);
            
        }
        
        origin.append(host);
       
        struct lws_client_connect_info i;
        
        memset(&i, 0, sizeof i);
        
        i.context = lwsContext;
        i.port = port;
        i.address = addr;
        i.path = path.c_str();
        i.host = host.c_str();
        i.origin = origin.c_str();
        
        if(kk::CStringEqual(proto, "wss") || kk::CStringEqual(proto, "https")) {
            i.ssl_connection = LCCSCF_USE_SSL;
        }
        
        i.protocol = protocol;
        i.userdata = this;
        i.pwsi = & _wsi;
    
        lws_client_connect_via_info(&i);
        
        return _wsi != nullptr;
        
    }
    
    void WebSocket::close() {
        if(_wsi) {
            lws_close_reason(_wsi,LWS_CLOSE_STATUS_GOINGAWAY,NULL,0);
            _wsi = nullptr;
        }
    }
    
    void WebSocket::send(CString text) {
        if(text) {
            kk::Boolean b = _datas.empty() && _wsi;
            _datas.push_back(new WebSocketData(text));
            if(b) {
                lws_callback_on_writable(_wsi);
            }
        }
    }
    
    void WebSocket::send(void * data, size_t size) {
        if(data && size > 0) {
            kk::Boolean b = _datas.empty() && _wsi;
            _datas.push_back(new WebSocketData(data,size));
            if(b) {
                lws_callback_on_writable(_wsi);
            }
        }
    }
    
    Boolean WebSocket::isClosed() {
        return _wsi == nullptr;
    }
    
    size_t WebSocket::size() {
        return _size;
    }
    
    void * WebSocket::data() {
        return _data;
    }
    
    void WebSocket::onError(kk::CString errmsg) {
        
        _errmsg = errmsg;
        
        if(this->has("error")) {
            kk::Strong v = new Event();
            Event * e = v.as<Event>();
            emit("error", e);
        }
    }
    
    void WebSocket::onClose() {
        
        _wsi = nullptr;
        
        if(this->has("close")) {
            kk::Strong v = new Event();
            Event * e = v.as<Event>();
            emit("close", e);
        }
        
    }
    
    void WebSocket::onData(void * data,size_t size){
        
        _data = data;
        _size = size;
        
        if(this->has("data")) {
            kk::Strong v = new Event();
            Event * e = v.as<Event>();
            emit("data", e);
        }
        
        _data = nullptr;
        _size = 0;
        
    }

    void WebSocket::onOpen() {
        if(this->has("open")) {
            kk::Strong v = new Event();
            Event * e = v.as<Event>();
            emit("open", e);
        }
    }
    
    Boolean WebSocket::onSend() {
        
        if(_wsi == nullptr) {
            return true;
        }
        
        std::list<WebSocketData *>::iterator i = _datas.begin();
        
        if(i != _datas.end()) {
            
            WebSocketData * data = *i;
            
            int n = lws_write(_wsi, data->data + LWS_SEND_BUFFER_PRE_PADDING, data->size, data->type);
            
            if(n == -1 || n != data->size) {
                close();
                return false;
            }
    
            _datas.erase(i);
            
        }
        
        return !_datas.empty();
    }
    
}
