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
#include "GAContext.h"
#include "GABody.h"
#include "GAScene.h"
#include "GAShape.h"
#include "require_js.h"
#include "GAActionWalk.h"
#include "GADocument.h"

#include <sstream>
#include <fstream>

class KKApplicationImpl : public KKApplication {
public:
    kk::Strong GAContext;
    kk::Strong JSContext;
    kk::Strong GAElement;
    std::list<std::map<std::string,std::string>> actions;
};

static duk_ret_t KKApplication_getString(duk_context * ctx) {
    
    int top = duk_get_top(ctx);
    
    const char * path = nullptr;
    
    if(top > 0 && duk_is_string(ctx, -top )) {
        path = duk_to_string(ctx, -top);
    }
    
    if(path) {
        
        KKApplicationImpl * app = nullptr;
        
        duk_push_current_function(ctx);
        
        duk_push_string(ctx, "__object");
        duk_get_prop(ctx, -2);
        
        if(duk_is_pointer(ctx, -1)) {
            app = (KKApplicationImpl *) duk_to_pointer(ctx, -1);
        }
        
        duk_pop_n(ctx, 2);
        
        if(app) {
            
            std::ostringstream s;
            
            s << app->GAContext.as<kk::GA::Context>()->basePath();
            
            if(! kk::CStringHasSuffix(app->GAContext.as<kk::GA::Context>()->basePath(), "/")) {
                s << "/";
            }
            
            s << path << std::ends;
            
            FILE * fd = fopen(s.str().c_str(),"r");

            if(fd != nullptr) {
                
                std::string data;
                char buf[204800];
                size_t n;
                
                while((n = fread(buf, 1, sizeof(buf), fd)) > 0){
                    data.append(buf,n);
                }
                
                fclose(fd);
                
                duk_push_string(ctx, data.c_str());
                
                return 1;
                
            } else {
                kk::Log("Not Open File: %s", s.str().c_str());
            }
        }
    }
    
    return 0;
}

struct KKApplication * KKApplicationCreate() {
    
    KKApplicationImpl * app = new KKApplicationImpl();

    app->GAContext = new kk::GA::Context();
    app->JSContext = new kk::script::Context();
    app->GAElement = new kk::GA::Element();
    app->jsContext = app->JSContext.as<kk::script::Context>()->jsContext();
    app->object = nullptr;
    
    {
        duk_context * ctx = app->JSContext.as<kk::script::Context>()->jsContext();
        
        duk_push_global_object(ctx);
        
        duk_push_string(ctx, "kk");
        duk_push_object(ctx);
        
        duk_push_string(ctx, "platform");
        duk_push_string(ctx, "server");
        duk_put_prop(ctx, -3);
        
        duk_push_string(ctx, "kernel");
        duk_push_number(ctx, KKApplicationKernel);
        duk_put_prop(ctx, -3);
        
        duk_push_string(ctx, "getString");
        duk_push_c_function(ctx, KKApplication_getString, 1);
        
        duk_push_string(ctx, "__object");
        duk_push_pointer(ctx, app);
        duk_put_prop(ctx, -3);
        
        duk_put_prop(ctx, -3);
        
        
        duk_put_prop(ctx, -3);
        
        
        duk_push_string(ctx, "element");
        kk::script::PushObject(ctx, app->GAElement.get());
        duk_put_prop(ctx, -3);
        
        duk_push_string(ctx, "app");
        duk_push_object(ctx);
        duk_put_prop(ctx, -3);
        
        duk_push_string(ctx, "__object");
        duk_push_pointer(ctx, app);
        duk_put_prop(ctx, -3);
        
        duk_pop(ctx);
    }

    {
        duk_context * ctx = app->JSContext.as<kk::script::Context>()->jsContext();
        duk_eval_lstring_noresult(ctx, (char *) require_js, sizeof(require_js));
    }
    
    {
        duk_context * ctx = app->JSContext.as<kk::script::Context>()->jsContext();
        
        kk::script::SetPrototype(ctx, &kk::GA::Context::ScriptClass);
        kk::script::SetPrototype(ctx, &kk::GA::Scene::ScriptClass);
        kk::script::SetPrototype(ctx, &kk::GA::Body::ScriptClass);
        kk::script::SetPrototype(ctx, &kk::GA::Shape::ScriptClass);
        kk::script::SetPrototype(ctx, &kk::GA::Action::ScriptClass);
        kk::script::SetPrototype(ctx, &kk::GA::ActionWalk::ScriptClass);
        kk::script::SetPrototype(ctx, &kk::GA::Document::ScriptClass);
        kk::script::SetPrototype(ctx, &kk::ElementEvent::ScriptClass);
        
    }
    
    {
        duk_context * ctx = app->JSContext.as<kk::script::Context>()->jsContext();
        duk_push_global_object(ctx);
        
        duk_push_string(ctx, "context");
        kk::script::PushObject(ctx, app->GAContext.get());
        duk_put_prop(ctx, -3);
        
        duk_pop(ctx);
    }
    
	return (struct KKApplication *) app;
}

void KKApplicationRun(struct KKApplication * app,const char * basePath) {

    KKApplicationImpl * v = (KKApplicationImpl *) app;
    
    kk::GA::Context * GAContext = v->GAContext.as<kk::GA::Context>();
    
    GAContext->setFrames(30);
    GAContext->setBasePath(basePath);
    
    std::stringstream s;
    
    s << basePath;
    
    if(! kk::CStringHasSuffix(basePath, "/")) {
        s << "/";
    }
    
    s << "main.js" << std::ends;
    
    duk_context * ctx = v->JSContext.as<kk::script::Context>()->jsContext();
    
    FILE * fd = fopen(s.str().c_str(), "r");

    if(fd != nullptr) {
        
        std::string data;
        
        char buf[204800];
        ssize_t n;
        
        while((n = fread(buf, 1, sizeof(buf), fd)) > 0){
            data.append(buf,n);
        }
        
        fclose(fd);
        
        duk_push_string(ctx, s.str().c_str());
        duk_compile_string_filename(ctx, 0, data.c_str());
        
        if(duk_is_function(ctx, -1)) {
            
            if(DUK_EXEC_SUCCESS != duk_pcall(ctx, 0)) {
                kk::script::Error(ctx, -1);
            }
            
            duk_pop(ctx);
            
        } else {

            kk::Log("Script Fail: %s",duk_to_string(ctx, -1));
            duk_pop(ctx);
        }
        
    } else {
        kk::Log("Not Open File: %s", s.str().c_str());
    }
    
}

void KKApplicationExit(struct KKApplication * app) {
    KKApplicationImpl * v = (KKApplicationImpl *) app;
    delete v;
}

void KKApplicationExec(struct KKApplication * app) {
    struct KKApplicationImpl * v = (struct KKApplicationImpl *) app;
    kk::GA::Context * GAContext = v->GAContext.as<kk::GA::Context>();
    GAContext->tick();
    GAContext->exec(v->GAElement.as<kk::GA::Element>());
}

struct KKApplication * KKApplicationGet(duk_context * jsContext) {
    
    KKApplication * app = nullptr;
    
    duk_push_global_object(jsContext);
    
    duk_push_string(jsContext, "__object");
    duk_get_prop(jsContext, -2);
    
    if(duk_is_pointer(jsContext, -1)) {
        app = (KKApplication *) duk_to_pointer(jsContext, -1);
    }
    
    duk_pop_2(jsContext);
    
    return app;
}

void duk_push_string_ptr(duk_context * ctx, void * ptr) {
    duk_push_sprintf(ctx, "0x%x",(long)ptr);
}

void KKApplicationError(struct KKApplication * app, duk_idx_t idx) {
    KKApplicationImpl * v = (KKApplicationImpl *) app;
    kk::GA::Context * GAContext = v->GAContext.as<kk::GA::Context>();
    kk::script::Error(app->jsContext, idx,GAContext->basePath());
}
