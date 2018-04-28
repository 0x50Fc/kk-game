//
//  KKWebSocket+JSContext.h
//  KKGame
//
//  Created by hailong11 on 2018/2/9.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#import <KKWebSocket/KKWebSocket.h>
#include <KKGame/duktape.h>

@interface KKWebSocket (JSContext)

+(void) openlib:(duk_context *) ctx;

@end
