//
//  KKGLViewElement.m
//  KKGame
//
//  Created by zhanghailong on 2018/2/5.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#import "KKGLViewElement.h"
#import "KKGLView.h"

#include "kk-config.h"
#include <KKObject/KKObject.h>
#include "GLContext.h"
#include "kk-app.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

@interface KKGLViewElement()<KKGLViewDelegate> {
    BOOL _loaded;
    NSLock * _lock;
    struct event_base * _base;
    struct evdns_base * _dns;
    kk::DispatchQueue * _queue;
}

@property(nonatomic,assign,readonly) kk::script::Context * jsContext;
@property(nonatomic,assign,readonly) kk::Application * app;

-(void) _emit:(NSString *)name event:(KKEvent *)event;

@end

static void KKGLViewElement_event_cb (kk::EventEmitter * emitter,kk::CString name,kk::Event * event,BK_DEF_ARG) {
    
    @autoreleasepool{
        
        BK_GET_VAR(element, CFTypeRef)
        
        kk::ElementEvent * e = dynamic_cast<kk::ElementEvent *>(event);
        
        if(e) {
            
            kk::script::Object * data = e->data.as<kk::script::Object>();
            
            if(data && data->heapptr() && data->jsContext() ) {
                
                duk_context * ctx = data->jsContext();
                
                NSString * n = [NSString stringWithCString:name + 4 encoding:NSUTF8StringEncoding];
                
                duk_push_heapptr(ctx, data->heapptr());
                
                id v = [KKGLContext JSContextToObject: -1 ctx:ctx];
                
                duk_pop(ctx);
                
                __strong KKGLViewElement * elem = (__bridge KKGLViewElement *) element;
                
                dispatch_async(dispatch_get_main_queue(), ^{
                    
                    KKElementEvent * ev = [[KKElementEvent alloc] initWithElement:elem];
                    ev.data = v;
                    [elem _emit:n event:ev];
                    
                });
                
            }
        }
    }
    
}

@implementation KKGLViewElement

@synthesize app = _app;

+(void) initialize {
    
    [KKViewContext setDefaultElementClass:[KKGLViewElement class] name:@"gl:view"];
}

-(void) dealloc {
    
    [_lock lock];
    
    if(_queue != nullptr) {
        _queue->release();
    }
    
    if(_jsContext != nullptr) {
        _jsContext->release();
    }
    
    if(_app != nullptr) {
        _app->release();
    }
    
    if(_dns != nullptr) {
        evdns_base_free(_dns, 0);
    }
    
    if(_base != nullptr) {
        event_base_free(_base);
    }
    
    [_lock unlock];
    
}

-(instancetype) init {
    if((self = [super init])) {
        _lock = [[NSLock alloc] init];
        [self setAttrs:@{@"keepalive" : @"true"}];
    }
    return self;
}

-(Class) viewClass {
    return [KKGLView class];
}

-(NSString *) reuse {
    return nil;
}

static void KKGLViewElement_EventOnCreateContext (duk_context * ctx,kk::DispatchQueue * queue, duk_context * newContext) {
  
    kk::Application * app = nullptr;
    
    duk_get_global_string(ctx, "__app");
    
    if(duk_is_pointer(ctx, -1)) {
        app = (kk::Application *) duk_to_pointer(ctx, -1);
    }
    
    duk_pop(ctx);
    
    if(app != nullptr) {
        app->installContext(newContext);
        duk_push_pointer(newContext, app);
        duk_put_global_string(newContext, "__app");
    }
    
    event_base * base = kk::ev_base(newContext);
    evdns_base * dns = kk::ev_dns(newContext);
    
    {
        
        kk::script::SetPrototype(newContext, &kk::WebSocket::ScriptClass);
        kk::script::SetPrototype(newContext, &kk::Http::ScriptClass);
        
        kk::Strong v = new kk::Http(base,dns);
        
        kk::script::PushObject(newContext, v.get());
        duk_put_global_string(newContext, "http");
        
    }
}


-(void) install {
    
    KKGLView * view = (KKGLView *) self.view;
    
    if(view == nil) {
        return;
    }
    
    NSString * path = [self get:@"path"];
    
    if(path == nil) {
        return;
    }
    
    NSString * basePath = [self.viewContext basePath];
    
    if(basePath == nil) {
        basePath = @"";
    }
    
    path = [basePath stringByAppendingPathComponent:path];
    
    view.GLContext.basePath = path;
    
    [_lock lock];
    
    [EAGLContext setCurrentContext:view.GLContext];
    
    if(_dns != nullptr) {
        evdns_base_free(_dns, 0);
        _dns = nullptr;
    }
    
    if(_base == nullptr) {
        _base = event_base_new();
    }
    
    if(_dns == nullptr) {
        _dns = evdns_base_new(_base, EVDNS_BASE_INITIALIZE_NAMESERVERS);
    }
    
    if(_queue == nullptr) {
        _queue = new kk::DispatchQueue("KKGLViewElement",_base);
        _queue->retain();
    }
    
    if(_jsContext == nullptr) {
        _jsContext = new kk::script::Context();
        _jsContext->retain();
        
        duk_context * ctx = _jsContext->jsContext();
        
        kk::ev_openlibs(ctx, _base,_dns);
        kk::wk_openlibs(ctx, _queue,KKGLViewElement_EventOnCreateContext);
        
        {
            kk::script::SetPrototype(ctx, &kk::WebSocket::ScriptClass);
            kk::script::SetPrototype(ctx, &kk::Http::ScriptClass);
            
            kk::Strong v = new kk::Http(_base,_dns);
            
            kk::script::PushObject(ctx, v.get());
            duk_put_global_string(ctx, "http");
        }
        
    }
    
    if(_app == nullptr) {
        _app = new kk::Application([path UTF8String],0,_jsContext);
        _app->retain();

        duk_context * ctx = _jsContext->jsContext();
        duk_push_pointer(ctx, _app);
        duk_put_global_string(ctx, "__app");
        
        BK_CTX
        BK_PTR(element, CFBridgingRetain(self), CFRelease);
        
        _app->document()->on("app_*", KKGLViewElement_event_cb, BK_ARG);
        
        _app->GAContext()->init();
        
        [self resizeGLContext:view.layer.contentsScale inWidth:view.width inHeight:view.height];
    }
    
    [EAGLContext setCurrentContext:nil];
    
    [_lock unlock];
    
}

-(void) uninstall {
    
    KKGLView * view = (KKGLView *) self.view;
    
    if(view == nil) {
        return;
    }
    
    [_lock lock];
    
    kk::DispatchQueue * queue = _queue;
    _queue = nullptr;
    
    kk::script::Context * jsContext = _jsContext;
    _jsContext = nullptr;
    
    kk::Application * app = _app;
    _app = nullptr;
    
    if(app != nullptr) {
        app->document()->off("app_*", KKGLViewElement_event_cb);
    }
    
    struct evdns_base * dns = _dns;
    _dns = nullptr;
    
    struct event_base * base = _base;
    _base = nullptr;
    
    _loaded = NO;
    
    [_lock unlock];
    
    [EAGLContext setCurrentContext:view.GLContext];
    
    if(queue != nullptr) {
        queue->join();
    }
    
    if(jsContext != nullptr) {
        jsContext->release();
        jsContext = nullptr;
    }
    
    if(app != nullptr) {
        app->release();
        app = nullptr;
    }
    
    if(queue != nullptr) {
        queue->release();
        queue = nullptr;
    }
    
    if(dns != nullptr) {
        evdns_base_free(dns, 0);
        dns = nullptr;
    }
    
    if(base != nullptr) {
        event_base_free(base);
        base = nullptr;
    }
    
    [EAGLContext setCurrentContext:nil];
    
}

-(void) setView:(UIView *)view {
    
    [(KKGLView *) self.view setDelegate:nil];
    [(KKGLView *) self.view recycle];
    
    [self uninstall];
    
    [super setView:view];
    
    [(KKGLView *) self.view setDelegate:self];
    [self.view setMultipleTouchEnabled:YES];
    
    [self install];
    
}


-(void) changedKey:(NSString *)key{
    [super changedKey:key];
}

-(void) resizeGLContext:(kk::GL::Float) scale inWidth:(GLsizei) inWidth inHeight:(GLsizei) inHeight {
    
    if(_app != nullptr) {
        
        kk::GL::Context * GLContext = _app->GAContext();
        
        kk::GL::Float width =  inWidth * 2.0f / scale;
        kk::GL::Float height =  inHeight * 2.0f / scale;
        kk::GL::Float dp = 750.0f / MIN(width,height) ;
        
        kk::GL::ContextState & s = GLContext->state();
        
        GLContext->setViewport(dp * width, dp * height);
        
        s.projection = glm::scale(kk::GL::mat4(1),kk::GL::vec3(2.0f / (dp *width),-2.0f / (dp * height),-0.0001));
    }
    
}

-(void) KKGLView:(KKGLView *)view resizeGLContext:(EAGLContext *)GLContext {
    
    [_lock lock];
    
    [self resizeGLContext:view.layer.contentsScale inWidth:view.width inHeight:view.height];
    
    [_lock unlock];
}

-(void) KKGLView:(KKGLView *)view displayGLContext:(EAGLContext *)GLContext {
    
    [_lock lock];
    
    if(_base != nullptr) {
        event_base_loop(_base, EVLOOP_ONCE | EVLOOP_NONBLOCK);
    }
    
    if(_app != nullptr) {
        
        if(!_app->isRunning()) {
            _app->run();
        }
        
        _app->exec();
   
        kk::GL::Context * GLContext = _app->GAContext();
        kk::Element * root = _app->document()->rootElement();
        
        if(!_loaded && (root == nullptr || GLContext->loadingProgress(root) >= 1.0f)) {
            
            _loaded = YES;
            
            __weak KKGLViewElement * element = self;
            
            dispatch_async(dispatch_get_main_queue(), ^{
                
                if(element) {
                    KKElementEvent * e = [[KKElementEvent alloc] initWithElement:element];
                    e.data = element.data;
                    [element _emit:@"load" event:e];
                }
                
            });
            
            
        }
        
    }

    [_lock unlock];
    
}


-(void) _emit:(NSString *)name event:(KKEvent *)event {
    [super emit:name event:event];
}

-(void) postEvent:(NSString *) name data:(id) data {
    
    dispatch_queue_t queue = [(KKGLView *) self.view queue];
    
    if(queue != nil) {
        
        __weak KKGLViewElement * element = self;
        
        dispatch_async(queue, ^{
            
            KKGLViewElement * e = element;
            
            if(e == nullptr) {
                return ;
            }
            
            [EAGLContext setCurrentContext: [(KKGLView *)e.view GLContext]];
            
            kk::Strong v = new kk::ElementEvent();
            
            kk::ElementEvent * ev = v.as<kk::ElementEvent>();
            
            [e->_lock lock];
            
            if(e->_jsContext != nullptr){
                
                duk_context * ctx = e->_jsContext->jsContext();
                
                [KKGLContext JSContextPushObject:data ctx:ctx];
                
                ev->data = new kk::script::Object(e->_jsContext,-1);
                
                duk_pop(ctx);
                
                if(e->_app != nullptr) {
                    e->_app->document()->emit([name UTF8String], ev);
                }
            }
            
            [e->_lock unlock];
            
            [EAGLContext setCurrentContext:nil];
            
        });
        
    }
    
}

-(void) emit:(NSString *)name event:(KKEvent *)event {
    [super emit:name event:event];
    
    if([event isKindOfClass:[KKElementEvent class]] && [@"reopen" isEqualToString:name]) {
        [(KKElementEvent *) event setCancelBubble:YES];
        
        dispatch_queue_t queue = [(KKGLView *) self.view queue];
        
        if(queue != nil) {
            [self uninstall];
            [self install];
        }
        
        return;
    }
    
    if([event isKindOfClass:[KKElementEvent class]]) {
        
        [(KKElementEvent *) event setCancelBubble:YES];
        
        id data = [(KKElementEvent *) event data];

        [self postEvent:name data:data];
        
    }
}

-(void) KKGLView:(KKGLView *) view touches:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event name:(NSString *) name {
    
    if(_app != nullptr) {
        
        NSMutableDictionary * data = [NSMutableDictionary dictionaryWithCapacity:4];
        
        CGSize size = view.bounds.size;
        
        if(size.width <=0 || size.height <=0){
            return;
        }
        
        data[@"width"] = @(self.frame.size.width);
        data[@"height"] = @(self.frame.size.height);
        data[@"type"] = name;
        
        NSMutableArray * items = [NSMutableArray arrayWithCapacity:4];
        
        data[@"count"] = @([[event touchesForView:view] count]);
        data[@"items"] = items;
        
        for(UITouch * touch in touches) {
            
            NSMutableDictionary * item = [NSMutableDictionary dictionaryWithCapacity:4];
            
            CGPoint p = [touch locationInView:view];
            
            item[@"dx"] = @(p.x * 2.0f / size.width -1.0f);
            item[@"dy"] = @(p.y * 2.0f / size.height -1.0f);
            item[@"x"] = @(p.x);
            item[@"y"] = @(p.y);
            item[@"id"] = [NSString stringWithFormat:@"0x%lx",(long) (__bridge void *) touch];
            
            [items addObject:item];
            
        }
        
        [self postEvent:name data:data];
        
    }
    
}

- (void) KKGLView:(KKGLView *) view touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    
    [self KKGLView:view touches:touches withEvent:event name:@"touchstart"];
    
}

- (void) KKGLView:(KKGLView *) view touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    
    [self KKGLView:view touches:touches withEvent:event name:@"touchmove"];
}

- (void) KKGLView:(KKGLView *) view touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    
    [self KKGLView:view touches:touches withEvent:event name:@"touchend"];
    
}

- (void) KKGLView:(KKGLView *) view touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    
    [self KKGLView:view touches:touches withEvent:event name:@"touchcancel"];
    
}

@end

@implementation KKGLView (KKViewElement)

@end
