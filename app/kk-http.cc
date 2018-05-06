//
//  kk-http.cc
//  KKGame
//
//  Created by 张海龙 on 2018/5/6.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include "kk-config.h"
#include "kk-http.h"


namespace kk {
    
    class HttpTask : public IHttpTask {
    public:
        
        HttpTask(struct lws * wsi,HttpOptions * options):_wsi(wsi),_options(options) {
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
            
        }
        
        virtual void onError(kk::CString errmsg) {
            
        }
        
        virtual void onClose() {
            _wsi = nullptr;
        }
        
        virtual void onData(void * data,size_t size) {
            
        }
        
        virtual Boolean onSend() {
            
            return false;
        }
        
        virtual Boolean onRead() {
            
            return true;
        }
        
        virtual void onDone() {
            
        }
        
    protected:
        kk::Strong _options;
        struct lws *_wsi;
    };
    
    int HttpCB(struct lws *wsi, enum lws_callback_reasons reason,
                           void *user, void *in, size_t len) {
        
        HttpTask * v = (HttpTask *) user;
        Http * http = (Http *) lws_context_user(lws_get_context(wsi));
        
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
                lwsl_user("RECEIVE_CLIENT_HTTP_READ: read %d\n", (int)len);
                lwsl_hexdump_notice(in, len);
                return 0; /* don't passthru */
                
            case LWS_CALLBACK_RECEIVE_CLIENT_HTTP:
                n = sizeof(buf) - LWS_PRE;
                if (lws_http_client_read(wsi, &p, &n) < 0)
                    return -1;
                
                return 0; /* don't passthru */
                
            case LWS_CALLBACK_COMPLETED_CLIENT_HTTP:
                lwsl_user("LWS_CALLBACK_COMPLETED_CLIENT_HTTP\n");
                bad |= status != 200;
                /*
                 * Do this to mark us as having processed the completion
                 * so close doesn't duplicate (with pipelining, completion !=
                 * connection close
                 */
                for (n = 0; n < count_clients; n++)
                    if (client_wsi[n] == wsi)
                        client_wsi[n] = NULL;
                if (++completed == count_clients)
                /* abort poll wait */
                    lws_cancel_service(lws_get_context(wsi));
                break;
                
                /* ...callbacks related to generating the POST... */
                
            case LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER:
                lwsl_user("LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER\n");
                up = (uint8_t **)in;
                uend = *up + len - 1;
                
                /* generate a random boundary string */
                
                lws_get_random(lws_get_context(wsi), &r, sizeof(r));
                lws_snprintf(pss->boundary, sizeof(pss->boundary) - 1,
                             "---boundary-%08x", r);
                
                n = lws_snprintf(buf, sizeof(buf) - 1,
                                 "multipart/form-data; boundary=%s", pss->boundary);
                if (lws_add_http_header_by_token(wsi,
                                                 WSI_TOKEN_HTTP_CONTENT_TYPE,
                                                 (uint8_t *)buf, n, up, uend))
                    return 1;
                /*
                 * Notice because we are sending multipart/form-data we can
                 * usually rely on the server to understand where the form
                 * payload ends without having to give it an overall
                 * content-length (which can be troublesome to compute ahead
                 * of generating the data to send).
                 *
                 * Tell lws we are going to send the body next...
                 */
                lws_client_http_body_pending(wsi, 1);
                lws_callback_on_writable(wsi);
                break;
                
            case LWS_CALLBACK_CLIENT_HTTP_WRITEABLE:
                lwsl_user("LWS_CALLBACK_CLIENT_HTTP_WRITEABLE\n");
                n = LWS_WRITE_HTTP;
                
                /*
                 * For a small body like this, we could prepare it in memory and
                 * send it all at once.  But to show how to handle, eg,
                 * arbitrary-sized file payloads, or huge form-data fields, the
                 * sending is done in multiple passes through the event loop.
                 */
                
                switch (pss->body_part++) {
                    case 0:
                        /* notice every usage of the boundary starts with -- */
                        p += lws_snprintf(p, end - p, "--%s\xd\xa"
                                          "content-disposition: "
                                          "form-data; name=\"text\"\xd\xa"
                                          "\xd\xa"
                                          "my text field"
                                          "\xd\xa", pss->boundary);
                        break;
                    case 1:
                        p += lws_snprintf(p, end - p,
                                          "--%s\xd\xa"
                                          "content-disposition: form-data; name=\"file\";"
                                          "filename=\"myfile.txt\"\xd\xa"
                                          "content-type: text/plain\xd\xa"
                                          "\xd\xa"
                                          "This is the contents of the "
                                          "uploaded file.\xd\xa"
                                          "\xd\xa", pss->boundary);
                        break;
                    case 2:
                        p += lws_snprintf(p, end - p, "--%s--\xd\xa",
                                          pss->boundary);
                        lws_client_http_body_pending(wsi, 0);
                        /* necessary to support H2, it means we will write no
                         * more on this stream */
                        n = LWS_WRITE_HTTP_FINAL;
                        break;
                        
                    default:
                        /*
                         * We can get extra callbacks here, if nothing to do,
                         * then do nothing.
                         */
                        return 0;
                }
                
                if (lws_write(wsi, (uint8_t *)start, lws_ptr_diff(p, start), n)
                    != lws_ptr_diff(p, start))
                    return 1;
                
                if (n != LWS_WRITE_HTTP_FINAL)
                    lws_callback_on_writable(wsi);
                
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
    
    Http::Http(uv_loop_t * loop) {
        
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
        
    }
    
    void Http::openlibs(kk::script::Context * jsContext) {
        
        jsContext->setObject("http", this);
        
        duk_context * ctx = jsContext->jsContext();
        duk_push_global_object(ctx);
        kk::script::PushObject(ctx, this);
        duk_put_prop_string(ctx, -2, "http");
        duk_pop(ctx);
    }
    
    Http * Http::GetContext(kk::script::Context * ctx) {
        return (WebSocketContext *) ctx->object("http");
    }
    
}
