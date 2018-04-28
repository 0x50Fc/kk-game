//
//  GAAction.c
//  KKGame
//
//  Created by hailong11 on 2018/2/8.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include "kk-config.h"
#include "GAAction.h"
#include "GABody.h"
#include "GAShape.h"
#include <typeinfo>

namespace kk {
 
    namespace GA {
        
        IMP_SCRIPT_CLASS_BEGIN(&kk::GA::Element::ScriptClass, Action, GAAction)
        
        IMP_SCRIPT_CLASS_END
        
        Action::Action(){
            
        }
        
        Body * Action::body() {
            return dynamic_cast<Body *>(parent());
        }
        
        
    }

}
