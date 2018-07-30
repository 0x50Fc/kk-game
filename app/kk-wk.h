//
//  kk-wk.h
//  KKGame
//
//  Created by hailong11 on 2018/7/30.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef kk_wk_h
#define kk_wk_h

#include "kk-ev.h"
#include "kk-dispatch.h"

namespace kk {
    
    typedef void (*EventOnCreateContext) (duk_context * ctx, kk::DispatchQueue * queue, duk_context * newContext);
    
    void wk_openlibs(duk_context * ctx,kk::DispatchQueue * queue, EventOnCreateContext onCreateContext);
    
}

#endif /* kk_wk_h */
