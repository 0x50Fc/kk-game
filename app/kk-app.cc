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

    kk::Float Kernel = 1.0;
    
    Application::Application(CString basePath,kk::Uint64 appid,kk::script::Context * jsContext)
        :_appid(appid) {
        
        _jsContext = jsContext;
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
    
    kk::GA::Context * Application::GAContext() {
        return _GAContext.as<kk::GA::Context>();
    }
    
    kk::script::Context * Application::jsContext() {
        return _jsContext.as<kk::script::Context>();
    }
    
    void Application::installContext(duk_context * ctx) {
        
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
            
            
            duk_pop(ctx);
        }
        
        {
            duk_eval_lstring_noresult(ctx, (char *) require_js, sizeof(require_js));
            kk::Crypto_openlibs(ctx);
        }
        
    }
    
    
    duk_context * Application::dukContext() {
        return jsContext()->jsContext();
    }
    
    kk::GA::Element * Application::GAElement() {
        return _GAElement.as<kk::GA::Element>();
    }
    
    void Application::exec() {
        kk::GA::Context * GAContext = this->GAContext();
        kk::GA::Element * GAElement = this->GAElement();
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
    
    void Application::run() {
        
        duk_context * ctx = dukContext();
        
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
        
    }
    
    kk::Uint64 Application::appid() {
        return _appid;
    }
    
}
