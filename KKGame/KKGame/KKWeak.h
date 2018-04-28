//
//  KKWeak.h
//  KKGame
//
//  Created by hailong11 on 2018/2/5.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#import <Foundation/Foundation.h>
#include <KKGame/kk-object.h>

@interface KKWeak : NSObject

@property(nonatomic,assign,readonly) kk::Object * object;

-(instancetype) initWithObject:(kk::Object *) object;

@end
