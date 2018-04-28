//
//  KKGamepadElement.m
//  demo
//
//  Created by hailong11 on 2018/3/30.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#import "KKGamepadElement.h"
#include <objc/runtime.h>

@interface UITouch(KKGamepadView)

@property(nonatomic,assign) CGPoint kk_location;

@end

@implementation UITouch(KKGamepadView)

-(CGPoint) kk_location {
    return [objc_getAssociatedObject(self, "_kk_location") CGPointValue];
}

-(void) setKk_location:(CGPoint)kk_location {
    objc_setAssociatedObject(self, "_kk_location", [NSValue valueWithCGPoint:kk_location], OBJC_ASSOCIATION_RETAIN_NONATOMIC);
}

@end

@interface KKGamepadView : UIView

@property(nonatomic,strong) UITouch * panTouch;
@property(nonatomic,weak) KKElement * element;

@end

@implementation KKGamepadView



-(void) touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    
    for(UITouch * touch in touches) {
        touch.kk_location = [touch locationInView:self];
    }
    
}

-(void) touchEnded:(UITouch *) touch {
    
    CGPoint p = touch.kk_location;
    CGPoint mv = [touch locationInView:self];
    
    if(_panTouch == touch) {
        
        CGSize size = self.bounds.size;
        CGFloat dw = size.width * 0.5f;
        CGFloat dh = size.height * 0.5f;
        
        NSMutableDictionary * data = [NSMutableDictionary dictionaryWithCapacity:4];
        
        data[@"control_type"] = @"gamepad";
        data[@"type"] = @"end";
        
        data[@"x"] = @((mv.x - p.x) / dw);
        data[@"y"] = @((mv.y - p.y) / dh);
        
        KKElementEvent * event = [[KKElementEvent alloc] initWithElement:self.element];
        
        event.data = data;
        
        [self.element emit:@"pan" event:event];
        
        _panTouch = nil;
        
        return ;
    }
    
    NSMutableDictionary * data = [NSMutableDictionary dictionaryWithCapacity:4];
    
    CGSize size = self.bounds.size;
    CGFloat dw = size.width * 0.5f;
    CGFloat dh = size.height * 0.5f;
    
    data[@"control_type"] = @"gamepad";
    data[@"x"] = @((p.x - dw) / dw);
    data[@"y"] = @((p.y - dh) / dh);
    
    KKElementEvent * event = [[KKElementEvent alloc] initWithElement:self.element];
    
    event.data = data;
    
    [self.element emit:@"tap" event:event];
}

-(void) touchMoved:(UITouch *) touch {
    
    BOOL isBegin = NO;
    
    if(_panTouch == nil) {
        
        CGPoint p = touch.kk_location;
        CGPoint mv = [touch locationInView:self];
        
        CGFloat dx = p.x - mv.x;
        CGFloat dy = p.y - mv.y;
        CGFloat r = dx * dx + dy * dy;
        
        if(r > 25) {
            _panTouch = touch;
            isBegin = YES;
        }
        
    }
    
    if(_panTouch == touch) {
        
        CGSize size = self.bounds.size;
        CGFloat dw = size.width * 0.5f;
        CGFloat dh = size.height * 0.5f;
        
        
        CGPoint p = touch.kk_location;
        CGPoint mv = [touch locationInView:self];
        
        NSMutableDictionary * data = [NSMutableDictionary dictionaryWithCapacity:4];
        
        data[@"control_type"] = @"gamepad";
        data[@"type"] = isBegin ? @"begin" : @"moved";
        
        data[@"x"] = @((mv.x - p.x) / dw);
        data[@"y"] = @((mv.y - p.y) / dh);
        
        KKElementEvent * event = [[KKElementEvent alloc] initWithElement:self.element];
        
        event.data = data;
        
        [self.element emit:@"pan" event:event];
        
        return;
    }
    
}

-(void) touchCancelled:(UITouch *) touch {
    
    if(_panTouch == touch) {
        [self touchEnded:touch];
        return;
    }
    
}

-(void) touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    for(UITouch * touch in touches) {
        [self touchEnded:touch];
    }
}

-(void) touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    for(UITouch * touch in touches) {
        [self touchMoved:touch];
    }
}

-(void) touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    for(UITouch * touch in touches) {
        [self touchCancelled:touch];
    }
}

@end

@implementation KKGamepadElement

+(void) initialize{
    [super initialize];
    [KKViewContext setDefaultElementClass:[KKGamepadElement class] name:@"gamepad"];
}

-(instancetype) init {
    if((self = [super init])) {
        [self set:@"view" value:NSStringFromClass([KKGamepadView class])];
    }
    return self;
}

-(void) setView:(UIView *)view {
    [(KKGamepadView *) self.view setElement:nil];
    [super setView:view];
    [(KKGamepadView *) self.view setElement:self];
    [self.view setMultipleTouchEnabled:YES];
}

@end
