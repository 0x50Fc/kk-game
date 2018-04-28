//
//  KKGestureElement.m
//  demo
//
//  Created by zuowu on 2018/3/22.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#import "KKGestureElement.h"

@interface KKGestureItemElement()<UIGestureRecognizerDelegate>

@end

@implementation KKGestureItemElement

@synthesize gestureRecognizer = _gestureRecognizer;

-(instancetype) init {
    if((self = [super init])) {
        //初始值
        _touchesRequired = -1;
        _tapsRequired = -1;
        _longPressDuration = -1;
        _panMinTouches = -1;
        _panMaxTouches = -1;
    }
    return self;
}

- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldReceiveTouch:(UITouch *)touch {
    return YES;
}

- (BOOL)gestureRecognizerShouldBegin:(UIGestureRecognizer *)gestureRecognizer {
    return YES;
}

-(UIGestureRecognizer *) gestureRecognizer {
    
    if(_gestureRecognizer == nil) {
        NSString * name = [self get:@"#name"];
//        NSLog(@"name == %@",name);
        if([name isEqualToString:@"gesture:tap"]) {
            
            UITapGestureRecognizer * recognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(tapAction:)];
            _gestureRecognizer = recognizer;
            
        }else if ([name isEqualToString:@"gesture:long-press"]) {
            
            UILongPressGestureRecognizer * recognizer = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(longPressAction:)];
            recognizer.numberOfTouchesRequired = _touchesRequired == -1 ? 1 : _touchesRequired;
            recognizer.numberOfTapsRequired = _tapsRequired == -1 ? 0 : _tapsRequired;
            recognizer.minimumPressDuration = _longPressDuration == -1 ? 0.5 : _longPressDuration;
            _gestureRecognizer = recognizer;
            
        }else if ([name isEqualToString:@"gesture:pan"]) {
            
            UIPanGestureRecognizer * recognizer = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(panAction:)];
            recognizer.minimumNumberOfTouches = _panMinTouches == -1 ? 1 : _panMinTouches;
            recognizer.maximumNumberOfTouches = _panMinTouches == -1 ? UINT_MAX : _panMaxTouches;
            _gestureRecognizer = recognizer;
            
        }else if ([name isEqualToString:@"gesture:pinch"]) {
            
            UIPinchGestureRecognizer * recognizer = [[UIPinchGestureRecognizer alloc] initWithTarget:self action:@selector(pinchAction:)];
            _gestureRecognizer = recognizer;
            
        }else if ([name isEqualToString:@"gesture:rotation"]) {
            
            UIRotationGestureRecognizer * recognizer = [[UIRotationGestureRecognizer alloc] initWithTarget:self action:@selector(rotationAction:)];
            _gestureRecognizer = recognizer;
            
        }
        
        else {
            //为空会造成空指针访问 默认创建基类
            _gestureRecognizer = [[UIGestureRecognizer alloc] initWithTarget:self action:@selector(tapAction:)];
        }
    }
    return _gestureRecognizer;
}
/*
    * action 会在手势识别调用,所有手势都在父类中定义了状态 state ,不同手势识别后会在不同的状态下调用回调方法
    * state
        typedef NS_ENUM(NSInteger, UIGestureRecognizerState) {
            UIGestureRecognizerStatePossible,
            UIGestureRecognizerStateBegan,
            UIGestureRecognizerStateChanged,
            UIGestureRecognizerStateEnded,
            UIGestureRecognizerStateCancelled,
            UIGestureRecognizerStateFailed,
            UIGestureRecognizerStateRecognized = UIGestureRecognizerStateEnded
        };
    * 每个手势会在某些state时候回调,并发送state状态,接收使用的时候可以根据state来进行操作
    * 下面的回调函数会在这些state时候发送 当前手势所有触摸点 以及每种手势的特定属性
    * 手势之间会有冲突,滑动会覆盖点击等,需要实际使用测试
 */
-(void) tapAction:(UITapGestureRecognizer *)sender{
    NSDictionary * data = [self recognizerEventData:sender typeName:@"tap" state:sender.state];
    KKElementEvent * event = [[KKElementEvent alloc] initWithElement:self];
    event.data = data;
    [self emit:@"change" event:event];
}

-(void) longPressAction:(UILongPressGestureRecognizer *)sender{
    NSDictionary * data = [self recognizerEventData:sender typeName:@"long-press" state:sender.state];
    KKElementEvent * event = [[KKElementEvent alloc] initWithElement:self];
    event.data = data;
    [self emit:@"change" event:event];
}

-(void) panAction:(UIPanGestureRecognizer *)sender{
    NSDictionary * data = [self recognizerEventData:sender typeName:@"pan" state:sender.state];
    KKElementEvent * event = [[KKElementEvent alloc] initWithElement:self];
    event.data = data;
    [self emit:@"change" event:event];
}

-(void) pinchAction:(UIPinchGestureRecognizer *)sender{
    NSDictionary * data = [self recognizerEventData:sender typeName:@"pinch" state:sender.state];
    [data setValue:@"pinch-scale" forKey:[NSString stringWithFormat:@"%f", sender.scale]];
    [data setValue:@"pinch-velocity" forKey:[NSString stringWithFormat:@"%f", sender.velocity]];
    KKElementEvent * event = [[KKElementEvent alloc] initWithElement:self];
    event.data = data;
    [self emit:@"change" event:event];
}

-(void) rotationAction:(UIRotationGestureRecognizer *)sender{
    NSDictionary * data = [self recognizerEventData:sender typeName:@"rotation" state:sender.state];
    [data setValue:@"rotation" forKey:[NSString stringWithFormat:@"%f", sender.rotation]];
    [data setValue:@"rotation-velocity" forKey:[NSString stringWithFormat:@"%f", sender.velocity]];
    KKElementEvent * event = [[KKElementEvent alloc] initWithElement:self];
    event.data = data;
    [self emit:@"change" event:event];
}

-(NSDictionary *)recognizerEventData:(UIGestureRecognizer *) sender typeName:(NSString *) typeName state:(NSInteger) state{
    
    //触摸点
    KKGestureElement * element = (KKGestureElement *)self.parent;
    NSMutableArray * arr = [[NSMutableArray alloc] init];
    NSLog(@" sender.numberOfTouches == %ld", sender.numberOfTouches);
    if (element && element.view) {
        for (int i = 0 ; i < sender.numberOfTouches; i++) {
            CGPoint point = [sender locationOfTouch:i inView:element.view];
            NSString * x = [NSString stringWithFormat:@"%f",point.x];
            NSString * y = [NSString stringWithFormat:@"%f",point.y];
            NSDictionary * dic = @{@"x":x,@"y":y};
            [arr addObject:dic];
        }
    }
    
    //数据
    NSMutableDictionary * data = [self data];
    [data setValue:arr forKey:@"points"];
    [data setValue:typeName forKey:@"type"];
    [data setValue:[NSString stringWithFormat:@"%ld", state] forKey:@"state"];
    NSLog(@"data = %@", data);
    
    return data;
}


-(void)changedKey:(NSString *)key{
//    NSLog(@"KKGestureItemElement change key");

    NSString * value = [self get:key];
    if ([key isEqualToString:@"touches"]) {
        _touchesRequired = [value integerValue];
    }else if ([key isEqualToString:@"taps"]) {
        _tapsRequired = [value integerValue];
    }else if ([key isEqualToString:@"long-press-duration"]) {
        _longPressDuration = [value floatValue];
    }else if ([key isEqualToString:@"pan-min"]) {
        _panMinTouches = [value integerValue];
    }else if ([key isEqualToString:@"pan-max"]) {
        _panMaxTouches = [value integerValue];
    }
    
}
@end

@implementation KKGestureElement

+(void) initialize{
    [super initialize];
    [KKViewContext setDefaultElementClass:[KKGestureElement class] name:@"gesture"];
    [KKViewContext setDefaultElementClass:[KKGestureItemElement class] name:@"gesture:tap"];
    [KKViewContext setDefaultElementClass:[KKGestureItemElement class] name:@"gesture:long-press"];
    [KKViewContext setDefaultElementClass:[KKGestureItemElement class] name:@"gesture:pan"];
    [KKViewContext setDefaultElementClass:[KKGestureItemElement class] name:@"gesture:pinch"];
    [KKViewContext setDefaultElementClass:[KKGestureItemElement class] name:@"gesture:rotation"];
}


-(void) willRemoveChildren:(KKElement *)element {
    [super willRemoveChildren:element];
    if(self.view) {
        if([element isKindOfClass:[KKGestureItemElement class]] && self.view) {
            [self.view removeGestureRecognizer:[(KKGestureItemElement *)element gestureRecognizer]];
        }
    }
}

-(void) setView:(UIView *)view {
    
    if(self.view) {
        KKElement * e = self.firstChild;
        while(e) {
            if([e isKindOfClass:[KKGestureItemElement class]]) {
                [self.view removeGestureRecognizer:[(KKGestureItemElement *)e gestureRecognizer]];
            }
            e = e.nextSibling;
        }
    }
    
    [super setView:view];

    if(self.view) {
        KKElement * e = self.firstChild;
        while(e) {
            if([e isKindOfClass:[KKGestureItemElement class]]) {
                [self.view addGestureRecognizer:[(KKGestureItemElement *)e gestureRecognizer]];
            }
            e = e.nextSibling;
        }
    }
    
//    NSLog(@"view inf = %@", self.view);
//    NSLog(@"view %f %f", self.view.frame.size.width, self.view.frame.size.width);
//    NSLog(@"end");
    
}

-(void) changedKey:(NSString *)key {
    [super changedKey:key];
}

@end
