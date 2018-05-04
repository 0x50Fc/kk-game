//
//  kk-lws.h
//  KKGame
//
//  Created by hailong11 on 2018/5/4.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef kk_lws_h
#define kk_lws_h

#include "kk-script.h"
#include <uv.h>

namespace kk {
    
    void lws_openlibs(duk_context * ctx, uv_loop_t * loop);
    
}


#endif /* kk_lws_hpp */
