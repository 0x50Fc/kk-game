//
//  main.m
//  app
//
//  Created by hailong11 on 2018/2/8.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

/*
#import <Foundation/Foundation.h>

#include "kk-config.h"
#include "kk-script.h"
#include "kk-app.h"

int run = 1;

void stop(int signo){
    run = 0;
}


static duk_ret_t main_setTimeout(duk_context * ctx) {
    
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
            
            dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(ms * NSEC_PER_MSEC)), dispatch_get_main_queue(), ^{
                
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

static duk_ret_t main_clearTimeout(duk_context * ctx) {
    
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

static duk_ret_t main_Interval_dealloc(duk_context * ctx) {
    
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

static duk_ret_t main_setInterval(duk_context * ctx) {
    
    int top = duk_get_top(ctx);
    
    if(top > 1 && duk_is_function(ctx, -top) && duk_is_number(ctx, -top + 1)) {
        
        @autoreleasepool{
            
            void * heapptr = duk_get_heapptr(ctx, -top);
            int ms = duk_to_int(ctx, -top +1);
            
            duk_push_global_object(ctx);
            
            duk_push_sprintf(ctx, "0x%x",(long) heapptr);
            
            duk_push_object(ctx);
            
            duk_push_string(ctx, "source");
            
            dispatch_source_t source = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, dispatch_get_main_queue());
            
            duk_push_pointer(ctx, (void *) CFBridgingRetain(source));
            duk_put_prop(ctx, -3);
            
            duk_push_string(ctx, "func");
            duk_push_heapptr(ctx, heapptr);
            duk_put_prop(ctx, -3);
            
            duk_push_c_function(ctx, main_Interval_dealloc, 1);
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

static duk_ret_t main_clearInterval(duk_context * ctx) {
    
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


int main(int argc, const char * argv[]) {
    
    struct KKApplication * app = KKApplicationCreate();
    
    duk_context * ctx = app->jsContext;
    
    duk_push_global_object(ctx);
    
    duk_push_string(ctx, "setTimeout");
    duk_push_c_function(ctx, main_setTimeout, 2);
    duk_put_prop(ctx, -3);
    
    duk_push_string(ctx, "clearTimeout");
    duk_push_c_function(ctx, main_clearTimeout, 1);
    duk_put_prop(ctx, -3);
    
    duk_push_string(ctx, "setInterval");
    duk_push_c_function(ctx, main_setInterval, 2);
    duk_put_prop(ctx, -3);
    
    duk_push_string(ctx, "cleaerInterval");
    duk_push_c_function(ctx, main_clearInterval, 1);
    duk_put_prop(ctx, -3);
    
    KKApplicationRun(app, "/Users/hailong11/Documents/src/git.sc.weibo.com/kk/fruit/srv/");
    
    signal(SIGINT, stop);
    
    while(run) {
        KKApplicationExec(app);
        sleep(1000 / 30);
    }
    
    KKApplicationExit(app);
    
    return 0;
}
*/

#include <chipmunk/chipmunk.h>

int main(int argc, const char * argv[]) {
    
    // cpVect是2D矢量，cpv()为初始化矢量的简写形式
    cpVect gravity = cpv(0, 0);
    
    // 创建一个空白的物理世界
    cpSpace *space = cpSpaceNew();
    cpSpaceSetGravity(space, gravity);
    
    // 为地面创建一个静态线段形状
    // 我们稍微倾斜线段以便球可以滚下去
    // 我们将形状关联到space的默认静态刚体上，告诉Chipmunk该形状是不可移动的
//    cpShape *ground = cpSegmentShapeNew(cpSpaceGetStaticBody(space), cpv(-20, 5), cpv(20, -5), 0);
//    cpShapeSetFriction(ground, 0);
//    cpSpaceAddShape(space, ground);
    
    // 现在让我们来构建一个球体落到线上并滚下去
    // 首先我们需要构建一个 cpBody 来容纳对象的物理属性
    // 包括对象的质量、位置、速度、角度等
    // 然后我们将碰撞形状关联到cpBody上以给它一个尺寸和形状
    
    cpFloat radius = 5;
    cpFloat mass = 1;
    
    // 转动惯量就像质量对于旋转一样
    // 使用 cpMomentFor*() 来近似计算它
    cpFloat moment = cpMomentForCircle(mass, 0, radius, cpvzero);
    
    // cpSpaceAdd*() 函数返回你添加的东西
    // 很便利在一行中创建并添加一个对象
    cpBody *ballBody = cpSpaceAddBody(space, cpBodyNew(mass, moment));
    cpBodySetPosition(ballBody, cpv(0, 0));
    cpBodyApplyForceAtLocalPoint(ballBody, {10000,10000}, {0,0});
    
    // 现在我们会球体创建碰撞形状
    // 你可以为同一个刚体创建多个碰撞形状
    // 它们将会附着关联到刚体上并移动更随
    //cpShape *ballShape = cpSpaceAddShape(space, cpCircleShapeNew(ballBody, radius, cpvzero));
    //cpShapeSetFriction(ballShape, 0.7);
    
    // 现在一切都建立起来了，我们通过称作时间步的小幅度时间增量来步进模拟空间中的所有物体
    // *高度*推荐使用固定长的时间步
    cpFloat timeStep = 1.0/60.0;
    for(cpFloat time = 0; time < 20; time += timeStep){
        cpVect pos = cpBodyGetPosition(ballBody);
        cpVect vel = cpBodyGetVelocity(ballBody);
        printf(
               "Time is %5.2f. ballBody is at (%5.2f, %5.2f). It's velocity is (%5.2f, %5.2f)\n",
               time, pos.x, pos.y, vel.x, vel.y
               );
        
        cpSpaceStep(space, timeStep);
    }
    
    // 清理我们的对象并退出
    //cpShapeFree(ballShape);
    cpBodyFree(ballBody);
//    cpShapeFree(ground);
    cpSpaceFree(space);
    
    return 0;
}
