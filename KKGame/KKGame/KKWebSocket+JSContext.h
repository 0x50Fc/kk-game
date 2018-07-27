//
//  KKWebSocket+JSContext.h
//  KKGame
//
//  Created by zhanghailong on 2018/2/9.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#import <KKWebSocket/KKWebSocket.h>
#include <KKGame/duktape.h>

@interface KKWebSocket (JSContext)

+(void) openlib:(duk_context *) ctx queue:(dispatch_queue_t) queue;

@end
