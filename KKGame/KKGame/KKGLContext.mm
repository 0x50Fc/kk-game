//
//  KKGLContext.m
//  KKGame
//
//  Created by hailong11 on 2018/2/5.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#import "KKGLContext.h"
#import "KKWeak.h"
#include "GLSliceMapElement.h"
#include "GLImageElement.h"
#include "GAScene.h"
#include "GAShape.h"
#include "GABody.h"
#include "GAActionMove.h"
#include "require_js.h"
#include "GLAnimation.h"
#include "GLShapeElement.h"
#include "GADocument.h"
#include "GATileMap.h"
#include "GLTextElement.h"
#include "GLMinimapElement.h"
#include "GLViewportElement.h"
#include "GLProgramElement.h"

#import <KKApplication/KKApplication.h>
#import <CommonCrypto/CommonCrypto.h>
#import "KKWebSocket+JSContext.h"

dispatch_queue_t KKGLContextGetDispatchQueue(duk_context *ctx) {
    dispatch_queue_t v = nil;
    duk_push_global_object(ctx);
    duk_get_prop_string(ctx, -1, "__dispatch_queue");
    if(duk_is_pointer(ctx, -1)) {
        v = (__bridge dispatch_queue_t) duk_to_pointer(ctx, -1);
    }
    duk_pop_2(ctx);
    return v;
}

@interface KKGLContext() {
    BOOL _recycle;
}

-(void) setImage:(kk::GL::Image *) image image:(UIImage *) v;

-(void) setImage:(kk::GL::Image *) image error:(NSError *) error;

-(void) ContextGetImage:(kk::GL::Image *) image;

@end

void kk::GL::ContextGetImage(kk::GL::Context * context, kk::GL::Image * image) {
    
    @autoreleasepool{
        
        KKGLContext * ctx = (KKGLContext *) [EAGLContext currentContext];
        
        if([ctx isKindOfClass:[KKGLContext class]]) {
            [ctx ContextGetImage:image];
        }
    
    }
}

static CGFloat KKGLContextDP() {
    static CGFloat dp = 0;
    
    if(dp == 0) {
        CGSize size = [UIScreen mainScreen].bounds.size;
        dp = MIN(size.width ,size.height) * [UIScreen mainScreen].scale / 375.0;
    }
    
    return dp;
}

static NSMutableDictionary * KKGLContextGetStringAttribute(kk::GL::Paint & paint) {
    
    CGFloat dp = KKGLContextDP();
    
    CGFloat fontSize = paint.fontSize * dp;
    
    NSMutableDictionary * attrs = [NSMutableDictionary dictionaryWithCapacity:4];
    UIFont * font = nil;
    
    if(!paint.fontFimlay.empty()) {
        font = [UIFont fontWithName:[NSString stringWithCString:paint.fontFimlay.c_str() encoding:NSUTF8StringEncoding] size:fontSize];
    }
    
    if(font == nil && paint.fontWeight == kk::GL::FontWeightBold) {
        font = [UIFont boldSystemFontOfSize:fontSize];
    }
    
    if(font == nil && paint.fontStyle == kk::GL::FontStyleItalic) {
        font = [UIFont italicSystemFontOfSize:fontSize];
    }
    
    if(font == nil) {
        font = [UIFont systemFontOfSize:fontSize];
    }
    
    attrs[NSFontAttributeName] = font;
    attrs[NSForegroundColorAttributeName] = [UIColor colorWithRed:paint.textColor.r green:paint.textColor.g blue:paint.textColor.b alpha:paint.textColor.a];
    
    if(paint.textShadow.radius >0) {
        NSShadow * shadow = [[NSShadow alloc] init];
        shadow.shadowBlurRadius = paint.textShadow.radius;
        shadow.shadowOffset = CGSizeMake(paint.textShadow.x, paint.textShadow.y);
        shadow.shadowColor = [UIColor colorWithRed:paint.textShadow.color.r green:paint.textShadow.color.g blue:paint.textShadow.color.b alpha:paint.textShadow.color.a];
        attrs[NSShadowAttributeName] = shadow;
    }
    
    if(paint.textStroke.size > 0) {
        attrs[NSStrokeWidthAttributeName] = @(-paint.textStroke.size * dp);
        attrs[NSStrokeColorAttributeName] = [UIColor colorWithRed:paint.textStroke.color.r green:paint.textStroke.color.g blue:paint.textStroke.color.b alpha:paint.textStroke.color.a];
    }
    
    return attrs;
}

void kk::GL::ContextGetStringTexture(kk::GL::Context * context,kk::GL::Texture * texture ,kk::CString text, kk::GL::Paint & paint) {
    
    if(text != nullptr) {
        
        @autoreleasepool{
            
            glBindTexture(GL_TEXTURE_2D, texture->texture());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            
            CGColorSpaceRef rgbSpace = CGColorSpaceCreateDeviceRGB();
            
            NSStringDrawingContext * drawingContext = [[NSStringDrawingContext alloc] init];
            
            NSMutableDictionary * attrs = KKGLContextGetStringAttribute(paint);
            NSAttributedString * string = [[NSAttributedString alloc] initWithString:[NSString stringWithCString:text encoding:NSUTF8StringEncoding] attributes:attrs];
            CGSize maxSize = CGSizeMake(INT32_MAX, INT32_MAX);
            
            if(paint.maxWidth >= 1.0) {
                maxSize.width = paint.maxWidth;
            }
            
            CGRect r = [string boundingRectWithSize:maxSize options:NSStringDrawingUsesLineFragmentOrigin context:drawingContext];
          
            GLsizei width = (GLsizei) ceil(r.size.width) +1;
            GLsizei height =  (GLsizei) ceil(r.size.height) +1;
            
            CGContextRef ctx = CGBitmapContextCreate(NULL, width, height, 8, width * 4, rgbSpace, kCGImageAlphaPremultipliedLast);
            
            UIGraphicsPushContext(ctx);
            
            CGContextSetTextMatrix(ctx , CGAffineTransformIdentity);
            CGContextTranslateCTM(ctx, 0, height);
            CGContextScaleCTM(ctx, 1.0, -1.0);
            
            [string drawWithRect:r options:NSStringDrawingUsesLineFragmentOrigin context:drawingContext];
            
            UIGraphicsPopContext();
            
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, CGBitmapContextGetData(ctx));
            
            CGContextRelease(ctx);
            
            CGColorSpaceRelease(rgbSpace);
            
            CGFloat scale = [UIScreen mainScreen].scale;
            
            texture->setSize(r.size.width / scale, r.size.height / scale);
        
        }
    }
}


static duk_ret_t KKGLContext_getString(duk_context * ctx) {

    kk::CString path = nullptr;
    kk::GL::Context * GLContext = nullptr;
    
    int top = duk_get_top(ctx);
    
    if(top >0  && duk_is_string(ctx, - top)) {
        path = duk_to_string(ctx, -top);
    }
    
    if(path) {
        
        duk_push_current_function(ctx);
        
        duk_push_string(ctx, "_GLContext");
        duk_get_prop(ctx, -2);
        
        if(duk_is_pointer(ctx, -1)) {
            GLContext = (kk::GL::Context *) duk_to_pointer(ctx, -1);
        }
        
        duk_pop_n(ctx, 2);
        
        if(GLContext) {
            
            @autoreleasepool {
                
                NSString * basePath = [NSString stringWithCString:GLContext->basePath() encoding:NSUTF8StringEncoding];
                
                NSString * text = [NSString stringWithContentsOfFile:[basePath stringByAppendingPathComponent:[NSString stringWithCString:path encoding:NSUTF8StringEncoding]] encoding:NSUTF8StringEncoding error:nil];
                
                if(text) {
                    duk_push_string(ctx, [text UTF8String]);
                    return 1;
                }
            }
        }
        
    }
    
    return 0;
}

@implementation KKGLContext

@synthesize http = _http;
@synthesize GLContext = _GLContext;
@synthesize JSContext = _JSContext;

-(void) dealloc {
    
    [_http cancel:self];
    
    if(_GLContext) {
        
        KKWeak * weak = [[KKWeak alloc] initWithObject:_GLContext];
        
        dispatch_async(self.queue,^(){
            kk::Object * v = weak.object;
            if(v) {
                v->release();
            }
        });
        
    }
    
    if(_JSContext) {
        
        KKWeak * weak = [[KKWeak alloc] initWithObject:_JSContext];
        
        dispatch_async(self.queue,^(){
            kk::Object * v = weak.object;
            if(v) {
                v->release();
            }
        });
        
    }
    
}

-(BOOL) isRecycle {
    return _recycle;
}

-(void) recycle {
    
    _recycle = YES;
    
    [_http cancel:self];
    _http = nil;
    
    if(_GLContext) {
        
        KKWeak * weak = [[KKWeak alloc] initWithObject:_GLContext];
        
        dispatch_async(self.queue,^(){
            kk::Object * v = weak.object;
            if(v) {
                v->release();
            }
        });
        
        _GLContext = nullptr;
    }
    
    if(_JSContext) {
        
        KKWeak * weak = [[KKWeak alloc] initWithObject:_JSContext];
        
        dispatch_async(self.queue,^(){
            kk::Object * v = weak.object;
            if(v) {
                v->release();
            }
        });
        
        _JSContext = nullptr;
    }
    
    dispatch_async(self.queue, ^{
        [EAGLContext setCurrentContext:nil];
    });
    
    [EAGLContext setCurrentContext:nil];
}

-(KKHttp *) http {
    if(_http == nil) {
        _http = [KKHttp main];
    }
    return _http;
}

static duk_ret_t KKGLContext_setTimeout(duk_context * ctx) {
    
    int top = duk_get_top(ctx);
    
    if(top > 1 && duk_is_function(ctx, -top) && duk_is_number(ctx, -top + 1)) {
        
        void * heapptr = duk_get_heapptr(ctx, -top);
        int ms = duk_to_int(ctx, -top +1);
        
        duk_push_global_object(ctx);
        
        duk_push_sprintf(ctx, "0x%0x",(long) heapptr);
        duk_push_heapptr(ctx, heapptr);
        duk_put_prop(ctx, -3);
        
        duk_pop(ctx);
        
        kk::script::Context * context = kk::script::GetContext(ctx);
        kk::script::Context ** pContext = (kk::script::Context **) malloc(sizeof(kk::script::Context *));
        
        * pContext = context;
        
        if(context) {
            context->weak((kk::Object **) pContext);
        }
        
        @autoreleasepool {
            
            dispatch_queue_t queue = KKGLContextGetDispatchQueue(ctx);
            
            dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(ms * NSEC_PER_MSEC)), queue, ^{
               
                if(pContext && * pContext) {
                    
                    duk_context * ctx = (* pContext)->jsContext();
                    
                    duk_push_global_object(ctx);
                    
                    duk_push_sprintf(ctx, "0x%x",(long) heapptr);
                    duk_get_prop(ctx, -2);
                    
                    if(duk_is_function(ctx, -1)) {
                        
                        if(DUK_EXEC_SUCCESS != duk_pcall(ctx, 0)) {
                            kk::script::Error(ctx, -1);
                        }
                        
                        duk_pop(ctx);
                        
                        duk_push_sprintf(ctx, "0x%x",(long) heapptr);
                        duk_del_prop(ctx, -2);
                        
                    } else {
                        duk_pop(ctx);
                    }
    
                    
                    duk_pop(ctx);
                    
                    context->unWeak((kk::Object **) &context);
                }
                
                if(pContext && * pContext) {
                    (* pContext)->unWeak((kk::Object **) pContext);
                }
                
                if(pContext) {
                    free(pContext);
                }

            });
            
        }
        
        duk_push_sprintf(ctx, "0x%x",(long) heapptr);
        
        return 1;
    }
    
    return 0;
}

static duk_ret_t KKGLContext_clearTimeout(duk_context * ctx) {
    
    int top = duk_get_top(ctx);
    
    if(top > 0 && duk_is_string(ctx, -top) ) {
        
        const char * iid = duk_to_string(ctx, -top);
        
        duk_push_global_object(ctx);
        
        duk_push_string(ctx, iid);
        duk_del_prop(ctx, -2);
        
        duk_pop(ctx);
    }
    
    return 0;
}

static duk_ret_t KKGLContext_Interval_dealloc(duk_context * ctx) {
    
    duk_push_string(ctx, "source");
    duk_get_prop(ctx, -2);
    
    if(duk_is_pointer(ctx, -1)) {
        @autoreleasepool {
            CFTypeRef v = (CFTypeRef) duk_to_pointer(ctx, -1);
            if(v) {
                dispatch_source_t source = (__bridge dispatch_source_t) v;
                dispatch_source_cancel(source);
                CFRelease(v);
            }
        }
    }
    
    duk_pop(ctx);
    
    return 0;
}

static duk_ret_t KKGLContext_setInterval(duk_context * ctx) {
    
    int top = duk_get_top(ctx);
    
    if(top > 1 && duk_is_function(ctx, -top) && duk_is_number(ctx, -top + 1)) {
        
        @autoreleasepool{
            
            void * heapptr = duk_get_heapptr(ctx, -top);
            int ms = duk_to_int(ctx, -top +1);
            dispatch_queue_t queue = KKGLContextGetDispatchQueue(ctx);
            
            duk_push_global_object(ctx);
            
            duk_push_sprintf(ctx, "0x%x",(long) heapptr);
            
            duk_push_object(ctx);
            
            duk_push_string(ctx, "source");
            
            dispatch_source_t source = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0,queue);
            
            duk_push_pointer(ctx, (void *) CFBridgingRetain(source));
            duk_put_prop(ctx, -3);
            
            duk_push_string(ctx, "func");
            duk_push_heapptr(ctx, heapptr);
            duk_put_prop(ctx, -3);
            
            duk_push_c_function(ctx, KKGLContext_Interval_dealloc, 1);
            duk_set_finalizer(ctx, -2);
            
            duk_put_prop(ctx, -3);
            
            duk_pop(ctx);
            
            kk::script::Context * context = kk::script::GetContext(ctx);
            kk::script::Context ** pContext = (kk::script::Context **) malloc(sizeof(kk::script::Context *));
            
            * pContext = context;
            
            if(context) {
                context->weak((kk::Object **) pContext);
            }
            
            dispatch_source_set_timer(source, dispatch_walltime(NULL, 0), (int64_t)(ms * NSEC_PER_MSEC), 0);
            
            dispatch_source_set_event_handler(source, ^{
                
                if(pContext && * pContext) {
                    
                    duk_context * ctx = (* pContext)->jsContext();
                    
                    duk_push_global_object(ctx);
                    
                    duk_push_sprintf(ctx, "0x%x",(long) heapptr);
                    duk_get_prop(ctx, -2);
                    
                    if(duk_is_object(ctx, -1)) {
                        
                        duk_push_string(ctx, "func");
                        duk_get_prop(ctx, -2);
                        
                        if(duk_is_function(ctx, -1)) {
                            if(DUK_EXEC_SUCCESS != duk_pcall(ctx, 0)) {
                                kk::script::Error(ctx, -1);
                            }
                        }
                        
                        duk_pop(ctx);
                        
                    }
                    
                    duk_pop_2(ctx);
                }
                
            });
            
            dispatch_source_set_cancel_handler(source, ^{
                if(pContext && * pContext) {
                    (* pContext)->unWeak((kk::Object **) pContext);
                }
                if(pContext) {
                    free(pContext);
                }
            });
            
            
            dispatch_resume(source);
            
            duk_push_sprintf(ctx, "0x%x",(long) heapptr);
            
            return 1;
            
        }
        
    }
    
    return 0;
}

static duk_ret_t KKGLContext_clearInterval(duk_context * ctx) {
    
    int top = duk_get_top(ctx);
    
    if(top > 0 && duk_is_string(ctx, -top) ) {
        
        const char * iid = duk_to_string(ctx, -top);
        
        duk_push_global_object(ctx);
        
        duk_push_string(ctx, iid);
        duk_del_prop(ctx, -2);
        
        duk_pop(ctx);
        
    }
    
    return 0;
}

-(instancetype) initWithAPI:(EAGLRenderingAPI)api {
    if((self = [super initWithAPI:api])) {
        
        self.multiThreaded = YES;
        
        _queue = dispatch_queue_create("KKGLContext", nil);
        _GLContext = new kk::GL::Context();
        _GLContext->retain();
        _JSContext = new kk::script::Context();
        _JSContext->retain();
        
        {
            duk_context * ctx = _JSContext->jsContext();
            
            kk::script::SetPrototype(ctx, &kk::ElementEvent::ScriptClass);
            kk::script::SetPrototype(ctx, &kk::GA::Context::ScriptClass);
            kk::script::SetPrototype(ctx, &kk::GA::Scene::ScriptClass);
            kk::script::SetPrototype(ctx, &kk::GA::Shape::ScriptClass);
            kk::script::SetPrototype(ctx, &kk::GA::Body::ScriptClass);
            kk::script::SetPrototype(ctx, &kk::GA::Action::ScriptClass);
            kk::script::SetPrototype(ctx, &kk::GA::ActionMove::ScriptClass);
            kk::script::SetPrototype(ctx, &kk::GA::Document::ScriptClass);
            kk::script::SetPrototype(ctx, &kk::GA::TileMap::ScriptClass);
            kk::script::SetPrototype(ctx, &kk::GL::SliceMapElement::ScriptClass);
            kk::script::SetPrototype(ctx, &kk::GL::ImageElement::ScriptClass);
            kk::script::SetPrototype(ctx, &kk::GL::ShapeElement::ScriptClass);
            kk::script::SetPrototype(ctx, &kk::GL::Animation::ScriptClass);
            kk::script::SetPrototype(ctx, &kk::GL::AnimationItem::ScriptClass);
            kk::script::SetPrototype(ctx, &kk::GL::TextElement::ScriptClass);
            kk::script::SetPrototype(ctx, &kk::GL::MinimapElement::ScriptClass);
            kk::script::SetPrototype(ctx, &kk::GL::ViewportElement::ScriptClass);
            kk::script::SetPrototype(ctx, &kk::GL::ProgramElement::ScriptClass);
            
            [KKWebSocket openlib:ctx];
            
            duk_push_global_object(ctx);
            
            duk_push_pointer(ctx, (__bridge void *) _queue);
            duk_put_prop_string(ctx, -2, "__dispatch_queue");
            
            duk_push_string(ctx, "setTimeout");
            duk_push_c_function(ctx, KKGLContext_setTimeout, 2);
            duk_put_prop(ctx, -3);
            
            duk_push_string(ctx, "setInterval");
            duk_push_c_function(ctx, KKGLContext_setInterval, 2);
            duk_put_prop(ctx, -3);
            
            duk_push_string(ctx, "clearTimeout");
            duk_push_c_function(ctx, KKGLContext_clearTimeout, 1);
            duk_put_prop(ctx, -3);
            
            duk_push_string(ctx, "clearInterval");
            duk_push_c_function(ctx, KKGLContext_clearInterval, 1);
            duk_put_prop(ctx, -3);
            
            duk_push_string(ctx,"context");
            kk::script::PushObject(ctx, _GLContext);
            duk_put_prop(ctx, -3);
            
            duk_pop(ctx);
        }
        
        {
            duk_context * ctx = _JSContext->jsContext();
            
            duk_push_global_object(ctx);
            
            duk_push_string(ctx, "kk");
            duk_push_object(ctx);
            
            duk_push_string(ctx, "kernel");
            duk_push_number(ctx, KKApplicationKernel);
            duk_put_prop(ctx, -3);
            
            duk_push_string(ctx, "platform");
            duk_push_string(ctx, "ios");
            duk_put_prop(ctx, -3);
            
            duk_push_string(ctx, "getString");
            duk_push_c_function(ctx, KKGLContext_getString, 1);
            
            duk_push_string(ctx, "_GLContext");
            duk_push_pointer(ctx, _GLContext);
            duk_put_prop(ctx, -3);
            
            duk_put_prop(ctx, -3);
            
            
            {
                NSBundle * main = [NSBundle mainBundle];
                
                duk_push_string(ctx, "app");
                duk_push_object(ctx);
                
                duk_push_string(ctx, "id");
                duk_push_string(ctx, [[[main infoDictionary] valueForKey:@"CFBundleIdentifier"] UTF8String]);
                duk_put_prop(ctx, -3);
                
                duk_push_string(ctx, "version");
                duk_push_string(ctx, [[[main infoDictionary] valueForKey:@"CFBundleShortVersionString"] UTF8String]);
                duk_put_prop(ctx, -3);
                
                duk_push_string(ctx, "build");
                duk_push_string(ctx, [[[main infoDictionary] valueForKey:@"CFBundleVersion"] UTF8String]);
                duk_put_prop(ctx, -3);
                
                duk_push_string(ctx, "name");
                duk_push_string(ctx, [[[main infoDictionary] valueForKey:@"CFBundleDisplayName"] UTF8String]);
                duk_put_prop(ctx, -3);
                
                duk_push_string(ctx, "lang");
                duk_push_string(ctx, [[[NSLocale currentLocale] localeIdentifier] UTF8String]);
                duk_put_prop(ctx, -3);
                
                duk_put_prop(ctx, -3);
                
            }
            
            {
                
                duk_push_string(ctx, "device");
                duk_push_object(ctx);
                
                duk_push_string(ctx, "id");
                
                UIDevice * device = [UIDevice currentDevice];
                
                CC_MD5_CTX m;
                
                CC_MD5_Init(&m);
                
                NSData * data = [[[device identifierForVendor] UUIDString] dataUsingEncoding:NSUTF8StringEncoding];
                
                CC_MD5_Update(&m, [data bytes], (CC_LONG) [data length]);
                
                unsigned char md[16];
                
                CC_MD5_Final(md, &m);
                
                duk_push_string(ctx, [[NSString stringWithFormat:@"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x"
                                      ,md[0],md[1],md[2],md[3],md[4],md[5],md[6],md[7]
                                       ,md[8],md[9],md[10],md[11],md[12],md[13],md[14],md[15]] UTF8String]);
                duk_put_prop(ctx, -3);
                
                duk_push_string(ctx, "systemName");
                duk_push_string(ctx, [[device systemName] UTF8String]);
                duk_put_prop(ctx, -3);
                
                duk_push_string(ctx, "systemVersion");
                duk_push_string(ctx, [[device systemVersion] UTF8String]);
                duk_put_prop(ctx, -3);
                
                duk_push_string(ctx, "model");
                duk_push_string(ctx, [[device model] UTF8String]);
                duk_put_prop(ctx, -3);
                
                duk_push_string(ctx, "name");
                duk_push_string(ctx, [[device name] UTF8String]);
                duk_put_prop(ctx, -3);
                
                duk_put_prop(ctx, -3);
                
            }
            
            duk_put_prop(ctx, -3);
            
            duk_push_string(ctx, "app");
            duk_push_object(ctx);
            duk_put_prop(ctx,-3);
            
            duk_pop(ctx);
            
            duk_eval_lstring_noresult(ctx, (char *)require_js, sizeof(require_js));
        }
        
        [EAGLContext setCurrentContext:self];
        
        _GLContext->init();
        
    }
    return self;
}

-(void) setImage:(kk::GL::Image *) image image:(UIImage *) v {
    
    CGColorSpaceRef rgbSpace = CGColorSpaceCreateDeviceRGB();
    
    CGImageRef imageRef = v.CGImage;
    
    GLsizei width = (GLsizei) CGImageGetWidth(imageRef);
    GLsizei height = (GLsizei) CGImageGetHeight(imageRef);
    
    CGContextRef ctx = CGBitmapContextCreate(NULL, width, height, 8, width * 4, rgbSpace, kCGImageAlphaPremultipliedLast);
    
    CGContextDrawImage(ctx, CGRectMake(0, 0, width, height), imageRef);
    
    [EAGLContext setCurrentContext:self];
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, image->texture());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, CGBitmapContextGetData(ctx));
    
    CGContextRelease(ctx);
    
    CGColorSpaceRelease(rgbSpace);
    
    image->setSize(width, height);
    
    image->setStatus(kk::GL::ImageStatusLoaded);

}

-(void) setImage:(kk::GL::Image *) image error:(NSError *) error {
    image->setError([[error localizedDescription] UTF8String]);
    image->setStatus(kk::GL::ImageStatusFail);
    NSLog(@"Image: %s Error: %@",image->uri(),error);
}

-(void) ContextGetImage:(kk::GL::Image *) image {
    
    if(image->status() != kk::GL::ImageStatusNone) {
        return;
    }
    
    NSString * uri = [NSString stringWithCString:image->uri() encoding:NSUTF8StringEncoding];
    
    if([uri hasPrefix:@"http://"] || [uri hasPrefix:@"https://"]) {
        
        KKWeak * oimage = [[KKWeak alloc] initWithObject:image];
        
        image->setStatus(kk::GL::ImageStatusLoading);
        
        KKHttpOptions * options = [[KKHttpOptions alloc] initWithURL:uri];
        options.type = KKHttpOptionsTypeImage;
        options.method = KKHttpOptionsGET;
        options.onload = ^(id data, NSError *error, id weakObject) {
            if(weakObject == nil) {
                return;
            }
            if(oimage.object == nil) {
                return;
            }
            if([data isKindOfClass:[UIImage class]]) {
                [(KKGLContext *) weakObject setImage:(kk::GL::Image *) oimage.object image:(UIImage *) data];
            } else if(error) {
                [(KKGLContext *) weakObject setImage:(kk::GL::Image *) oimage.object error:error];
            } else {
                [(KKGLContext *) weakObject setImage:(kk::GL::Image *) oimage.object error:[NSError errorWithDomain:@"KKGLContext" code:900 userInfo:[NSDictionary dictionaryWithObject:@"图片格式错误!" forKey:NSLocalizedDescriptionKey]]];
            }
        };
        options.onfail = ^(NSError *error, id weakObject) {
            if(weakObject == nil) {
                return;
            }
            if(oimage.object == nil) {
                return;
            }
            if(error) {
                [(KKGLContext *) weakObject setImage:(kk::GL::Image *) oimage.object error:error];
            } else {
                [(KKGLContext *) weakObject setImage:(kk::GL::Image *) oimage.object error:[NSError errorWithDomain:@"KKGLContext" code:800 userInfo:@{NSLocalizedDescriptionKey:@"网络加载错误!",@"url":uri}]];
            }
        };
        
        [self.http send:options weakObject:self];
        
    } else {
        
        KKWeak * oimage = [[KKWeak alloc] initWithObject:image];
        __weak KKGLContext * weakObject = self;
        
        NSString * path = [[NSString stringWithCString:_GLContext->basePath() encoding:NSUTF8StringEncoding] stringByAppendingPathComponent:uri];
        
        image->setStatus(kk::GL::ImageStatusLoading);
        
        dispatch_queue_t queue = self.queue;
        
        dispatch_async(KKHttpIODispatchQueue(), ^{
            UIImage * v = [UIImage imageWithContentsOfFile:path];
            dispatch_async(queue, ^{
                if(oimage.object) {
                    if(v) {
                        [(KKGLContext *) weakObject setImage:(kk::GL::Image *) oimage.object image:v];
                    } else {
                        [(KKGLContext *) weakObject setImage:(kk::GL::Image *) oimage.object error:[NSError errorWithDomain:@"KKGLContext" code:900 userInfo:@{NSLocalizedDescriptionKey:@"网络加载错误!",@"url":uri}]];
                    }
                }
            });
        });
    
    }
}

+(void) JSContextPushObject:(id) object ctx:(duk_context * )ctx {
    
    if(object == nil) {
        duk_push_null(ctx);
        return;
    }
    
    if([object isKindOfClass:[NSString class]]) {
        duk_push_string(ctx, [object UTF8String]);
        return;
    }
    
    if([object isKindOfClass:[NSNumber class]]) {
        
        if(strcmp(@encode(BOOL),[object objCType]) == 0) {
            duk_push_boolean(ctx, [object boolValue]);
        } else {
            duk_push_number(ctx, [object doubleValue]);
        }
        return;
    }
    
    if([object isKindOfClass:[NSArray class]]) {
        
        duk_push_array(ctx);
        
        duk_idx_t i = 0;
        
        for(id value in object ){
            
            duk_push_int(ctx, i);
            [self JSContextPushObject:value ctx:ctx];
            duk_put_prop(ctx,-3);
            
        }
        
        return ;
    }
    
    if([object isKindOfClass:[NSDictionary class]]) {
        
        duk_push_object(ctx);
        
        NSEnumerator * keyEnum = [object keyEnumerator];
        NSString * key;
        
        while((key = [keyEnum nextObject])) {
            duk_push_string(ctx, [key UTF8String]);
            [self JSContextPushObject:[object valueForKey:key] ctx:ctx];
            duk_put_prop(ctx, -3);
        }
        
        return;
    }
    
    duk_push_null(ctx);
    
    return;
}

+(id) JSContextToObject:(duk_idx_t) idx ctx:(duk_context * )ctx {
    
    if(duk_is_array(ctx, idx)) {
        
        NSMutableArray * vs = [NSMutableArray arrayWithCapacity:4];
        
        duk_size_t n = duk_get_length(ctx, idx);
        
        for(duk_uarridx_t i =0 ;i<n;i++) {
            duk_get_prop_index(ctx, idx, i);
            id value = [self JSContextToObject: -1 ctx:ctx];
            if(value) {
                [vs addObject:value];
            }
            duk_pop(ctx);
        }
        
        return vs;
    } else if(duk_is_object(ctx, idx)) {
        
        NSMutableDictionary * vs = [NSMutableDictionary dictionaryWithCapacity:4];
        
        duk_enum(ctx, idx, DUK_ENUM_INCLUDE_SYMBOLS);
        
        while(duk_next(ctx, -1, 1)) {
            NSString * key = [NSString stringWithCString:duk_to_string(ctx,-2) encoding:NSUTF8StringEncoding];
            id value = [self JSContextToObject: -1 ctx:ctx];
            if(key && value) {
                [vs setValue:value forKey:key];
            }
            duk_pop_2(ctx);
        }
        
        duk_pop(ctx);
        
        return vs;
        
    } else if(duk_is_boolean(ctx, idx)) {
        return [NSNumber numberWithBool:duk_to_boolean(ctx, idx)];
    } else if(duk_is_string(ctx, idx)) {
        return [NSString stringWithCString:duk_to_string(ctx,idx) encoding:NSUTF8StringEncoding];
    } else if(duk_is_number(ctx, idx)) {
        return [NSNumber numberWithDouble:duk_to_number(ctx, idx)];
    }
    
    return nil;
}


@end
