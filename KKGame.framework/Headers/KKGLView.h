//
//  KKGLView.h
//  KKGame
//
//  Created by zhanghailong on 2018/2/5.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <KKGame/KKGLContext.h>
@class KKGLView;

@protocol KKGLViewDelegate<NSObject>

@optional

- (void) KKGLView:(KKGLView *) view displayGLContext:(EAGLContext *) GLContext;

- (void) KKGLView:(KKGLView *) view resizeGLContext:(EAGLContext *) GLContext;

- (void) KKGLView:(KKGLView *) view touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event;
- (void) KKGLView:(KKGLView *) view touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event;
- (void) KKGLView:(KKGLView *) view touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event;
- (void) KKGLView:(KKGLView *) view touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event;

@end

@interface KKGLView : UIView

@property(nonatomic,assign,readonly) GLsizei width;
@property(nonatomic,assign,readonly) GLsizei height;
@property(atomic,weak) id<KKGLViewDelegate> delegate;
@property(nonatomic,strong,readonly) dispatch_queue_t queue;
@property(nonatomic,strong,readonly) KKGLContext * GLContext;

-(void) recycle;

@end
