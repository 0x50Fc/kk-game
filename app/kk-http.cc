//
//  kk-http.cc
//  KKGame
//
//  Created by 张海龙 on 2018/5/6.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include "kk-config.h"
#include "kk-http.h"
#include "kk-string.h"
#include "kk-script.h"
#include <zlib.h>

#define DATA_SIZE 4096
#define DATA_MIN_SIZE 1024

namespace kk {
    
    HttpResponse::HttpResponse(kk::Int status)
        :_status(status),_body(nullptr),_size(0) {
        
    }
    
    HttpResponse::~HttpResponse() {
    }
    
    kk::CString HttpResponse::get(kk::CString key) {
        std::map<kk::String,kk::String>::iterator i = _headers.find(key);
        if(i != _headers.end()) {
            return i->second.c_str();
        }
        return nullptr;
    }
    
    void HttpResponse::set(kk::CString key,kk::CString value) {
        _headers[key] = value;
    }
    
    std::map<kk::String,kk::String> & HttpResponse::headers() {
        return _headers;
    }
    
    void * HttpResponse::body(size_t * size) {
        if(size) {
            *size = _size;
        }
        return _body;
    }
    
    void HttpResponse::setBody(void * body,size_t size) {
        _body = body;
        _size = size;
    }
    
    enum HttpContentEncoding {
        HttpContentEncodingNone,
        HttpContentEncodingGzip
    };
    
    enum HttpBodyType {
        HttpBodyTypeString,
        HttpBodyTypeBytes,
        HttpBodyTypeJSON
    };
    
    class HttpTask : public IHttpTask {
    public:
        
        HttpTask(HttpOptions * options)
            :_wsi(nullptr),_options(options)
            ,_size(0),_n(0),_data(nullptr)
            ,_encoding(HttpContentEncodingNone)
            ,js_bodyType(HttpBodyTypeString) {
        }
        
        virtual ~HttpTask() {
            
            if(_data) {
                free(_data);
            }
            
            if(_encoding == HttpContentEncodingGzip) {
                inflateEnd(&_gzip);
            }
            
        }
        
        
        virtual HttpOptions * options() {
            return _options.as<HttpOptions>();
        }
        
        virtual void cancel() {
            if(_wsi) {
                lws_close_reason(_wsi,LWS_CLOSE_STATUS_GOINGAWAY,NULL,0);
                _wsi = nullptr;
            }
        }
        
        virtual Boolean isCanceled() {
            return _wsi == nullptr;
        }
        
        
        virtual void onOpen() {
            
            kk::Int status = lws_http_client_http_response(_wsi);
   
            HttpOptions * options = _options.as<HttpOptions>();
            
            HttpResponse * resp  = new HttpResponse(status);
            
            options->setResponse(resp);
            
            
            char v[255];
            
            if(-1 != lws_hdr_copy(_wsi, v, sizeof(v) - 1, WSI_TOKEN_HTTP_CONTENT_ENCODING)) {
                if(strncmp(v, "gzip", sizeof(v)) == 0) {
                    _encoding = HttpContentEncodingGzip;
                }
            }
            
            if(-1 != lws_hdr_copy(_wsi, v, sizeof(v) - 1, WSI_TOKEN_HTTP_CONTENT_TYPE)) {
                resp->set("Content-Type", v);
            }
            
            if(-1 != lws_hdr_copy(_wsi, v, sizeof(v) - 1, WSI_TOKEN_HTTP_CONTENT_LENGTH)) {
                resp->set("Content-Length", v);
            }
            
            if(-1 != lws_hdr_copy(_wsi, v, sizeof(v) - 1, WSI_TOKEN_HTTP_LOCATION)) {
                resp->set("Location", v);
            }
            
            if(_encoding == HttpContentEncodingGzip) {
                memset(&_gzip, 0, sizeof(_gzip));
                if(inflateInit2(&_gzip,(15+32)) != Z_OK) {
                    kk::Log("[HTTP] [GZIP] [ERROR] inflateInit2");
                    _encoding = HttpContentEncodingNone;
                }
            }
            
            kk::Strong e = new Event();
            options->emit("open", e.as<Event>());
        }
        
        virtual void onError(kk::CString errmsg) {
            kk::Log("[ERROR] %s",errmsg);
            
            HttpOptions * options = _options.as<HttpOptions>();
            options->setErrmsg(errmsg);
            kk::Strong e = new Event();
            options->emit("error", e.as<Event>());
            
            {
                kk::script::Object * v = js_onfail.as<kk::script::Object>();
                
                if(v) {
                    
                    duk_context * ctx = v->jsContext();
                    
                    duk_push_heapptr(ctx, v->heapptr());
                    
                    if(duk_is_function(ctx, -1)) {
                        
                        duk_push_string(ctx, errmsg);
                        
                        if(DUK_EXEC_SUCCESS != duk_pcall(ctx, 1)) {
                            kk::script::Error(ctx, -1);
                        }
                    }
                    
                    duk_pop(ctx);
                }
            }
            
        }
        
        virtual void onData(void * in,size_t len) {
            
            if(_encoding == HttpContentEncodingGzip) {
                
                _gzip.next_in = (Bytef *) in;
                _gzip.avail_in = len;
                
                bool done = 0;
                
                do {
                    
                    if(_data == nullptr) {
                        _size = DATA_SIZE;
                        _data = (char *) malloc(_size);
                    } else if(_size < DATA_MIN_SIZE + _n) {
                        _size = _size + DATA_SIZE;
                        _data = (char *) realloc(_data, _size);
                    }
                    
                    size_t n = _size - _n;
                    _gzip.next_out = (Bytef *) _data + _n;
                    _gzip.avail_out = n;
                    
                    int r = inflate(&_gzip,Z_SYNC_FLUSH);
                    
                    if(r == Z_STREAM_END){
                        done = 1;
                    }
                    else if(r != Z_OK){
                        break;
                    }
                    
                    _n = _n + n - _gzip.avail_out;
                    
                } while(!done && _gzip.avail_in > 0);
                
            } else {
            
                if(_data == nullptr) {
                    _size = len + 1;
                    _data = (char *) malloc(_size);
                } else if(_size < len + _n) {
                    _size = MAX(_size + DATA_SIZE, _n + len + 1);
                    _data = (char *) realloc(_data, _size);
                }
                
                memcpy(_data + _n, in, len);
                
                _n += len;
            }
        }
        
        virtual void onClose() {
            _wsi = nullptr;
            HttpOptions * options = _options.as<HttpOptions>();
            kk::Strong e = new Event();
            options->emit("close", e.as<Event>());
        }
        
        virtual Boolean onRead() {
            char data[DATA_SIZE];
            char * p = data + LWS_PRE;
            int n = DATA_SIZE - LWS_PRE;
            int r = lws_http_client_read(_wsi, &p, &n);
            return r >= 0;
        }
        
        virtual void onDone() {
            
            if(_n < _size && _data) {
                _data[_n] = 0;
            }
            
            HttpOptions * options = _options.as<HttpOptions>();
            HttpResponse * resp = options->response();
            resp->setBody(_data, _n);
            
            kk::Strong e = new Event();
            options->emit("load", e.as<Event>());
            
            {
                kk::script::Object * v = js_onload.as<kk::script::Object>();
                
                if(v) {
                    
                    duk_context * ctx = v->jsContext();
                    
                    duk_push_heapptr(ctx, v->heapptr());
                    
                    if(duk_is_function(ctx, -1)) {
                        
                        if(js_bodyType == HttpBodyTypeBytes) {
                            
                            void * d = duk_push_fixed_buffer(ctx, _n);
                            
                            memcpy(d, _data, _n);
                            
                            duk_push_buffer_object(ctx, -1, 0, _n, DUK_BUFOBJ_UINT8ARRAY);
                            
                            duk_remove(ctx, -2);
                            
                        } else if(js_bodyType == HttpBodyTypeJSON) {
                            duk_push_lstring(ctx, _data, _n);
                            duk_json_decode(ctx, -1);
                        } else {
                            duk_push_lstring(ctx, _data, _n);
                        }
                        
                        if(DUK_EXEC_SUCCESS != duk_pcall(ctx, 1)) {
                            kk::script::Error(ctx, -1);
                        }
                    }
                    
                    duk_pop(ctx);
                }
            }
        }
        
        virtual struct lws ** pwsi() {
            return & _wsi;
        }
        
        virtual struct lws * wsi() {
            return _wsi;
        }
        
        virtual duk_ret_t duk_cancel(duk_context* ctx) {
            cancel();
            return 0;
        }
        
        DEF_SCRIPT_CLASS
    public:
        HttpBodyType js_bodyType;
        kk::Strong js_onload;
        kk::Strong js_onfail;
    protected:
        HttpTask():_wsi(nullptr),_options(nullptr),_size(0),_n(0),_data(nullptr),_encoding(HttpContentEncodingNone) {
        }
        kk::Strong _options;
        kk::Strong _response;
        struct lws *_wsi;
        char * _data;
        size_t _size;
        size_t _n;
        HttpContentEncoding _encoding;
        z_stream _gzip;
    };
    
    IMP_SCRIPT_CLASS_BEGIN(nullptr, HttpTask, HttpTask)
    
    static kk::script::Method methods[] = {
        {"cancel",(kk::script::Function) &HttpTask::duk_cancel},
    };
    
    kk::script::SetMethod(ctx, -1, methods, sizeof(methods) / sizeof(kk::script::Method));
    
    duk_push_string(ctx, "alloc");
    duk_del_prop(ctx, -2);
    
    IMP_SCRIPT_CLASS_END
    
    
    int HttpCB(struct lws *wsi, enum lws_callback_reasons reason,
                           void *user, void *in, size_t len) {
        
        HttpTask * v = (HttpTask *) user;
        Http * http = (Http *) lws_context_user(lws_get_context(wsi));
        kk::HttpOptions * options = v == nullptr ? nullptr : v->options();
        
        switch (reason) {
                
            case LWS_CALLBACK_WSI_CREATE:
                v->retain();
                break;
            case LWS_CALLBACK_WSI_DESTROY:
                v->release();
                break;
            case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
            {
                if(!v->isCanceled()) {
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
                if(!v->isCanceled()) {
                    v->onClose();
                }
            }
                break;
                
            case LWS_CALLBACK_ESTABLISHED_CLIENT_HTTP:
            {
                if(!v->isCanceled()) {
                    v->onOpen();
                }
            }
                break;
            case LWS_CALLBACK_RECEIVE_CLIENT_HTTP_READ:
                if(v->isCanceled()) {
                    return 1;
                }
                
                v->onData(in, len);
                
                return 0; /* don't passthru */
                
            case LWS_CALLBACK_RECEIVE_CLIENT_HTTP:
                
                if(v->isCanceled()) {
                    return 1;
                }
                
                if(!v->onRead()){
                    return 1;
                }
                
                return 0; /* don't passthru */
                
            case LWS_CALLBACK_COMPLETED_CLIENT_HTTP:
                if(v->isCanceled()) {
                    return 1;
                }
                
                v->onDone();
                
                break;
            case LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER:
                if(v->isCanceled()) {
                    return 1;
                }
                
                {
                    uint8_t **up = (uint8_t **)in;
                    uint8_t *uend = *up + len - 1;

                    std::map<kk::String, kk::String> & h = options->headers();
                    std::map<kk::String, kk::String>::iterator i = h.begin();

                    kk::String key;
                    
                    while(i != h.end() && * up < uend) {
                        
                        key.clear();
                        key.append(i->first);
                        key.append(":");
                        
                        lws_add_http_header_by_name(wsi, (uint8_t *) key.c_str(), (uint8_t *) i->second.c_str(), i->second.size(), up, uend);

                        i ++;
                    }

                    if(kk::CStringEqual(options->method(), "POST")) {
                        lws_callback_on_writable(wsi);
                    }
                }
                
                break;
                
            case LWS_CALLBACK_CLIENT_HTTP_WRITEABLE:

                if(v->isCanceled()) {
                    return 1;
                }
                
            {
                size_t n;
                void * b = options->body(&n);
                lws_write(wsi, (uint8_t *) b, n, LWS_WRITE_HTTP);
            }
                return 0;
                
            default:
                break;
        }
        
        return lws_callback_http_dummy(wsi, reason, user, in, len);
    }
    
    static const struct lws_protocols protocols[] = {
        {
            "_",
            HttpCB,
            0,
            0,
        },
        { NULL, NULL, 0, 0 }
    };
    
    static void Http_idle_cb(uv_idle_t* handle){
        Http * http = (Http *) handle->data;
        lws_service(http->lwsContext(), 0);
    }
    
    Http::Http():_lwsContext(nullptr) {
    }
    
    Http::Http(uv_loop_t * loop,kk::CString proxy) {
        
        lws_context_creation_info info;
        
        memset(&info, 0, sizeof(lws_context_creation_info));
        
        info.options = LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
        info.port = CONTEXT_PORT_NO_LISTEN;
        info.protocols = protocols;
        info.user = this;
        info.uid = -1;
        info.gid = -1;
        info.http_proxy_address = proxy;
        
        _lwsContext = lws_create_context(&info);
        
        if(_lwsContext){
            uv_idle_init(loop, &_idle);
            _idle.data = this;
            uv_idle_start(&_idle, Http_idle_cb);
        }
    }
    
    Http::~Http() {
        if(_lwsContext) {
            uv_idle_stop(&_idle);
            lws_context_destroy(_lwsContext);
        }
    }
    
    struct lws_context * Http::lwsContext() {
        return _lwsContext;
    }
    
    kk::Strong Http::send(HttpOptions * options) {
        
        kk::String u = options->url();
        
        kk::CString proto= nullptr;
        kk::CString addr = nullptr;
        kk::CString cpath = nullptr;
        kk::Int port = 0;
        kk::String host;
        kk::String path;
        
        if(0 != lws_parse_uri((char *) u.c_str(), &proto, &addr, &port, &cpath)) {
            return (kk::Object *) nullptr;
        }
        
        if(cpath == nullptr || * cpath != '/') {
            path.append("/");
        }
        
        if(cpath) {
            path.append(cpath);
        }
        
        host.append(addr);
        
        if(port != 0) {

            host.append(":");
            char fmt[64];
            snprintf(fmt, sizeof(fmt), "%d",port);
            host.append(fmt);
            
        }
        
        struct lws_client_connect_info i;
        
        memset(&i, 0, sizeof i);
        
        i.context = _lwsContext;
        i.port = port;
        i.address = addr;
        i.path = path.c_str();
        i.host = host.c_str();
        i.origin = host.c_str();
 
        if(kk::CStringEqual(proto, "https")) {
            i.ssl_connection = LCCSCF_USE_SSL;
        }
        
        kk::Strong v = new HttpTask(options);
        
        HttpTask * httpTask = v.as<HttpTask>();
        
        i.method = options->method();
        i.protocol = "_";
        i.userdata = httpTask;
        i.pwsi = httpTask->pwsi();
        
        lws_client_connect_via_info(&i);
        
        if(httpTask->wsi()) {
            return v;
        }
        
        return (kk::Object *) nullptr;
    }
    
    void Http::openlibs(kk::script::Context * jsContext) {
        
        jsContext->setObject("http", this);
        
        duk_context * ctx = jsContext->jsContext();
        
        kk::script::SetPrototype(ctx, &Http::ScriptClass);
        
        duk_push_global_object(ctx);
        kk::script::PushObject(ctx, this);
        duk_put_prop_string(ctx, -2, "http");
        duk_pop(ctx);
        
    }
    
    IMP_SCRIPT_CLASS_BEGIN(nullptr, Http, Http)
    
    static kk::script::Method methods[] = {
        {"send",(kk::script::Function) &Http::duk_send},
    };
    
    kk::script::SetMethod(ctx, -1, methods, sizeof(methods) / sizeof(kk::script::Method));

    IMP_SCRIPT_CLASS_END
    
    static kk::String HttpUserAgent = "kk-http/1.0";
    
    CString Http::userAgent() {
        return HttpUserAgent.c_str();
    }
    
    void Http::setUserAgent(CString userAgent) {
        HttpUserAgent = userAgent;
    }
    
    duk_ret_t Http::duk_send(duk_context * ctx) {
        
        int top = duk_get_top(ctx);
        
        if(top >0 && duk_is_object(ctx, -top)) {
            
            HttpOptions * options = new HttpOptions();
            
            HttpBodyType bodyType = HttpBodyTypeString;
            kk::String url;
            
            {
                duk_get_prop_string(ctx, -top, "method");
                
                if(duk_is_string(ctx, -1)) {
                    options->setMethod(duk_to_string(ctx, -1));
                }
                
                duk_pop(ctx);
            }
            
            {
                duk_get_prop_string(ctx, -top, "type");
                
                if(duk_is_string(ctx, -1)) {
                    kk::CString v = duk_to_string(ctx, -1);
                    if(kk::CStringEqual(v, "json")) {
                        bodyType = HttpBodyTypeJSON;
                    } else if(kk::CStringEqual(v, "bytes")) {
                        bodyType = HttpBodyTypeBytes;
                    }
                }
                
                duk_pop(ctx);
            }
            
            {
                duk_get_prop_string(ctx, -top, "url");
                
                if(duk_is_string(ctx, -1)) {
                    url = duk_to_string(ctx, -1);
                }
                
                duk_pop(ctx);
            }
            
            if(kk::CStringEqual(options->method(),"GET")) {
                
                duk_get_prop_string(ctx, -top, "data");
                
                if(duk_is_object(ctx, -1)) {
                    
                    std::string::size_type i = url.find("?");
                    
                    if(i == std::string::npos) {
                        url.append("?");
                    } else if(i != url.size() -1) {
                        url.append("&");
                    }
                    
                    i = 0;
                    
                    duk_enum(ctx, -1, DUK_ENUM_INCLUDE_SYMBOLS);
                    
                    while(duk_next(ctx, -1, 1)) {
                        
                        kk::CString key = duk_to_string(ctx, -2);
                        
                        if(i != 0) {
                            url.append("&");
                        }
                        
                        url.append(key);
                        url.append("=");
                        
                        duk_get_global_string(ctx, "encodeURIComponent");
                        duk_dup(ctx, -2);
                        duk_call(ctx, 1);
                        
                        url.append(duk_to_string(ctx, -1));
                        
                        duk_pop_n(ctx,3);
                        
                    }
                    
                    duk_pop(ctx);
                    
                }
                
                duk_pop(ctx);
                
            }
            
            options->setUrl(url.c_str());
            
            if(kk::CStringEqual(options->method(),"POST")) {
                
                duk_get_prop_string(ctx, -top, "data");
                
                if(duk_is_object(ctx, -1)) {
                    
                    kk::String data;
                    
                    size_t i = 0;
                    
                    duk_enum(ctx, -1, DUK_ENUM_INCLUDE_SYMBOLS);
                    
                    while(duk_next(ctx, -1, 1)) {
                        
                        kk::CString key = duk_to_string(ctx, -2);
                        
                        if(i != 0) {
                            data.append("&");
                        }
                        
                        data.append(key);
                        data.append("=");
                        
                        duk_get_global_string(ctx, "encodeURIComponent");
                        duk_dup(ctx, -2);
                        duk_call(ctx, 1);
                        
                        data.append(duk_to_string(ctx, -1));
                        
                        duk_pop_n(ctx,3);
                        
                    }
                    
                    duk_pop(ctx);
                    
                    options->setBody((void *) data.data(), data.size());
                    options->set("Content-Type", "application/x-www-form-urlencoded");
                    
                } else if(duk_is_string(ctx, -1)) {
                    size_t n;
                    kk::CString data = duk_to_lstring(ctx, -1, &n);
                    options->setBody((void *) data, n);
                } else if(duk_is_buffer_data(ctx, -1)) {
                    duk_size_t n;
                    void * bytes = duk_get_buffer_data(ctx, -1, &n);
                    options->setBody(bytes, n);
                }
                
                duk_pop(ctx);
                
            }
            
            kk::Strong v = send(options);
            
            HttpTask * httpTask = v.as<HttpTask>();
            
            if(httpTask == nullptr) {
                
                duk_get_prop_string(ctx, -top, "onfail");
                
                if(duk_is_function(ctx, -1)) {
                    
                    duk_push_string(ctx, options->errmsg() == nullptr ? "错误的URL" : options->errmsg());
                    
                    if(duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
                        kk::script::Error(ctx, -1);
                    }
                }
                
                duk_pop(ctx);
                
                return 0;
                
            } else {
                
                httpTask->js_bodyType = bodyType;
                
                duk_get_prop_string(ctx, -top, "onload");
                
                if(duk_is_function(ctx, -1)) {
                    httpTask->js_onload = new kk::script::Object(kk::script::GetContext(ctx),-1);
                }
                
                duk_pop(ctx);
                
                duk_get_prop_string(ctx, -top, "onfail");
                
                if(duk_is_function(ctx, -1)) {
                    httpTask->js_onfail = new kk::script::Object(kk::script::GetContext(ctx),-1);
                }
                
                duk_pop(ctx);
                
                kk::script::PushObject(ctx, httpTask);
                
                return 1;
            }
            
        }
        
        return 0;
    }
    
    Http * Http::GetContext(kk::script::Context * ctx) {
        return (Http *) ctx->object("http");
    }
    
    HttpOptions::HttpOptions():_body(nullptr),_size(0) {
        setMethod("GET");
        _headers["Accept"] = "*/*";
        _headers["Accept-Charset"] = "utf-8";
        _headers["Accept-Encoding"] = "gzip";
        _headers["Connection"] = "keep-alive";
        _headers["User-Agent"] = Http::userAgent();
    }
    
    HttpOptions::~HttpOptions() {
        if(_body){
            delete [] _body;
        }
    }
    
    kk::CString HttpOptions::url(){
        return _url.c_str();
    }
    
    void HttpOptions::setUrl(kk::CString url) {
        _url = url;
    }
    
    kk::CString HttpOptions::method(){
        return _method.c_str();
    }
    
    void HttpOptions::setMethod(kk::CString method) {
        _method = method;
    }
    
    kk::CString HttpOptions::get(kk::CString key) {
        std::map<kk::String,kk::String>::iterator i = _headers.find(key);
        if(i != _headers.end()) {
            return i->second.c_str();
        }
        return nullptr;
    }
    
    void HttpOptions::set(kk::CString key,kk::CString value) {
        _headers[key] = value;
    }
    
    std::map<kk::String,kk::String> & HttpOptions::headers() {
        return _headers;
    }
    
    void * HttpOptions::body(size_t * size) {
        if(size) {
            * size = _size;
        }
        return _body;
    }
    
    void HttpOptions::setBody(void * body,size_t size) {
        
        if(body && size >0) {
            
            if(_body) {
                delete [] _body;
            }
            
            _body = new unsigned char[size];
            
            memcpy(_body, body, size);
            
            _size =  size;
            
        } else if(_body) {
            delete [] _body;
            _body = nullptr;
            _size = 0;
        }
    }
    
    HttpResponse * HttpOptions::response() {
        return _response.as<HttpResponse>();
    }
    
    void HttpOptions::setResponse(HttpResponse * response) {
        _response = response;
    }
    
    kk::CString HttpOptions::errmsg() {
        return _errmsg.c_str();
    }
    
    void HttpOptions::setErrmsg(kk::CString errmsg) {
        _errmsg = errmsg;
    }

}
