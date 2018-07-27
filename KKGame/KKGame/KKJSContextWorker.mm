//
//  KKJSContextWorker.m
//  KKGame
//
//  Created by hailong11 on 2018/7/27.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#import "KKJSContextWorker.h"
#include "kk-script.h"

static duk_ret_t KKJSContextWorkerDeallocFunc(duk_context * ctx) {
    
    kk::script::Context * jsContext = nil;
    
    duk_push_string(ctx, "__jsContext");
    duk_get_prop(ctx, -2);
    
    if(duk_is_pointer(ctx, -1)) {
        jsContext = (kk::script::Context *) duk_to_pointer(ctx, -1);
    }
    
    duk_pop(ctx);
    
    dispatch_queue_t queue = nil;
    
    duk_push_string(ctx, "__queue");
    duk_get_prop(ctx, -2);
    
    if(duk_is_pointer(ctx, -1)) {
        queue = CFBridgingRelease(duk_to_pointer(ctx, -1));
    }
    
    duk_pop(ctx);
    
    if(queue != nil && jsContext != nil) {
        
        dispatch_sync(queue, ^{
            jsContext->release();
        });
        
    }
    
    return 0;
}

static duk_ret_t KKJSContextWorkerThreadPostMessageFunc(duk_context * ctx) {
    
    
    char * json = nil;
    
    int top = duk_get_top(ctx);
    
    if(top > 0) {
        const char * v = duk_json_encode(ctx, -top);
        json = (char *) malloc(strlen(v) + 1);
        strcpy(json, v);
    } else {
        return 0;
    }
    
    duk_push_current_function(ctx);
    
    duk_context * jsContext = nil;
    dispatch_queue_t queue = nil;
    void * heapptr = nil;
    
    duk_push_string(ctx, "__jsContext");
    duk_get_prop(ctx, -2);
    
    if(duk_is_pointer(ctx, -1)) {
        jsContext = (duk_context *) duk_to_pointer(ctx, -1);
    }
    
    duk_pop(ctx);
    
    duk_push_string(ctx, "__queue");
    duk_get_prop(ctx, -2);
    
    if(duk_is_pointer(ctx, -1)) {
        queue = (__bridge id) duk_to_pointer(ctx, -1);
    }
    
    duk_pop(ctx);
    
    duk_push_string(ctx, "__heapptr");
    duk_get_prop(ctx, -2);
    
    if(duk_is_pointer(ctx, -1)) {
        heapptr =  duk_to_pointer(ctx, -1);
    }
    
    duk_pop(ctx);
    
    duk_pop(ctx);
    
    if(jsContext && queue && heapptr) {
        
        dispatch_async(queue, ^{
           
            duk_push_heapptr(jsContext, heapptr);
            
            duk_push_string(jsContext, "onmessage");
            duk_get_prop(jsContext, -2);
            
            if(duk_is_function(jsContext, -1)) {
                
                duk_push_string(jsContext, json);
                duk_json_decode(jsContext, -1);
                
                if(duk_pcall(jsContext, 1) != DUK_EXEC_SUCCESS) {
                    kk::script::Error(jsContext, -1);
                }
            }
            
            duk_pop_2(jsContext);
            
            free(json);
        });
        
    } else {
        free(json);
    }
    
    return 0;
}

static duk_ret_t KKJSContextWorkerAllocFunc(duk_context * ctx) {
    
    int top = duk_get_top(ctx);
    
    if(top > 0 && duk_is_function(ctx, -top) ) {
        
        duk_dup(ctx, -top);
        duk_dump_function(ctx);
        
        duk_size_t n;
        void * data = duk_get_buffer(ctx, -1, &n);
        void * queue = nil;
        KKJSContextWorkerOnCreateContext onCreateContext = nil;
        
        kk::script::Context * jsContext = new kk::script::Context();
        
        jsContext->retain();
        
        duk_push_current_function(ctx);
        
        duk_push_string(ctx, "__onCreateContext");
        duk_get_prop(ctx, -2);
        
        if(duk_is_pointer(ctx, -1)) {
            onCreateContext = (KKJSContextWorkerOnCreateContext) duk_to_pointer(ctx, -1);
        }
        
        duk_pop(ctx);
        
        duk_push_string(ctx, "__queue");
        duk_get_prop(ctx, -2);
        
        if(duk_is_pointer(ctx, -1)) {
            queue = duk_to_pointer(ctx, -1);
        }
        
        duk_pop(ctx);
        
        duk_pop(ctx);
        
        void * toData = malloc(n);
        
        memcpy(toData, data, n);
        
        duk_pop(ctx);
        
        duk_push_object(ctx);
        duk_push_c_function(ctx, KKJSContextWorkerDeallocFunc, 1);
        duk_set_finalizer(ctx, -2);
        
        duk_push_this(ctx);
        duk_set_prototype(ctx, -2);
        
        duk_push_string(ctx, "__jsContext");
        duk_push_pointer(ctx, jsContext);
        duk_put_prop(ctx, -3);
        
        dispatch_queue_t toQueue = dispatch_queue_create("KKJSContextWorker", nil);
        
        duk_push_string(ctx, "__queue");
        duk_push_pointer(ctx, (void *) CFBridgingRetain(toQueue));
        duk_put_prop(ctx, -3);
        
        void * heapptr = duk_get_heapptr(ctx, -1);
        
        if(onCreateContext) {
            (*onCreateContext)(ctx,jsContext->jsContext(),toQueue);
        }
        
        {
            duk_context * dukContext = jsContext->jsContext();
            
            duk_push_global_object(dukContext);
            
            duk_push_string(dukContext, "postMessage");
            duk_push_c_function(dukContext, KKJSContextWorkerThreadPostMessageFunc, 1);
            duk_push_string(dukContext, "__jsContext");
            duk_push_pointer(dukContext, (void *) ctx);
            duk_put_prop(dukContext, -3);
            duk_push_string(dukContext, "__queue");
            duk_push_pointer(dukContext, queue);
            duk_put_prop(dukContext, -3);
            duk_push_string(dukContext, "__heapptr");
            duk_push_pointer(dukContext, heapptr);
            duk_put_prop(dukContext, -3);
            
            duk_put_prop(dukContext, -3);
            
            duk_pop(dukContext);
            
            dispatch_async(toQueue, ^{
                
                void * v = duk_push_fixed_buffer(dukContext, n);
                
                memcpy(v, toData, n);
                
                duk_load_function(dukContext);
                
                if(duk_pcall(dukContext, 0) != DUK_EXEC_SUCCESS) {
                    kk::script::Error(dukContext, -1);
                }
                
                duk_pop(dukContext);
                
                free(toData);
                
            });
        }
        
        return 1;
    }
    
    return 0;
}

static duk_ret_t KKJSContextWorkerPostMessageFunc(duk_context * ctx) {
    
    char * json = nil;
    
    int top = duk_get_top(ctx);
    
    if(top > 0) {
        const char * v = duk_json_encode(ctx, -top);
        json = (char *) malloc(strlen(v) + 1);
        strcpy(json, v);
    } else {
        return 0;
    }
    
    duk_push_this(ctx);
    
    kk::script::Context * jsContext = nil;
    
    duk_push_string(ctx, "__jsContext");
    duk_get_prop(ctx, -2);
    
    if(duk_is_pointer(ctx, -1)) {
        jsContext = (kk::script::Context *) duk_to_pointer(ctx, -1);
    }
    
    duk_pop(ctx);
    
    dispatch_queue_t queue = nil;
    
    duk_push_string(ctx, "__queue");
    duk_get_prop(ctx, -2);
    
    if(duk_is_pointer(ctx, -1)) {
        queue = (__bridge id)(duk_to_pointer(ctx, -1));
    }
    
    duk_pop(ctx);
    
    duk_pop(ctx);
    
    if(queue != nil && jsContext != nil) {
        
        dispatch_sync(queue, ^{
            
            duk_context * dukContext = jsContext->jsContext();
            
            duk_push_global_object(dukContext);
            
            duk_push_string(dukContext, "onmessage");
            duk_get_prop(dukContext, -2);
            
            if(duk_is_function(dukContext, -1)) {
                
                duk_push_string(dukContext, json);
                duk_json_decode(dukContext, -1);
                
                if(duk_pcall(dukContext, 1) != DUK_EXEC_SUCCESS) {
                    kk::script::Error(dukContext, -1);
                }
                
            }
            
            duk_pop_2(dukContext);
            
            free(json);
            
        });
        
    } else {
        free(json);
    }
    
    
    
    return 0;
}

static duk_ret_t KKJSContextWorkerTerminateFunc(duk_context * ctx) {
    
    duk_push_this(ctx);
    
    kk::script::Context * jsContext = nil;
    
    duk_push_string(ctx, "__jsContext");
    duk_get_prop(ctx, -2);
    
    if(duk_is_pointer(ctx, -1)) {
        jsContext = (kk::script::Context *) duk_to_pointer(ctx, -1);
    }
    
    duk_pop(ctx);
    
    dispatch_queue_t queue = nil;
    
    duk_push_string(ctx, "__queue");
    duk_get_prop(ctx, -2);
    
    if(duk_is_pointer(ctx, -1)) {
        queue = CFBridgingRelease(duk_to_pointer(ctx, -1));
    }
    
    duk_pop(ctx);
    
    if(queue != nil && jsContext != nil) {
        
        dispatch_sync(queue, ^{
            jsContext->release();
        });
        
    }
    
    duk_push_string(ctx, "__jsContext");
    duk_del_prop(ctx, -2);
    
    duk_push_string(ctx, "__queue");
    duk_del_prop(ctx, -2);
    
    duk_pop(ctx);
    
    return 0;
}

@implementation KKJSContextWorker

+(void) openlib:(duk_context *) ctx queue:(dispatch_queue_t) queue onCreateContext:(KKJSContextWorkerOnCreateContext) onCreateContext  {
    
    duk_push_global_object(ctx);
    
    duk_push_string(ctx, "WebWorker");
    
    duk_push_object(ctx);
    
    duk_push_string(ctx, "alloc");
    duk_push_c_function(ctx, KKJSContextWorkerAllocFunc, 1);
    duk_push_string(ctx, "__onCreateContext");
    duk_push_pointer(ctx, (void *) onCreateContext);
    duk_put_prop(ctx, -3);
    duk_push_string(ctx, "__queue");
    duk_push_pointer(ctx, (__bridge void *) queue);
    duk_put_prop(ctx, -3);
    duk_put_prop(ctx, -3);
    
    duk_push_string(ctx, "postMessage");
    duk_push_c_function(ctx, KKJSContextWorkerPostMessageFunc, 1);
    duk_put_prop(ctx, -3);
    
    duk_push_string(ctx, "terminate");
    duk_push_c_function(ctx, KKJSContextWorkerTerminateFunc, 0);
    duk_put_prop(ctx, -3);
    
    duk_put_prop(ctx, -3);
    
    duk_pop(ctx);
    
}

@end
