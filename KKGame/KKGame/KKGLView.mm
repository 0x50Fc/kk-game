//
//  KKGLView.m
//  KKGame
//
//  Created by zhanghailong on 2018/2/5.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#import "KKGLView.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include "GLContext.h"

@interface KKGLView() {
    struct {
        GLuint frame;
        GLuint render;
        GLuint depth;
    } _data;
    CGSize _resize;
}

@end

@implementation KKGLView

@synthesize width = _width;
@synthesize height = _height;

+(Class) layerClass {
    return [CAEAGLLayer class];
}

-(void) installGLContext:(KKGLContext *) GLContext {
    
    if(GLContext.isRecycle) {
        return;
    }
    
    [EAGLContext setCurrentContext:GLContext];
    glGenFramebuffers(1, &_data.frame);
    glGenRenderbuffers(1, &_data.render);
    glGenRenderbuffers(1, &_data.depth);
}

-(void) uninstallGLContext:(KKGLContext *) GLContext {
    
    if(GLContext.isRecycle) {
        return;
    }

    [EAGLContext setCurrentContext:GLContext];
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
    _resize = CGSizeZero;
}

-(void) resizeGLContext:(KKGLContext *) GLContext {
    
    if(GLContext.isRecycle) {
        return;
    }

    if(CGSizeEqualToSize(_resize, self.bounds.size)) {
        return;
    }
    
    _resize = self.bounds.size;
    
    [EAGLContext setCurrentContext:GLContext];
    
    self.layer.contentsScale = [UIScreen mainScreen].scale;
    
    glBindRenderbuffer(GL_RENDERBUFFER, _data.render);
    
    [GLContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer*) self.layer];
    
    
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
    
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE , GL_ONE_MINUS_SRC_ALPHA);
    
    kk::GL::Float scale = self.layer.contentsScale;
    
    dispatch_async(GLContext.queue, ^{
        
        kk::GL::Float width =  _width * 2.0f / scale;
        kk::GL::Float height =  _height * 2.0f / scale;
        kk::GL::Float dp = 750.0f / MIN(width,height) ;
        
        kk::GL::ContextState & s = GLContext.GLContext->state();
        
        GLContext.GLContext->setViewport(dp * width, dp * height);
        
        s.projection = glm::scale(kk::GL::mat4(1),kk::GL::vec3(2.0f / (dp *width),-2.0f / (dp * height),-0.0001));
        
    });
    
}

-(void) displayGLContext:(KKGLContext *) GLContext element:(kk::Element *) element {
    
    if(GLContext.isRecycle) {
        return;
    }
    
    [EAGLContext setCurrentContext:GLContext];
    
    CGFloat r = 0.0,g = 0.0,b = 0.0,a = 0.0;
    
    glClearColor(r, g, b, a);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    GLContext.GLContext->tick();
    GLContext.GLContext->exec(element);
    GLContext.GLContext->draw(element);
    [GLContext presentRenderbuffer:GL_RENDERBUFFER]; // 渲染到设备
    
}

-(BOOL) isVisible {
    return self.window != nil;
}

-(void) didMoveToWindow {
    [super didMoveToWindow];
    if(_onVisible) {
        _onVisible(self.window != nil);
    }
}

@end
