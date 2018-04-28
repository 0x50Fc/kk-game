//
//  KKWeak.m
//  KKGame
//
//  Created by hailong11 on 2018/2/5.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#import "KKWeak.h"

@implementation KKWeak

@synthesize object = _object;

-(void) dealloc {
    if(_object) {
        _object->unWeak(& _object);
    }
}

-(instancetype) initWithObject:(kk::Object *) object {
    if((self = [super init])) {
        _object = object;
        if(_object) {
            _object->weak(& _object);
        }
    }
    return self;
}

@end
