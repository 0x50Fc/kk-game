//
//  KKGLView.h
//  KKGame
//
//  Created by hailong11 on 2018/2/5.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <KKGame/KKGLContext.h>

#ifdef __cplusplus

namespace kk {
    
    class Element;
    
}

typedef kk::Element * KKElementRef;

#else

typedef void * KKElementRef;

#endif

@interface KKGLView : UIView

@property(nonatomic,assign,readonly) GLsizei width;
@property(nonatomic,assign,readonly) GLsizei height;
@property(nonatomic,strong) void (^onVisible)(BOOL visible);
@property(nonatomic,assign,readonly,getter=isVisible) BOOL visible;

-(void) installGLContext:(KKGLContext *) GLContext;

-(void) uninstallGLContext:(KKGLContext *) GLContext;

-(void) resizeGLContext:(KKGLContext *) GLContext;

-(void) displayGLContext:(KKGLContext *) GLContext element:(KKElementRef) element;

@end
