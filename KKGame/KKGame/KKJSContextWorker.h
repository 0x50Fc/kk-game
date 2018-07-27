//
//  KKJSContextWorker.h
//  KKGame
//
//  Created by hailong11 on 2018/7/27.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <KKGame/duktape.h>

typedef void (*KKJSContextWorkerOnCreateContext) (duk_context * ctx, duk_context * newContext,dispatch_queue_t queue);

@interface KKJSContextWorker : NSObject

+(void) openlib:(duk_context *) ctx queue:(dispatch_queue_t) queue onCreateContext:(KKJSContextWorkerOnCreateContext) onCreateContext;

@end
