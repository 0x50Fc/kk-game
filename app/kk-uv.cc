//
//  kk-uv.c
//  KKGame
//
//  Created by 张海龙 on 2018/5/5.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include "kk-config.h"
#include "kk-uv.h"

namespace kk {
    
    void uv_openlibs(duk_context * ctx, uv_loop_t * loop) {
        
        duk_push_global_object(ctx);
        
        
        duk_push_string(ctx, "__uv_loop");
        duk_push_pointer(ctx, loop);
        duk_def_prop(ctx, -3, DUK_DEFPROP_CLEAR_ENUMERABLE |
                     DUK_DEFPROP_CLEAR_WRITABLE |
                     DUK_DEFPROP_HAVE_VALUE |
                     DUK_DEFPROP_CLEAR_CONFIGURABLE);
        
        duk_pop(ctx);
        
        
    }
    
    uv_loop_t * uv_get_loop(duk_context * ctx) {
        
        uv_loop_t * v = nullptr;
        
        duk_push_global_object(ctx);
        
        duk_get_prop_string(ctx, -1, "__uv_loop");
        
        if(duk_is_pointer(ctx, -1)) {
            v = (uv_loop_t *) duk_to_pointer(ctx, -1);
        }
        
        duk_pop(ctx);
        
        return v;
    }
    
}
