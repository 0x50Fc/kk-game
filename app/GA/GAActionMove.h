//
//  GAActionMove.h
//  KKGame
//
//  Created by hailong11 on 2018/2/9.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef GAActionMove_h
#define GAActionMove_h

#include "GAAction.h"

namespace kk {
    
    namespace GA {
        
        
        class ActionMove : public Action {
        public:
            ActionMove();
            virtual void changedKey(String& key);
            virtual void exec(Context * context);
            Point move;     //移动位置
            Float speed;    //速度
            Float angle;    //移动方向
            
            DEF_SCRIPT_CLASS
        protected:
            kk::Boolean _hasUpdate;
            kk::Boolean _landing;
            Float _distance;
        };
    }
    
}

#endif /* GAActionMove_h */
