//
//  GAAction.h
//  KKGame
//
//  Created by zhanghailong on 2018/2/8.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef GAAction_h
#define GAAction_h

#include "GAContext.h"
#include "GAInCollision.h"

namespace kk {

    namespace GA {
        
        class Body;
        class Shape;
  

        class Action : public InCollision {
        public:

            virtual Body * body();
        
            DEF_SCRIPT_CLASS_NOALLOC
            
            KK_DEF_ELEMENT_CREATE(Action)
            
        };
        
    }
}

#endif /* GAAction_h */
