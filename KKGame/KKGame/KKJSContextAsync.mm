//
//  KKJSContextAsync.m
//  KKGame
//
//  Created by hailong11 on 2018/7/27.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#import "KKJSContextAsync.h"
#include "kk-script.h"

static dispatch_queue_t KKJSContextAsyncGetDispatchQueue(duk_context *ctx) {
    dispatch_queue_t v = nil;
    duk_push_current_function(ctx);
    duk_get_prop_string(ctx, -1, "__queue");
    if(duk_is_pointer(ctx, -1)) {
        v = (__bridge dispatch_queue_t) duk_to_pointer(ctx, -1);
    }
    duk_pop_2(ctx);
    return v;
}

static duk_ret_t KKJSContextAsync_set(duk_context * ctx, BOOL isRepeat);

static duk_ret_t KKJSContextAsync_setTimeout(duk_context * ctx) {
    return KKJSContextAsync_set(ctx,NO);
}

static duk_ret_t KKJSContextAsync_setInterval(duk_context * ctx) {
    return KKJSContextAsync_set(ctx,YES);
}

static duk_ret_t KKJSContextAsync_dealloc(duk_context * ctx) {
    
    duk_push_string(ctx, "source");
    duk_get_prop(ctx, -2);
    
    if(duk_is_pointer(ctx, -1)) {
        @autoreleasepool {
            CFTypeRef v = (CFTypeRef) duk_to_pointer(ctx, -1);
            if(v) {
                dispatch_source_t source = (__bridge dispatch_source_t) v;
                dispatch_source_cancel(source);
                CFRelease(v);
            }
        }
    }
    
    duk_pop(ctx);
    
    return 0;
}

static duk_ret_t KKJSContextAsync_set(duk_context * ctx, BOOL isRepeat) {
    
    int top = duk_get_top(ctx);
    
    if(top > 1 && duk_is_function(ctx, -top) && duk_is_number(ctx, -top + 1)) {
        
        @autoreleasepool{
            
            void * heapptr = duk_get_heapptr(ctx, -top);
            int ms = duk_to_int(ctx, -top +1);
            dispatch_queue_t queue = KKJSContextAsyncGetDispatchQueue(ctx);
            
            duk_push_global_object(ctx);
            
            duk_push_sprintf(ctx, "0x%x",(long) heapptr);
            
            duk_push_object(ctx);
            
            duk_push_string(ctx, "source");
            
            dispatch_source_t source = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0,queue);
            
            duk_push_pointer(ctx, (void *) CFBridgingRetain(source));
            duk_put_prop(ctx, -3);
            
            duk_push_string(ctx, "func");
            duk_push_heapptr(ctx, heapptr);
            duk_put_prop(ctx, -3);
            
            duk_push_c_function(ctx, KKJSContextAsync_dealloc, 1);
            duk_set_finalizer(ctx, -2);
            
            duk_put_prop(ctx, -3);
            
            duk_pop(ctx);
        
            dispatch_source_set_timer(source, dispatch_walltime(NULL, 0), (int64_t)(ms * NSEC_PER_MSEC), 0);
            
            dispatch_source_set_event_handler(source, ^{
                
                duk_push_global_object(ctx);
                
                duk_push_sprintf(ctx, "0x%x",(long) heapptr);
                duk_get_prop(ctx, -2);
                
                if(duk_is_object(ctx, -1)) {
                    
                    duk_push_string(ctx, "func");
                    duk_get_prop(ctx, -2);
                    
                    if(duk_is_function(ctx, -1)) {
                        if(DUK_EXEC_SUCCESS != duk_pcall(ctx, 0)) {
                            kk::script::Error(ctx, -1);
                        }
                    }
                    
                    duk_pop(ctx);
                    
                }
                
                duk_pop(ctx);
                
                if(!isRepeat) {
                    duk_push_sprintf(ctx, "0x%x",(long) heapptr);
                    duk_del_prop(ctx, -2);
                }
                
                duk_pop(ctx);
                
            });
            
            dispatch_resume(source);
            
            duk_push_sprintf(ctx, "0x%x",(long) heapptr);
            
            return 1;
            
        }
        
    }
    
    return 0;
}

static duk_ret_t KKJSContextAsync_clear(duk_context * ctx) {
    
    int top = duk_get_top(ctx);
    
    if(top > 0 && duk_is_string(ctx, -top) ) {
        
        const char * iid = duk_to_string(ctx, -top);
        
        duk_push_global_object(ctx);
        
        duk_push_string(ctx, iid);
        duk_del_prop(ctx, -2);
        
        duk_pop(ctx);
        
    }
    
    return 0;
}

@implementation KKJSContextAsync

+(void) openlib:(duk_context *) ctx queue:(dispatch_queue_t) queue {
    
    duk_push_global_object(ctx);
    
    duk_push_string(ctx, "setTimeout");
    duk_push_c_function(ctx, KKJSContextAsync_setTimeout, 2);
    
    duk_push_string(ctx, "__queue");
    duk_push_pointer(ctx, (__bridge void *) queue);
    duk_put_prop(ctx, -3);
    
    duk_put_prop(ctx, -3);
    
    duk_push_string(ctx, "setInterval");
    duk_push_c_function(ctx, KKJSContextAsync_setInterval, 2);
    
    duk_push_string(ctx, "__queue");
    duk_push_pointer(ctx, (__bridge void *) queue);
    duk_put_prop(ctx, -3);
    
    duk_put_prop(ctx, -3);
    
    duk_push_string(ctx, "clearTimeout");
    duk_push_c_function(ctx, KKJSContextAsync_clear, 1);
    
    duk_push_string(ctx, "__queue");
    duk_push_pointer(ctx, (__bridge void *) queue);
    duk_put_prop(ctx, -3);
    
    duk_put_prop(ctx, -3);
    
    duk_push_string(ctx, "clearInterval");
    duk_push_c_function(ctx, KKJSContextAsync_clear, 1);
    
    duk_push_string(ctx, "__queue");
    duk_push_pointer(ctx, (__bridge void *) queue);
    duk_put_prop(ctx, -3);
    
    duk_put_prop(ctx, -3);
    
    duk_pop(ctx);
}

@end
