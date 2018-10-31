//
//  KKGLView.m
//  KKGame
//
//  Created by zhanghailong on 2018/2/5.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#import "KKGLView.h"
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#include <OpenGLES/EAGL.h>

@interface KKGLView() {
    struct {
        GLuint frame;
        GLuint render;
        GLuint depth;
    } _data;
    CGSize _resize;
    dispatch_source_t _source;
    BOOL _visible;
}

@end

@implementation KKGLView

@synthesize delegate = _delegate;
@synthesize width = _width;
@synthesize height = _height;
@synthesize GLContext = _GLContext;

+(Class) layerClass {
    return [CAEAGLLayer class];
}

-(instancetype) initWithFrame:(CGRect) frame {
    if((self = [super initWithFrame:frame])) {
        
        _GLContext = [[KKGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
       
        [EAGLContext setCurrentContext:_GLContext];
        
        glGenFramebuffers(1, &_data.frame);
        glGenRenderbuffers(1, &_data.render);
        glGenRenderbuffers(1, &_data.depth);
        
        [EAGLContext setCurrentContext:nil];
        
        _source = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0,_GLContext.queue);
        
        int64_t frames = 60;
        dispatch_source_set_timer(_source, dispatch_walltime(NULL, 0), (int64_t)(NSEC_PER_SEC / frames), 0);
        
        __block dispatch_source_t s = _source;
        __weak KKGLView * view = self;

        dispatch_source_set_event_handler(_source, ^{
            @synchronized (view) {
                [view displayGLContext];
            }
        });
        
        dispatch_source_set_cancel_handler(_source, ^{
            s = nil;
        });
        
        self.layer.contentsScale = [[UIScreen mainScreen] scale];
        self.layer.opaque = NO;
        
        [self resizeGLContext];
        
        dispatch_resume(_source);
    }
    return self;
}

-(dispatch_queue_t) queue {
    return [_GLContext queue];
}

-(void) dealloc {

}

-(void) setBounds:(CGRect)bounds {
    [super setBounds:bounds];
    @synchronized (self) {
        [self resizeGLContext];
    }
}

-(void) setFrame:(CGRect)frame {
    [super setFrame:frame];
    @synchronized (self) {
        [self resizeGLContext];
    }
}

-(void) resizeGLContext {
    
    if(CGSizeEqualToSize(_resize, self.bounds.size)) {
        return;
    }
    
    _resize = self.bounds.size;
    
    [EAGLContext setCurrentContext:_GLContext];
    
    self.layer.contentsScale = [UIScreen mainScreen].scale;
    
    glBindRenderbuffer(GL_RENDERBUFFER, _data.render);
    
    [_GLContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer*) self.layer];
    
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &_width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &_height);
    
    glBindRenderbuffer(GL_RENDERBUFFER, _data.depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16
                          , _width, _height);
    
    glBindFramebuffer(GL_FRAMEBUFFER, _data.frame);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _data.render);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _data.depth);
    
    glViewport(0,0,_width,_height);
    
    glBindRenderbuffer(GL_RENDERBUFFER, _data.render);
    
    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE , GL_ONE_MINUS_SRC_ALPHA);
    
    if([_delegate respondsToSelector:@selector(KKGLView:resizeGLContext:)]) {
        [_delegate KKGLView:self resizeGLContext:_GLContext];
    }
    
    [EAGLContext setCurrentContext:nil];
    
}

-(void) displayGLContext{
    
    if(!_visible) {
        return;
    }
    
    [EAGLContext setCurrentContext:_GLContext];
    
    glClearColor(0, 0, 0, 0);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if([_delegate respondsToSelector:@selector(KKGLView:displayGLContext:)]) {
        [_delegate KKGLView:self displayGLContext:_GLContext];
    }

    glFlush();
    
    if(_visible) {
        [_GLContext presentRenderbuffer:GL_RENDERBUFFER]; // 渲染到设备
    }
    
    [EAGLContext setCurrentContext:nil];
}


- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    
    if([_delegate respondsToSelector:@selector(KKGLView:touchesBegan:withEvent:)]) {
        [_delegate KKGLView:self touchesBegan:touches withEvent:event];
    }
    
    [super touchesBegan:touches withEvent:event];
}

- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
 
    if([_delegate respondsToSelector:@selector(KKGLView:touchesMoved:withEvent:)]) {
        [_delegate KKGLView:self touchesMoved:touches withEvent:event];
    }
    
    [super touchesMoved:touches withEvent:event];
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    
    if([_delegate respondsToSelector:@selector(KKGLView:touchesEnded:withEvent:)]) {
        [_delegate KKGLView:self touchesEnded:touches withEvent:event];
    }
    
    [super touchesEnded:touches withEvent:event];
}

- (void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
 
    if([_delegate respondsToSelector:@selector(KKGLView:touchesCancelled:withEvent:)]) {
        [_delegate KKGLView:self touchesCancelled:touches withEvent:event];
    }
    
    [super touchesCancelled:touches withEvent:event];
}

-(void) recycleGLContext {
    
    [EAGLContext setCurrentContext:_GLContext];
    
    if(_data.frame) {
        glDeleteFramebuffers(1,&_data.frame);
        _data.frame = 0;
    }
    
    if(_data.render) {
        glDeleteFramebuffers(1,&_data.render);
        _data.render = 0;
    }
    
    if(_data.depth) {
        glDeleteFramebuffers(1,&_data.depth);
        _data.depth = 0;
    }
    
    [EAGLContext setCurrentContext:nil];
    
}

-(void) recycle {
    
    if(_source) {
        dispatch_source_cancel(_source);
        _source = nil;
    }
    
    if(_GLContext != nil) {

        dispatch_sync(_GLContext.queue, ^{
            [self recycleGLContext];
        });
        
    }
    
    [_GLContext recycle];
}

-(void) didMoveToWindow {
    [super didMoveToWindow];
    _visible = self.window != nil;
}

@end
