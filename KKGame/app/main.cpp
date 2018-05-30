//
//  main.cpp
//  app
//
//  Created by hailong11 on 2018/5/4.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include "kk-config.h"
#include "kk-app.h"
#include "kk-string.h"
#include <libwebsockets.h>

static void main_lws_log_emit(int level, const char * line) {
    
}

int main(int argc, const char * argv[]) {
    
    
    lws_set_log_level(~0, main_lws_log_emit);
    
    kk::Uint64 appid = 0;
    kk::CString path = nullptr;
    std::map<kk::String,kk::String> query;

    for(int i=1;i<argc;i++ ){
        if(kk::CStringEqual(argv[i], "-id") && i + 1 < argc) {
            appid = atoll(argv[i + 1]);
            i ++;
        } else if(kk::CStringHasPrefix(argv[i], "-") && i + 1 < argc) {
            query[argv[i] + 1] = argv[i + 1];
            i ++;
        } else if(path == nullptr){
            path = argv[i];
        }
    }
    
    if(path == nullptr) {
        path = ".";
    }
    
    kk::Application * app = new kk::Application(path,appid,nullptr);
    
    duk_context * ctx = app->dukContext();
    
    duk_push_global_object(ctx);
    
    duk_get_prop_string(ctx, -1, "app");
    
    duk_push_object(ctx);
    std::map<kk::String,kk::String>::iterator i = query.begin();
    while(i != query.end()) {
        duk_push_string(ctx, i->first.c_str());
        duk_push_string(ctx, i->second.c_str());
        duk_put_prop(ctx, -3);
        i ++;
    }
    duk_put_prop_string(ctx, -2, "query");
    
    duk_pop_2(ctx);
    
    app->run();
    
    delete app;
    
    return 0;
}
