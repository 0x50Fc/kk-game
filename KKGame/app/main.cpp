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
#include "kk-ev.h"
#include "kk-ws.h"
#include "kk-http.h"
#include <event.h>
#include <unistd.h>

static void main_stdin_bufferevent_data_cb(struct bufferevent *bev, void *ctx) {
    
    kk::Application * app = (kk::Application *) ctx;
    
    evbuffer * input = bufferevent_get_input(bev);
    
    size_t nread = EVBUFFER_LENGTH(input);
    char * p = (char *) EVBUFFER_DATA(input);
    char * end = p + nread;
    char * v = p;
    
    while(v < end) {
        
        if(* v == '\n') {
            
            *v = 0;
            
            app->runCommand(p);
            
            if(kk::CStringEqual(p, "exit")) {
                
                v ++ ;
                p = v;
                
                event_base_loopbreak(bufferevent_get_base(bev));
                break;
            }
            
            v ++ ;
            p = v;
            continue;
        }
        
        v ++;
    }
    
    evbuffer_drain(input, ( p - (char *) EVBUFFER_DATA(input)));
    
    bufferevent_enable(bev, EV_READ);
}

static event * app_ev_exec;

static void app_ev_exec_cb(evutil_socket_t fd, short ev, void * ctx) {
    
    kk::Application * app = (kk::Application *) ctx;
    
    app->exec();
    
    kk::Uint frames = app->GAContext()->frames();
    timeval tv = {0, (int) (1000000 / frames)};
    evtimer_add(app_ev_exec, &tv);
}


int main(int argc, const char * argv[]) {
    
    
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
    
    kk::script::Context * jsContext = new kk::script::Context();
    
    jsContext->retain();
    
    kk::Application * app = new kk::Application(path,appid,jsContext);
    
    app->retain();
    
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
    
    event_base * base = event_init();
    evdns_base * dns = evdns_base_new(base, EVDNS_BASE_INITIALIZE_NAMESERVERS);
    
    evdns_base_load_hosts(dns, "/etc/hosts");
    
    kk::ev_openlibs(jsContext->jsContext(), base,dns);
    
    {
        
        kk::script::SetPrototype(ctx, &kk::WebSocket::ScriptClass);
        kk::script::SetPrototype(ctx, &kk::Http::ScriptClass);
        
        kk::Strong v = new kk::Http(base,dns);
        
        kk::script::PushObject(ctx, v.get());
        duk_put_global_string(ctx, "http");
    }
    
    app->run();
    
    bufferevent * ev_stdin = bufferevent_new(STDIN_FILENO, main_stdin_bufferevent_data_cb, NULL, NULL, app);
    
    bufferevent_base_set(base, ev_stdin);
    
    bufferevent_enable(ev_stdin, EV_READ);
    
    app_ev_exec =  evtimer_new(base, app_ev_exec_cb, app);
    
    kk::Uint frames = app->GAContext()->frames();
    
    timeval tv = {0, (int) (1000000 / frames)};
    
    evtimer_add(app_ev_exec, &tv);

    event_base_loop(base, 0);
    
    evtimer_del(app_ev_exec);
    
    evdns_base_free(dns, 0);
    
    app->release();
    
    jsContext->release();
    
    bufferevent_free(ev_stdin);
    
    return 0;
}
