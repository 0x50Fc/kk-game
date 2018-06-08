//
//  kk-ev.h
//  app
//
//  Created by zhanghailong on 2018/6/8.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef kk_ev_h
#define kk_ev_h

#include "kk-object.h"
#include <event.h>

namespace kk {
    
    event_base * ev_base(duk_context * ctx);
    
    void ev_openlibs(duk_context * ctx,event_base * base);
    
}

#endif /* kk_event_hpp */
