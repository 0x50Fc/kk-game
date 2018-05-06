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
#include "require_js.h"
#include "GAScene.h"
#include "GABody.h"
#include "GAShape.h"
#include "GAAction.h"
#include "GAActionWalk.h"
#include "GADocument.h"
#include "GATileMap.h"

namespace kk {

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
    
    struct Timer : uv_timer_t{
        Application * app;
        void * heapptr;
    };
    
    static void Timer_uv_close_cb(uv_handle_t* handle) {
        Timer * v = (Timer *) handle;
        delete v;
    }
    
    static duk_ret_t Timer_duk_dealloc(duk_context * ctx) {

        int top = duk_get_top(ctx);
        
        if(top > 0 && duk_is_object(ctx, -top)) {
            
            duk_get_prop_string(ctx, -top, "timer");
            
            if(duk_is_pointer(ctx, -1)) {
                Timer * v = (Timer *) duk_to_pointer(ctx, -1);
                if(!uv_is_closing((uv_handle_t *)v)) {
                    uv_close((uv_handle_t *) v, Timer_uv_close_cb);
                }
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
                    if(!uv_is_closing((uv_handle_t *)v)) {
                        uv_close((uv_handle_t *) v, Timer_uv_close_cb);
                    }
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
    
    kk::Float Kernel = 1.0;
    
    Application::Application(CString basePath) {
        
        _jsContext = new kk::script::Context();
        _GAContext = new kk::GA::Context();
        _GAElement = new kk::GA::Element();
        
        GAContext()->setBasePath(basePath);
        
        duk_context * ctx = dukContext();
        
        {
            
            duk_push_global_object(ctx);
            
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
            
            duk_pop(ctx);
        }
        
        {
            duk_eval_lstring_noresult(ctx, (char *) require_js, sizeof(require_js));
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

    void Application::run(uv_loop_t * loop) {
        
        duk_context * ctx = dukContext();
        
        uv_openlibs(ctx, loop);
        
        kk::Strong wsContext = new kk::WebSocketContext(loop);

        wsContext.as<kk::WebSocketContext>()->openlibs(jsContext());
        
        kk::script::SetPrototype(ctx, &kk::WebSocket::ScriptClass);

        kk::GA::Context * GAContext = this->GAContext();
        
        kk::String v = GAContext->getString("main.js");
        
        kk::String evalCode = "(function(element) {" + v + "} )";
        
        duk_eval_string(ctx, evalCode.c_str());
        
        if(duk_is_function(ctx, -1)) {
            
            kk::script::PushObject(ctx, GAElement());
            
            if(DUK_EXEC_SUCCESS != duk_pcall(ctx, 1)) {
                kk::script::Error(ctx, -1);
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
        uv_loop_t * loop = uv_handle_get_loop((uv_handle_t *) handle);
        uv_stop(loop);
    }
    
    void Application::run()  {
        
        uv_loop_t loop;
        
        uv_loop_init(&loop);
        
        uv_signal_t sigint;
        uv_signal_init(&loop, &sigint);
        
        uv_signal_start(&sigint, Application_exit_cb, SIGINT);
        
        run(&loop);
        
        uv_run(&loop, UV_RUN_DEFAULT);
        
    }
    
}
