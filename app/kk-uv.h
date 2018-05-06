//
//  kk-uv.h
//  KKGame
//
//  Created by 张海龙 on 2018/5/5.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef kk_uv_h
#define kk_uv_h

#include "kk-script.h"
#include <uv.h>

namespace kk {
    
    void uv_openlibs(duk_context * ctx, uv_loop_t * loop);
    
    uv_loop_t * uv_get_loop(duk_context * ctx);
    
}


#endif /* kk_uv_hpp */
