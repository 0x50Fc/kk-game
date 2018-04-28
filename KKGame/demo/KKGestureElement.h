//
//  KKGestureElement.h
//  demo
//
//  Created by zuowu on 2018/3/22.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#import <KKView/KKViewElement.h>

@interface KKGestureItemElement : KKElement
{
    NSInteger _touchesRequired;           //触摸点数量
    NSInteger _tapsRequired;              //点击数量
    CFTimeInterval _longPressDuration;    //长按间隔
    
    NSInteger _panMinTouches;  //滑动最小触摸数
    NSInteger _panMaxTouches;  //滑动最大触摸数
    
}

@property(nonatomic,readonly,strong) UIGestureRecognizer * gestureRecognizer;

@end

@interface KKGestureElement : KKViewElement
{
    UIView * _gestureView;
//    //单击
//    UITapGestureRecognizer * _singleTapRecognizer;
//    //双击
//    UITapGestureRecognizer * _doubleTapRecognizer;
//    //长按
//    UILongPressGestureRecognizer * _longPressRecognizer;
//    //平移
//    UIPanGestureRecognizer * _moveingRecognizer;
    
}

@end
