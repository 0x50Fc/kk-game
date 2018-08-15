//
//  GAScene.h
//  KKGame
//
//  Created by zhanghailong on 2018/2/1.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef GAScene_h
#define GAScene_h

#include "GAContext.h"

struct cpSpace;

namespace kk {
    
    namespace GA {
        
        class Body;
        class Shape;
        class Scene;
        
        class IScene  {
        public:
            virtual Scene * scene() = 0;
        };
        
        class Scene : public Element , public IPosition , public IScene {
        
        public:
            virtual ~Scene();
            virtual void exec(Context * context);
            virtual void changedKey(String& key);
            
            virtual ::cpSpace * cpSpace();
            
            virtual Body * focus();
            virtual void setFocus(Body * body);
            virtual Point convert(Point point);    // 屏幕坐标 (-1.0 ～ 1.0 , -1.0~ 1.0), 转换为场景坐标
            
            virtual duk_ret_t duk_focus(duk_context * ctx);
            virtual duk_ret_t duk_setFocus(duk_context * ctx);
            virtual duk_ret_t duk_convert(duk_context * ctx);
            /**
             * 附近物体
             * scene.nearby(x,y,r) -> []
             */
            virtual duk_ret_t duk_nearby(duk_context * ctx);
            
            virtual Point position();
            virtual void setPosition(Point p);
            virtual Float zIndex();
            virtual Size & viewport();
            virtual Scene * scene();
            
            KK_DEF_ELEMENT_CREATE(Scene)
            
            DEF_SCRIPT_CLASS_NOALLOC
            
        public:
            Size size;
    
        protected:
            Point _position;
            Size _viewport;
            Weak _focus;
            TimeInterval _prevTimeInterval;
            ::cpSpace * _cpSpace;
        };
        
    }
    
}

#endif /* GAScene_h */
