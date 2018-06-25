//
//  KKGLContext.h
//  KKGame
//
//  Created by zhanghailong on 2018/2/5.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <KKHttp/KKHttp.h>
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#include <OpenGLES/EAGL.h>
#include <KKGame/duktape.h>

#ifdef __cplusplus

namespace kk {
    
    namespace GL {
        class Context;
    }
    
    namespace script {
        class Context;
    }
}

typedef kk::GL::Context * KKGLContextRef;
typedef kk::script::Context * KKJSContextRef;

#else

typedef void * KKGLContextRef;
typedef void * KKJSContextRef;

#endif

@interface KKGLContext : EAGLContext

@property(nonatomic,strong,readonly) dispatch_queue_t queue;
@property(nonatomic,strong) id<KKHttp> http;
@property(nonatomic,readonly,assign) KKGLContextRef GLContext;
@property(nonatomic,readonly,assign) KKJSContextRef JSContext;
@property(nonatomic,readonly,assign,getter=isRecycle) BOOL recycle;

-(void) recycle;

+(void) JSContextPushObject:(id) object ctx:(duk_context * )ctx;

+(id) JSContextToObject:(duk_idx_t) idx ctx:(duk_context * )ctx;

@end

