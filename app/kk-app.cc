//
//  kk-app.cc
//  KKGame
//
//  Created by zhanghailong on 2018/2/7.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include "kk-config.h"
#include "kk-app.h"

#if defined(KK_PLATFORM_IOS)

#include <KKObject/KKObject.h>

#else

#include "kk-crypto.h"
#include "kk-document-binary.h"

#endif

#include "require_js.h"
#include "GAScene.h"
#include "GABody.h"
#include "GAShape.h"
#include "GAAction.h"
#include "GAActionWalk.h"
#include "GADocument.h"
#include "GATileMap.h"

#ifdef KK_APP_GL

#include "GLContext.h"
#include "GLAnimation.h"
#include "GLShapeElement.h"
#include "GLTextElement.h"
#include "GLMinimapElement.h"
#include "GLViewportElement.h"
#include "GLTileMapElement.h"
#include "GLSpineElement.h"
#include "GLMetaElement.h"
#include "GLSliceMapElement.h"
#include "GLImageElement.h"

#endif

namespace kk {

    static duk_ret_t Application_print(duk_context * ctx) {
        
        Application * app = nullptr;
        
        duk_push_current_function(ctx);
        
        duk_get_prop_string(ctx, -1, "__object");
        
        if(duk_is_pointer(ctx, -1)) {
            app = (Application *)(duk_to_pointer(ctx, -1));
        }
        
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
    
    Application::Application(CString basePath,kk::Uint64 appid,kk::script::Context * jsContext):_appid(appid),_running(false),_exiting(false) {
        
        _jsContext = jsContext;
#ifdef KK_APP_GL
        _GAContext = new kk::GL::Context();
#else
        _GAContext = new kk::GA::Context();
#endif
        _document = new kk::Document();
 
        GAContext()->setBasePath(basePath);
        
        duk_context * ctx = dukContext();
        
        installContext(ctx);
        
        {
           
            kk::script::SetPrototype(ctx, &kk::DocumentBinaryObserver::ScriptClass);
            kk::script::SetPrototype(ctx, &kk::ElementEvent::ScriptClass);
            
            kk::Document::library("kk::GA::Scene", kk::GA::Scene::Create);
            kk::Document::library("kk::GA::Shape", kk::GA::Shape::Create);
            kk::Document::library("kk::GA::Body", kk::GA::Body::Create);
            kk::Document::library("kk::GA::Action", kk::GA::Action::Create);
            kk::Document::library("kk::GA::ActionWalk", kk::GA::ActionWalk::Create);
            kk::Document::library("kk::GA::Document", kk::GA::Document::Create);
            kk::Document::library("kk::GA::TileMap", kk::GA::TileMap::Create);

        }
        
#ifdef KK_APP_GL
        {
            kk::Document::library("kk::GL::SliceMapElement", kk::GL::SliceMapElement::Create);
            kk::Document::library("kk::GL::ImageElement", kk::GL::ImageElement::Create);
            kk::Document::library("kk::GL::ShapeElement", kk::GL::ShapeElement::Create);
            kk::Document::library("kk::GL::Animation", kk::GL::Animation::Create);
            kk::Document::library("kk::GL::AnimationItem", kk::GL::AnimationItem::Create);
            kk::Document::library("kk::GL::TextElement", kk::GL::TextElement::Create);
            kk::Document::library("kk::GL::MinimapElement", kk::GL::MinimapElement::Create);
            kk::Document::library("kk::GL::ViewportElement", kk::GL::ViewportElement::Create);
            kk::Document::library("kk::GL::TileMapElement", kk::GL::TileMapElement::Create);
            kk::Document::library("kk::GL::SpineElement", kk::GL::SpineElement::Create);
            kk::Document::library("kk::GL::MetaElement", kk::GL::MetaElement::Create);
        }
#endif
        
        {
            
            duk_push_global_object(ctx);
            
            duk_push_string(ctx, "context");
            kk::script::PushObject(ctx, _GAContext.get());
            duk_put_prop(ctx, -3);
            
            duk_push_string(ctx, "document");
            kk::script::PushObject(ctx, _document.get());
            duk_put_prop(ctx, -3);
            
            duk_push_string(ctx, "app");
            duk_push_object(ctx);
            duk_put_prop(ctx, -3);
            
            duk_pop(ctx);
        }
        
    }
    
    Application::~Application() {

    }
    
    kk::Boolean Application::isRunning() {
        return _running;
    }
    
    KKGAContext * Application::GAContext() {
        return _GAContext.as<KKGAContext>();
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
    
    kk::Document * Application::document() {
        return _document.as<kk::Document>();
    }
    
    void Application::exec() {
        KKGAContext * GAContext = this->GAContext();
        kk::Document * document = this->document();
        if(GAContext && document) {
            GAContext->tick();
            kk::Element * element = document->rootElement();
            if(element != nullptr) {
                GAContext->exec(element);
#ifdef KK_APP_GL
                GAContext->draw(element);
#endif
            }
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
 
        duk_push_string(ctx, "main.js");
        
        duk_compile_string_filename(ctx, 0, v.c_str());
        
        if(duk_is_function(ctx, -1)) {
            
            if(DUK_EXEC_SUCCESS != duk_pcall(ctx, 0)) {
                kk::script::Error(ctx, -1);
            }
            
            duk_pop(ctx);
            
        } else {
            duk_pop(ctx);
        }
        
        _running = true;
    }
    
    kk::Uint64 Application::appid() {
        return _appid;
    }
    
    kk::Boolean Application::isExiting() {
        return _exiting;
    }
    
    void Application::exit() {
        _exiting = true;
    }
    
}
