//
//  GAAction.c
//  KKGame
//
//  Created by zhanghailong on 2018/2/8.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include "kk-config.h"
#include "GAAction.h"
#include "GABody.h"
#include "GAShape.h"
#include <typeinfo>

namespace kk {
 
    namespace GA {
        
        IMP_SCRIPT_CLASS_BEGIN_NOALLOC(&kk::GA::Element::ScriptClass, Action, GAAction)
        
        IMP_SCRIPT_CLASS_END
        
        KK_IMP_ELEMENT_CREATE(Action)
        
        Action::Action(kk::Document * document,kk::CString name, kk::ElementKey elementId):kk::GA::InCollision(document,name,elementId){
        }
        
        Body * Action::body() {
            return dynamic_cast<Body *>(parent());
        }
        

    }

}
