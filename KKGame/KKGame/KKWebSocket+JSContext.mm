//
//  KKWebSocket+JSContext.m
//  KKGame
//
//  Created by zhanghailong on 2018/2/9.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#import "KKWebSocket+JSContext.h"

#include "kk-script.h"

extern dispatch_queue_t KKGLContextGetDispatchQueue(duk_context *ctx);

static duk_ret_t KKWebSocketDeallocFunc(duk_context * ctx) {

    duk_push_string(ctx, "__object");
    
    duk_get_prop(ctx, -2);
    
    if(duk_is_pointer(ctx, -1)) {
        @autoreleasepool{
            
            CFTypeRef ref =  (CFTypeRef) duk_to_pointer(ctx, -1);
            
            KKWebSocket * v = (__bridge KKWebSocket *) ref;
            
            v.onconnected = nil;
            v.ondata = nil;
            v.ontext = nil;
            v.ondisconnected = nil;
            [v disconnect];
            
            CFRelease(ref);
        }
    }
    
    duk_pop(ctx);
    
    return 0;
}

static duk_ret_t KKWebSocketAllocFunc(duk_context * ctx) {
    
    const char * url = NULL;
    int top = duk_get_top(ctx);
    
    if(top >0 && duk_is_string(ctx, - top)) {
        url = duk_to_string(ctx, -top);
    }
    
    if(url) {
        
        duk_push_object(ctx);
        
        CFTypeRef ref = nil;
        
        @autoreleasepool{
            
            KKWebSocket * v = [[KKWebSocket alloc] initWithURL:[NSURL URLWithString:[NSString stringWithCString:url encoding:NSUTF8StringEncoding]]];
            
            v.queue = KKGLContextGetDispatchQueue(ctx);
            
            dispatch_async(KKGLContextGetDispatchQueue(ctx), ^{
                [v connect];
            });
            
            ref = (__bridge CFTypeRef) v;
            
            CFRetain(ref);
            
            duk_push_string(ctx, "__object");
            duk_push_pointer(ctx, (void *) ref);
            
            duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_HAVE_WRITABLE | DUK_DEFPROP_CLEAR_WRITABLE);
            
        }
        
        duk_push_this(ctx);
        duk_set_prototype(ctx, -2);
        
        duk_push_c_function(ctx, KKWebSocketDeallocFunc, 1);
        duk_set_finalizer(ctx, -2);
        
        return 1;
    }
    
    return 0;
}

static duk_ret_t KKWebSocketDisconnectFunc(duk_context * ctx) {
    
    KKWebSocket * v = nil;
    
    duk_push_this(ctx);
    
    duk_push_string(ctx, "__object");
    duk_get_prop(ctx, -2);
    
    if(duk_is_pointer(ctx, -1)) {
        v = (__bridge KKWebSocket *) duk_to_pointer(ctx, -1);
    }
    
    duk_pop_2(ctx);
    
    @autoreleasepool {
        
        [v disconnect];
        
    }
    
    
    return 0;
}

static duk_ret_t KKWebSocketOnFunc(duk_context * ctx) {
    
    const char * name = NULL;
    int top = duk_get_top(ctx);
    void * heapptr = NULL;
    
    if(top >0 && duk_is_string(ctx, - top)) {
        name = duk_to_string(ctx, -top);
    }
    
    if(top > 1 && duk_is_function(ctx, - top +1)) {
        heapptr = duk_get_heapptr(ctx, - top + 1);
    }
    
    if(name) {
        
        KKWebSocket * v = nil;
        
        duk_push_this(ctx);
        
        duk_push_string(ctx, "__object");
        duk_get_prop(ctx, -2);
        
        if(duk_is_pointer(ctx, -1)) {
            v = (__bridge KKWebSocket *) duk_to_pointer(ctx, -1);
        }
        
        duk_pop_2(ctx);
        
        if(v) {
            
            if(strcmp(name, "open") == 0) {
                
                if(heapptr) {
                    v.onconnected = ^{
                        
                        duk_push_heapptr(ctx, heapptr);
                        
                        if(duk_is_function(ctx, -1)) {
                            if(DUK_EXEC_SUCCESS != duk_pcall(ctx, 0)) {
                                kk::script::Error(ctx, -1);
                            }
                            duk_pop(ctx);
                        } else {
                            duk_pop(ctx);
                        }
                        
                    };
                } else {
                    v.onconnected = nil;
                }
                
            } else if(strcmp(name, "data") == 0) {
                
                if(heapptr) {
                    v.ondata = ^(NSData * data) {
                        
                        duk_push_heapptr(ctx, heapptr);
                        
                        if(duk_is_function(ctx, -1)) {
                            
                            void * d = duk_push_fixed_buffer(ctx, [data length]);
                            
                            memcpy(d, [data bytes], [data length]);
                            
                            duk_push_buffer_object(ctx, -1, 0, [data length], DUK_BUFOBJ_UINT8ARRAY);
                            
                            duk_remove(ctx, -2);
                            
                            if(DUK_EXEC_SUCCESS != duk_pcall(ctx, 1)) {
                                kk::script::Error(ctx, -1);
                            }
                            
                            duk_pop(ctx);
                        } else {
                            duk_pop(ctx);
                        }
                    };
                }
                else {
                    v.ondata = nil;
                }
                
            } else if(strcmp(name, "text") == 0) {
                
                if(heapptr) {
                    v.ontext = ^(NSString * text) {
                        
                        duk_push_heapptr(ctx, heapptr);
                        
                        if(duk_is_function(ctx, -1)) {
                            
                            duk_push_string(ctx, [text UTF8String]);
                            
                            if(DUK_EXEC_SUCCESS != duk_pcall(ctx, 1)) {
                                kk::script::Error(ctx, -1);
                            }
                            
                            duk_pop(ctx);
                        } else {
                            duk_pop(ctx);
                        }
                        
                    };
                }
                else {
                    v.ontext = nil;
                }
                
            } else if(strcmp(name, "close") == 0) {
                
                if(heapptr) {
                    v.ondisconnected = ^(NSError * error){
                        
                        duk_push_heapptr(ctx, heapptr);
                        
                        if(duk_is_function(ctx, -1)) {
                            duk_push_string(ctx, [[error localizedDescription] UTF8String]);
                            if(DUK_EXEC_SUCCESS != duk_pcall(ctx, 1)) {
                                kk::script::Error(ctx, -1);
                            }
                            duk_pop(ctx);
                        } else {
                            duk_pop(ctx);
                        }
                    };
                }
                else {
                    v.ondisconnected = nil;
                }
                
            }
            
        }
        
        duk_push_this(ctx);
        
        if(heapptr) {
            duk_push_sprintf(ctx, "__on_%s",name);
            duk_push_heapptr(ctx, heapptr);
            duk_put_prop(ctx, -3);
        } else {
            duk_push_sprintf(ctx, "__on_%s",name);
            duk_del_prop(ctx, -2);
        }
        
        duk_pop(ctx);
        
    }
    
    
    
    
    return 0;
}

static duk_ret_t KKWebSocketWriteFunc(duk_context * ctx) {
    
    KKWebSocket * v = nil;
    
    duk_push_this(ctx);
    
    duk_push_string(ctx, "__object");
    duk_get_prop(ctx, -2);
    
    if(duk_is_pointer(ctx, -1)) {
        v = (__bridge KKWebSocket *) duk_to_pointer(ctx, -1);
    }
    
    duk_pop_2(ctx);
    
    @autoreleasepool {
        
        int top = duk_get_top(ctx);
        
        if(top >0 ) {
            if(duk_is_string(ctx, - top)) {
                [v writeString:[NSString stringWithCString:duk_to_string(ctx, -top) encoding:NSUTF8StringEncoding]];
            } else if(duk_is_buffer_data(ctx, -top)) {
                duk_size_t n;
                void * bytes = duk_get_buffer_data(ctx, - top, &n);
                [v writeData:[NSData dataWithBytes:bytes length:n]];
            }
        }
        
    }
    
    return 0;
}

static duk_ret_t KKWebSocketStateFunc(duk_context * ctx) {
    
    KKWebSocket * v = nil;
    
    duk_push_this(ctx);
    
    duk_push_string(ctx, "__object");
    duk_get_prop(ctx, -2);
    
    if(duk_is_pointer(ctx, -1)) {
        v = (__bridge KKWebSocket *) duk_to_pointer(ctx, -1);
    }
    
    duk_pop_2(ctx);
    
    duk_push_int(ctx, v.state);
    
    return 1;
}


@implementation KKWebSocket (JSContext)

+(void) openlib:(duk_context *) ctx {
    
    duk_push_global_object(ctx);
    
    duk_push_string(ctx, "WebSocket");
    
    duk_push_object(ctx);
    
    duk_push_string(ctx, "alloc");
    duk_push_c_function(ctx, KKWebSocketAllocFunc, 1);
    duk_put_prop(ctx, -3);
    
    duk_push_string(ctx, "close");
    duk_push_c_function(ctx, KKWebSocketDisconnectFunc, 0);
    duk_put_prop(ctx, -3);
    
    duk_push_string(ctx, "on");
    duk_push_c_function(ctx, KKWebSocketOnFunc, 2);
    duk_put_prop(ctx, -3);
    
    duk_push_string(ctx, "send");
    duk_push_c_function(ctx, KKWebSocketWriteFunc, 1);
    duk_put_prop(ctx, -3);
    
    duk_push_string(ctx, "state");
    duk_push_c_function(ctx, KKWebSocketStateFunc, 0);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_GETTER );
    
    duk_put_prop(ctx, -3);
    
    duk_pop(ctx);
}

@end
