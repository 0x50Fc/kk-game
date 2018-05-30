//
//  kk-app.cc
//  KKGame
//
//  Created by zhanghailong on 2018/2/7.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include "kk-config.h"
#include "kk-app.h"
#include "kk-string.h"
#include "kk-websocket.h"
#include "kk-http.h"
#include "kk-crypto.h"
#include "require_js.h"
#include "GAScene.h"
#include "GABody.h"
#include "GAShape.h"
#include "GAAction.h"
#include "GAActionWalk.h"
#include "GADocument.h"
#include "GATileMap.h"

namespace kk {

    static duk_ret_t Application_print(duk_context * ctx) {
        
        duk_push_current_function(ctx);
        
        duk_get_prop_string(ctx, -1, "__object");
        
        Application * app = (Application *)(duk_to_pointer(ctx, -1));
        
        duk_pop_2(ctx);
        
        kk::Uint64 appid = app != nullptr ? app->appid() : 0;
        
        int top = duk_get_top(ctx);
        
        for(int i=0;i<top;i++) {
            
            if(duk_is_string(ctx, - top + i)) {
                kk::Log("[%lld] %s",appid,duk_to_string(ctx, - top + i));
            } else if(duk_is_number(ctx, - top + i)) {
                kk::Log("[%lld] %g",appid,duk_to_number(ctx, - top + i));
            } else if(duk_is_boolean(ctx, - top + i)) {
                kk::Log("[%lld] %s",appid,duk_to_boolean(ctx, - top + i) ? "true":"false");
            } else if(duk_is_buffer_data(ctx, - top + i)) {
                kk::Log("[%lld] [bytes]:",appid);
                {
                    size_t n;
                    unsigned char * bytes = (unsigned char *) duk_get_buffer_data(ctx, - top + i, &n);
                    while(n >0) {
                        printf("%u",*bytes);
                        bytes ++;
                        n --;
                        if(n != 0) {
                            printf(",");
                        }
                    }
                    printf("\n");
                }
            } else if(duk_is_function(ctx, - top + i)) {
                kk::Log("[%lld] [function]",appid);
            } else if(duk_is_undefined(ctx, - top + i)) {
                kk::Log("[%lld] [undefined]",appid);
            } else if(duk_is_null(ctx, - top + i)) {
                kk::Log("[%lld] [null]",appid);
            } else {
                kk::Log("[%lld] %s",appid,duk_json_encode(ctx, - top + i));
                duk_pop(ctx);
            }
            
        }
        
        return 0;
    }
    
    static duk_ret_t Application_getString(duk_context * ctx) {
        
        duk_push_current_function(ctx);
        
        duk_get_prop_string(ctx, -1, "__object");
        
        Application * app = (Application *)(duk_to_pointer(ctx, -1));
        
        duk_pop_2(ctx);
        
        if(app) {
            
            int top = duk_get_top(ctx);
            kk::CString path = nullptr;
            if(top >0 && duk_is_string(ctx, -top)) {
                path = duk_to_string(ctx, -top);
            }
            
            if(path) {
                kk::String v = app->GAContext()->getString(path);
                duk_push_string(ctx, v.c_str());
                return 1;
            }
            
        }
        
        return 0;
    }
    
    static duk_ret_t Application_compile(duk_context * ctx) {
        
        duk_push_current_function(ctx);
        
        duk_get_prop_string(ctx, -1, "__object");
        
        Application * app = (Application *)(duk_to_pointer(ctx, -1));
        
        duk_pop_2(ctx);
        
        if(app) {
            
            kk::CString path = nullptr;
            kk::CString prefix = nullptr;
            kk::CString suffix = nullptr;
            
            int top = duk_get_top(ctx);
            
            if(top > 0  && duk_is_string(ctx, - top)) {
                path = duk_to_string(ctx, -top);
            }
            
            if(top > 1  && duk_is_string(ctx, - top + 1)) {
                prefix = duk_to_string(ctx, -top + 1);
            }
            
            if(top > 2  && duk_is_string(ctx, - top + 2)) {
                suffix = duk_to_string(ctx, -top + 2);
            }
            
            kk::String code;
            
            if(prefix) {
                code.append(prefix);
            }
            
            kk::String v = app->GAContext()->getString(path);
            
            code.append(v);
            
            if(suffix) {
                code.append(suffix);
            }
            
            duk_push_string(ctx, path);
            duk_compile_string_filename(ctx, 0, code.c_str());
            
            return 1;
        }
        
        return 0;
    }
    
    struct Timer : uv_timer_t{
        Application * app;
        void * heapptr;
    };
    
    static duk_ret_t Timer_duk_dealloc(duk_context * ctx) {

        int top = duk_get_top(ctx);
        
        if(top > 0 && duk_is_object(ctx, -top)) {
            
            duk_get_prop_string(ctx, -top, "timer");
            
            if(duk_is_pointer(ctx, -1)) {
                Timer * v = (Timer *) duk_to_pointer(ctx, -1);
                delete v;
            }
            
            duk_pop(ctx);
        }
        
        return 0;
    }
    
    static void Application_setTimeout_cb(uv_timer_t* handle) {
        Timer * v = (Timer *) handle;
        duk_context * ctx = v->app->dukContext();
        
        duk_push_global_object(ctx);
        
        duk_push_sprintf(ctx, "0x%x", (unsigned long) v->heapptr);
        
        duk_get_prop(ctx, -2);
        
        if(duk_is_object(ctx, -1)) {
            
            duk_get_prop_string(ctx, -1, "fn");
            
            if(duk_is_function(ctx, -1)) {
                
                if(DUK_EXEC_SUCCESS != duk_pcall(ctx, 0)) {
                    kk::script::Error(ctx, -1);
                }
                
            }
            
            duk_pop(ctx);
            
        }
        
        duk_pop(ctx);
        
        duk_push_sprintf(ctx, "0x%x", (unsigned long) v->heapptr);
        
        duk_del_prop(ctx, -2);
        
        duk_pop(ctx);
        
    }
    
    static duk_ret_t Application_setTimeout(duk_context * ctx) {
        
        duk_push_current_function(ctx);
        
        duk_get_prop_string(ctx, -1, "__object");
        
        Application * app = (Application *)(duk_to_pointer(ctx, -1));
        
        duk_pop_2(ctx);
        
        if(app) {
            
            int top = duk_get_top(ctx);
            
            if(top >1 && duk_is_function(ctx, -top) && duk_is_number(ctx, -top + 1)) {
                
                void * fn = duk_get_heapptr(ctx, -top);
                uint64_t tv = duk_to_uint(ctx, -top + 1);
                
                duk_push_global_object(ctx);
                
                duk_push_object(ctx);
                
                duk_push_c_function(ctx, Timer_duk_dealloc, 1);
                duk_set_finalizer(ctx, -2);
                
                void * heapptr = duk_get_heapptr(ctx, -1);

                duk_push_sprintf(ctx, "0x%x", (unsigned long) heapptr);
                
                duk_dup(ctx, -2);
                duk_remove(ctx, -3);
                
                duk_push_string(ctx, "fn");
                duk_push_heapptr(ctx, fn);
                duk_put_prop(ctx, -3);
                
                duk_push_string(ctx, "timer");
                Timer * v = new Timer();
                duk_push_pointer(ctx, v);
                duk_put_prop(ctx, -3);
                
                duk_put_prop(ctx, -3);
                
                duk_pop(ctx);
                
                v->app = app;
                v->heapptr = heapptr;
                
                uv_timer_init(uv_get_loop(ctx), v);
                
                uv_timer_start(v, Application_setTimeout_cb, tv, 0);
                
                duk_push_sprintf(ctx, "0x%x", (unsigned long) heapptr);
                
                return 1;
                
            }
            
        }
        
        return 0;
    }
    
    static duk_ret_t Application_clearTimeout(duk_context * ctx) {
        
        int top = duk_get_top(ctx);
        
        if(top >0 && duk_is_string(ctx, -top) ) {
            
            duk_push_global_object(ctx);
            
            duk_dup(ctx, -top - 1);
            
            duk_get_prop(ctx, -2);
            
            if(duk_is_object(ctx, -1)) {
                
                duk_get_prop_string(ctx, -1, "timer");
                
                if(duk_is_pointer(ctx, -1)) {
                    
                    Timer * v = (Timer *) duk_to_pointer(ctx, -1);
                    uv_timer_stop((uv_timer_t *) v);
                }
                
                duk_pop(ctx);
                
                duk_get_prop_string(ctx, -1, "timer");
                duk_del_prop(ctx, -2);
                
            }
            
            duk_pop(ctx);
            
            duk_dup(ctx, -top - 1);
            
            duk_del_prop(ctx, -2);
            
            duk_pop(ctx);
            
        }
        
        return 0;
    }
    
    static void Application_setInterval_cb(uv_timer_t* handle) {
        
        Timer * v = (Timer *) handle;
        
        duk_context * ctx = v->app->dukContext();
        
        duk_push_global_object(ctx);
        
        duk_push_sprintf(ctx, "0x%x", (unsigned long) v->heapptr);
        
        duk_get_prop(ctx, -2);
        
        if(duk_is_object(ctx, -1)) {
            
            duk_get_prop_string(ctx, -1, "fn");
            
            if(duk_is_function(ctx, -1)) {
                
                if(DUK_EXEC_SUCCESS != duk_pcall(ctx, 0)) {
                    kk::script::Error(ctx, -1);
                }
                
            }
            
            duk_pop(ctx);
        }
        
        duk_pop_2(ctx);
        
    }
    
    static duk_ret_t Application_setInterval(duk_context * ctx) {
        
        duk_push_current_function(ctx);
        
        duk_get_prop_string(ctx, -1, "__object");
        
        Application * app = (Application *)(duk_to_pointer(ctx, -1));
        
        duk_pop_2(ctx);
        
        if(app) {
            
            int top = duk_get_top(ctx);
            
            if(top >1 && duk_is_function(ctx, -top) && duk_is_number(ctx, -top + 1)) {
                
                void * fn = duk_get_heapptr(ctx, -top);
                uint64_t tv = duk_to_uint(ctx, -top + 1);
                
                duk_push_global_object(ctx);
                
                duk_push_object(ctx);
                
                duk_push_c_function(ctx, Timer_duk_dealloc, 1);
                duk_set_finalizer(ctx, -2);
                
                void * heapptr = duk_get_heapptr(ctx, -1);
                
                duk_push_sprintf(ctx, "0x%x", (unsigned long) heapptr);
                
                duk_dup(ctx, -2);
                duk_remove(ctx, -3);
                
                duk_push_string(ctx, "fn");
                duk_push_heapptr(ctx, fn);
                duk_put_prop(ctx, -3);
                
                duk_push_string(ctx, "timer");
                Timer * v = new Timer();
                duk_push_pointer(ctx, v);
                duk_put_prop(ctx, -3);
                
                duk_put_prop(ctx, -3);
                
                duk_pop(ctx);
                
                v->app = app;
                v->heapptr = heapptr;
                
                uv_timer_init(uv_get_loop(ctx), v);
                
                uv_timer_start(v, Application_setInterval_cb, tv, tv);
                
                duk_push_sprintf(ctx, "0x%x", (unsigned long) heapptr);
                
                return 1;
                
            }
            
        }
        
    
        return 0;
    }
    
    static duk_ret_t Application_clearInterval(duk_context * ctx) {
        return Application_clearTimeout(ctx);
    }
    
    static duk_ret_t Application_exit(duk_context * ctx) {
        
        duk_push_current_function(ctx);
        
        duk_get_prop_string(ctx, -1, "__object");
        
        Application * app = (Application *)(duk_to_pointer(ctx, -1));
        
        duk_pop_2(ctx);
        
        if(app) {
            app->exit();
        }
        
        return 0;
    }
    
    kk::Float Kernel = 1.0;
    
    Application::Application(CString basePath,kk::Uint64 appid,ApplicationExitCB cb)
        :_running(false),_appid(appid),_cb(cb) {
        
        _jsContext = new kk::script::Context();
        _GAContext = new kk::GA::Context();
        _GAElement = new kk::GA::Element();
        
        GAContext()->setBasePath(basePath);
        
        duk_context * ctx = dukContext();
        
        {
            
            duk_push_global_object(ctx);
            
            duk_push_string(ctx, "print");
            duk_push_c_function(ctx, Application_print, DUK_VARARGS);
                duk_push_string(ctx, "__object");
                duk_push_pointer(ctx, this);
                duk_put_prop(ctx, -3);
            duk_put_prop(ctx, -3);
            
            duk_push_string(ctx, "kk");
            duk_push_object(ctx);
            
            duk_push_string(ctx, "platform");
            duk_push_string(ctx, "kk");
            duk_put_prop(ctx, -3);
            
            duk_push_string(ctx, "kernel");
            duk_push_number(ctx, Kernel);
            duk_put_prop(ctx, -3);
            
            duk_push_string(ctx, "getString");
            duk_push_c_function(ctx, Application_getString, 1);
                duk_push_string(ctx, "__object");
                duk_push_pointer(ctx, this);
                duk_put_prop(ctx, -3);
            duk_put_prop(ctx, -3);
            
            duk_push_string(ctx, "compile");
            duk_push_c_function(ctx, Application_compile, 3);
                duk_push_string(ctx, "__object");
                duk_push_pointer(ctx, this);
                duk_put_prop(ctx, -3);
            duk_put_prop(ctx, -3);
            
            duk_put_prop(ctx, -3);
            
            
            duk_push_string(ctx, "setTimeout");
            duk_push_c_function(ctx, Application_setTimeout, 2);
                duk_push_string(ctx, "__object");
                duk_push_pointer(ctx, this);
                duk_put_prop(ctx, -3);
            duk_put_prop(ctx, -3);
            
            duk_push_string(ctx, "clearTimeout");
            duk_push_c_function(ctx, Application_clearTimeout, 1);
                duk_push_string(ctx, "__object");
                duk_push_pointer(ctx, this);
                duk_put_prop(ctx, -3);
            duk_put_prop(ctx, -3);
            
            duk_push_string(ctx, "setInterval");
            duk_push_c_function(ctx, Application_setInterval, 2);
                duk_push_string(ctx, "__object");
                duk_push_pointer(ctx, this);
                duk_put_prop(ctx, -3);
            duk_put_prop(ctx, -3);
            
            duk_push_string(ctx, "clearInterval");
            duk_push_c_function(ctx, Application_clearInterval, 1);
                duk_push_string(ctx, "__object");
                duk_push_pointer(ctx, this);
                duk_put_prop(ctx, -3);
            duk_put_prop(ctx, -3);
            
            duk_push_string(ctx, "exit");
            duk_push_c_function(ctx, Application_exit, 1);
            {
                duk_push_string(ctx, "__object");
                duk_push_pointer(ctx, this);
                duk_put_prop(ctx, -3);
            }
            duk_put_prop(ctx, -3);
            
            duk_pop(ctx);
        }
        
        {
            duk_eval_lstring_noresult(ctx, (char *) require_js, sizeof(require_js));
            kk::Crypto_openlibs(ctx);
        }
        
        {
           
            kk::script::SetPrototype(ctx, &kk::GA::Context::ScriptClass);
            kk::script::SetPrototype(ctx, &kk::GA::Scene::ScriptClass);
            kk::script::SetPrototype(ctx, &kk::GA::Shape::ScriptClass);
            kk::script::SetPrototype(ctx, &kk::GA::Body::ScriptClass);
            kk::script::SetPrototype(ctx, &kk::GA::Action::ScriptClass);
            kk::script::SetPrototype(ctx, &kk::GA::ActionWalk::ScriptClass);
            kk::script::SetPrototype(ctx, &kk::GA::Document::ScriptClass);
            kk::script::SetPrototype(ctx, &kk::GA::TileMap::ScriptClass);
            kk::script::SetPrototype(ctx, &kk::ElementEvent::ScriptClass);
            
        }
        
        {
            
            duk_push_global_object(ctx);
            
            duk_push_string(ctx, "context");
            kk::script::PushObject(ctx, _GAContext.get());
            duk_put_prop(ctx, -3);
            
            duk_push_string(ctx, "app");
            duk_push_object(ctx);
            duk_put_prop(ctx, -3);
            
            duk_pop(ctx);
        }
        
    }
    
    Application::~Application() {

    }
    
    void Application::exit() {
        if(_running) {
            _running = false;
            uv_timer_stop(&_timer);
            if(_cb) {
                (*_cb)(this);
            }
        }
    }
    
    kk::GA::Context * Application::GAContext() {
        return _GAContext.as<kk::GA::Context>();
    }
    
    kk::script::Context * Application::jsContext() {
        return _jsContext.as<kk::script::Context>();
    }
    
    duk_context * Application::dukContext() {
        return jsContext()->jsContext();
    }
    
    kk::GA::Element * Application::GAElement() {
        return _GAElement.as<kk::GA::Element>();
    }
    
    static void Application_uv_timer_cb(uv_timer_t* handle) {
        Application * app = (Application *) handle->data;
        kk::GA::Context * GAContext = app->GAContext();
        kk::GA::Element * GAElement = app->GAElement();
        if(GAContext && GAElement) {
            GAContext->tick();
            GAContext->exec(GAElement);
        }
    }

    void Application::runCommand(kk::CString command) {
        
        duk_context * ctx = dukContext();
        
        duk_push_global_object(ctx);
        
        duk_get_prop_string(ctx, -1, "app");
        
        if(duk_is_object(ctx, -1)) {
            
            duk_get_prop_string(ctx, -1, "runCommand");
            
            if(duk_is_function(ctx, -1)) {
                
                duk_push_string(ctx, command);
                
                if(duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
                    kk::script::Error(ctx, -1);
                }
            }
        }
        
        duk_pop_n(ctx, 3);
        
    }
    
    void Application::run(uv_loop_t * loop) {
        
        if(_running) {
            return ;
        }
        
        _running = true;
        
        duk_context * ctx = dukContext();
        
        uv_openlibs(ctx, loop);
        
        kk::Strong wsContext = new kk::WebSocketContext(loop);

        wsContext.as<kk::WebSocketContext>()->openlibs(jsContext());
        
        kk::script::SetPrototype(ctx, &kk::WebSocket::ScriptClass);

        kk::Strong http = new kk::Http(loop,nullptr);
        
        http.as<kk::Http>()->openlibs(jsContext());
        
        kk::GA::Context * GAContext = this->GAContext();
        
        kk::String v = GAContext->getString("main.js");
    
        kk::String evalCode = "(function(element) {" + v + "} )";
        
        duk_push_string(ctx, "main.js");
        
        duk_compile_string_filename(ctx, 0, evalCode.c_str());
        
        if(duk_is_function(ctx, -1)) {
            
            if(DUK_EXEC_SUCCESS != duk_pcall(ctx, 0)) {
                kk::script::Error(ctx, -1);
            } else if(duk_is_function(ctx, -1)) {

                kk::script::PushObject(ctx, GAElement());
                
                if(DUK_EXEC_SUCCESS != duk_pcall(ctx, 1)) {
                    kk::script::Error(ctx, -1);
                }
            }
            
            duk_pop(ctx);
            
        } else {
            duk_pop(ctx);
        }
        
        uv_timer_init(loop, &_timer);
        
        _timer.data = this;
        
        uint64_t tv = 1000 / GAContext->frames();
        
        uv_timer_start(&_timer, Application_uv_timer_cb, tv, tv);
        
    }
    
    static void Application_exit_cb(uv_signal_t* handle, int signum) {
        Application * app = (Application *) handle->data;
        uv_loop_t * loop = uv_handle_get_loop((uv_handle_t *) handle);
        app->exit();
        uv_stop(loop);
    }
    
    static char data[65536];
    
    static void Application_alloc_cb(uv_handle_t* handle,size_t suggested_size,uv_buf_t* buf) {
        buf->base = data;
        buf->len = sizeof(data);
    }
    
    static void Application_read_cb(uv_stream_t* stream,
                                    ssize_t nread,
                                    const uv_buf_t* buf) {
        
        Application * app = (Application *) stream->data;
        uv_loop_t * loop = uv_handle_get_loop((uv_handle_t *) stream);
        
        if(nread > 1) {
            
            buf->base[nread - 1] = 0;
            
            app->runCommand(buf->base);
            
            if(kk::CStringEqual(buf->base, "exit")) {
                app->exit();
                uv_stop(loop);
            }
        }
        
    }
  
    void Application::run()  {
        
        uv_loop_t loop;
        
        uv_loop_init(&loop);
        
        uv_signal_t sigint;
        uv_signal_init(&loop, &sigint);
        
        sigint.data = this;
        
        uv_signal_start(&sigint, Application_exit_cb, SIGINT);
        
        {
            uv_tty_t tty;
            uv_tty_init(&loop, &tty, STDIN_FILENO, 1);
            uv_tty_set_mode(&tty, UV_TTY_MODE_NORMAL);
            
            tty.data = this;
        
            uv_read_start((uv_stream_t *)&tty, Application_alloc_cb, Application_read_cb);
            
        }
        
        run(&loop);
        
        uv_run(&loop, UV_RUN_DEFAULT);
        
    }
    
    kk::Boolean Application::isRunning() {
        return this->_running;
    }
    
    kk::Uint64 Application::appid() {
        return _appid;
    }
    
}
