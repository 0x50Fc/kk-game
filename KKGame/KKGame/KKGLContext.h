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

struct duk_hthread;

@interface KKGLContext : EAGLContext

@property(nonatomic,strong,readonly) dispatch_queue_t queue;
@property(nonatomic,strong) id<KKHttp> http;
@property(nonatomic,strong) NSString * basePath;

-(void) recycle;

+(void) JSContextPushObject:(id) object ctx:(struct duk_hthread * )ctx;

+(id) JSContextToObject:(int) idx ctx:(struct duk_hthread * )ctx;

@end

