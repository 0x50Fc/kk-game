//
//  KKStrong.h
//  KKGame
//
//  Created by zhanghailong on 2018/2/5.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface KKStrong : NSObject

@property(nonatomic,assign,readonly) kk::Object * object;

-(instancetype) initWithObject:(kk::Object *) object;

@end
