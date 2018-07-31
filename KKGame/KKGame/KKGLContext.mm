//
//  KKGLContext.m
//  KKGame
//
//  Created by zhanghailong on 2018/2/5.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include "kk-object.h"
#import "KKGLContext.h"
#import "KKWeak.h"
#include "GLSliceMapElement.h"
#include "GLImageElement.h"
#include "GAScene.h"
#include "GAShape.h"
#include "GABody.h"
#include "GAActionWalk.h"
#include "require_js.h"
#include "GLAnimation.h"
#include "GLShapeElement.h"
#include "GADocument.h"
#include "GATileMap.h"
#include "GLTextElement.h"
#include "GLMinimapElement.h"
#include "GLViewportElement.h"
#include "GLTileMapElement.h"
#include "GLSpineElement.h"
#include "GLMetaElement.h"

#import <KKApplication/KKApplication.h>
#import <CommonCrypto/CommonCrypto.h>

@interface KKGLContext() {
    BOOL _recycle;
}

@end

static void KKGLContextSetImageError(kk::GL::Image * image, NSError * error) {
    image->setError([[error localizedDescription] UTF8String]);
    image->setStatus(kk::GL::ImageStatusFail);
    NSLog(@"Image: %s Error: %@",image->uri(),error);
}

static void KKGLContextSetImage(kk::GL::Image * image,UIImage * v) {
    
    CGColorSpaceRef rgbSpace = CGColorSpaceCreateDeviceRGB();
    
    CGImageRef imageRef = v.CGImage;
    
    GLsizei width = (GLsizei) CGImageGetWidth(imageRef);
    GLsizei height = (GLsizei) CGImageGetHeight(imageRef);
    
    CGContextRef ctx = CGBitmapContextCreate(NULL, width, height, 8, width * 4, rgbSpace, kCGImageAlphaPremultipliedLast);
    
    CGContextDrawImage(ctx, CGRectMake(0, 0, width, height), imageRef);
    
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

void kk::GL::ContextGetImage(kk::GL::Context * context, kk::GL::Image * image) {
    
    @autoreleasepool{
        
        if(image->status() != kk::GL::ImageStatusNone) {
            return;
        }
        
        KKGLContext * GLContext = (KKGLContext *) [EAGLContext currentContext];
        
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
                    [EAGLContext setCurrentContext:GLContext];
                    KKGLContextSetImage((kk::GL::Image *) oimage.object, (UIImage *) data);
                } else if(error) {
                    KKGLContextSetImageError((kk::GL::Image *) oimage.object,error);
                } else {
                    KKGLContextSetImageError((kk::GL::Image *) oimage.object,[NSError errorWithDomain:@"KKGLContext" code:900 userInfo:[NSDictionary dictionaryWithObject:@"图片格式错误!" forKey:NSLocalizedDescriptionKey]]);
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
                    KKGLContextSetImageError((kk::GL::Image *) oimage.object,error);
                } else {
                    KKGLContextSetImageError((kk::GL::Image *) oimage.object,[NSError errorWithDomain:@"KKGLContext" code:800 userInfo:@{NSLocalizedDescriptionKey:@"网络加载错误!",@"url":uri}]);
                }
            };
            
            [GLContext.http send:options weakObject:GLContext];
            
        } else {
            
            KKWeak * oimage = [[KKWeak alloc] initWithObject:image];

            NSString * path = nil;
            
            if([uri hasPrefix:@"/"]) {
                path = uri;
            } else {
                path = [GLContext.basePath stringByAppendingPathComponent:uri];
            }
            
            image->setStatus(kk::GL::ImageStatusLoading);
            
            dispatch_queue_t queue = GLContext.queue;
            
            dispatch_async(KKHttpIODispatchQueue(), ^{
                UIImage * v = [UIImage imageWithContentsOfFile:path];
                dispatch_async(queue, ^{
                    if(oimage.object) {
                        if(v) {
                            [EAGLContext setCurrentContext:GLContext];
                            KKGLContextSetImage((kk::GL::Image *) oimage.object, v);
                        } else {
                            KKGLContextSetImageError((kk::GL::Image *) oimage.object,[NSError errorWithDomain:@"KKGLContext" code:900 userInfo:@{NSLocalizedDescriptionKey:@"网络加载错误!",@"url":uri}]);
                        }
                    }
                });
            });
            
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



@implementation KKGLContext

@synthesize http = _http;

-(void) recycle {
    [_http cancel:self];
}

-(KKHttp *) http {
    if(_http == nil) {
        _http = [KKHttp main];
    }
    return _http;
}

-(instancetype) initWithAPI:(EAGLRenderingAPI)api {
    if((self = [super initWithAPI:api])) {
        self.multiThreaded = YES;
        _queue = dispatch_queue_create("KKGLContext", DISPATCH_QUEUE_SERIAL);
    }
    return self;
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
