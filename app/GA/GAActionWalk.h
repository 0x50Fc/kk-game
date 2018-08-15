//
//  GAActionWalk.h
//  KKGame
//
//  Created by zhanghailong on 2018/2/9.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef GAActionWalk_h
#define GAActionWalk_h

#include "GAAction.h"

namespace kk {
    
    namespace GA {
        
        enum ActionWalkNavigateState {
            ActionWalkNavigateStateNone,
            ActionWalkNavigateStateNavigating
        };
        
        class ActionWalk : public Action {
        public:

            virtual void changedKey(String& key);
            virtual void exec(Context * context);
            Float x;       //移动位置
            Float y;       //移动位置
            Float speed;    //速度
            Float angle;    //移动方向
            
            Float duration;    //持续时间
            kk::Weak target;   //跟随目标
            
            virtual duk_ret_t duk_target(duk_context * ctx);
            virtual duk_ret_t duk_setTarget(duk_context * ctx);
            virtual duk_ret_t duk_navigate(duk_context * ctx);  //碰撞寻路
            
            DEF_SCRIPT_CLASS_NOALLOC
            
            KK_DEF_ELEMENT_CREATE(ActionWalk)
            
        protected:
            
            kk::Boolean _hasUpdate;
            kk::Boolean _landing;
            kk::Boolean _enabled;
            Float _distance;
            TimeInterval _startTimeInterval;
            ActionWalkNavigateState _navigateState;
            TimeInterval _navigateStartTimeInterval;
            TimeInterval _navigateDuration;
            Point _lastPosition;
        };
    }
    
}

#endif /* GAActionWalk_h */
