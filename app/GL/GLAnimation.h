//
//  GLAnimation.h
//  KKGame
//
//  Created by zhanghailong on 2018/2/13.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef GLAnimation_h
#define GLAnimation_h

#include "GLContext.h"

namespace kk {
    
    namespace GL {
        
        enum AnimationState {
            AnimationStateNone,
            AnimationStateWait,    // 等待中
            AnimationStateExecute, // 执行中
        };
        
        class Animation : public kk::GA::Element {
        public:
            Animation();
            virtual void exec(kk::GA::Context * context);
            virtual kk::GL::AnimationTarget * target();
            virtual void changedKey(String& key);
            
            DEF_SCRIPT_CLASS
        public:
            AnimationState animationState;
            kk::GA::TimeInterval afterDelay;    // 延时时间
            kk::GA::TimeInterval duration;      // 执行时间
            kk::Int repeatCount;               // 重复次数
            kk::Boolean autoreverses;           // 自动反转
            Boolean enabled;
        protected:
            kk::GA::TimeInterval start;         // 开始时间
            virtual void onExec(Context * context);
            virtual void onExecute(Context * context,Float value);
            virtual void onStart(Context * context);
            virtual void onDone(Context * context);
            kk::Int _count;
        };
        
        class AnimationItem : public kk::Element, public LoadingProgress {
        public:
            AnimationItem();
            virtual void changedKey(String& key);
            virtual Image * image(Context * context);
            virtual Float loadingProgress();
            DEF_SCRIPT_CLASS
        public:
            mat4 transform;
            Float value;
        private:
            kk::Strong _image;
        };
        
    }
    
}

#endif /* GLAnimation_h */
