//
//  KKStrong.m
//  KKGame
//
//  Created by zhanghailong on 2018/2/5.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#import "KKStrong.h"

@implementation KKStrong

@synthesize object = _object;

-(void) dealloc {
    if(_object) {
        _object->release();
    }
}

-(instancetype) initWithObject:(kk::Object *) object {
    if((self = [super init])) {
        _object = object;
        if(_object) {
            _object->retain();
        }
    }
    return self;
}

@end
