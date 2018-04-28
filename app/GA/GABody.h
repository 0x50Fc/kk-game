//
//  GABody.h
//  KKGame
//
//  Created by hailong11 on 2018/2/1.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef GABody_h
#define GABody_h

#include "GAContext.h"

struct cpBody;

namespace kk {
    
    namespace GA {
       
        
        class Scene;
        class Shape;
        
        enum BodyType {
            BodyTypeFixed,      //固定地
            BodyTypeMovable,    //活动的
        };
        
        class Body : public Element ,public IPosition {
        public:
            Body();
            virtual ~Body();
            virtual Point position();
            virtual void setPosition(Point value);
            virtual Float zIndex();
            virtual void changedKey(String& key);
            virtual Scene * scene();
            virtual ::cpBody * cpBody();
            virtual void onWillRemoveFromParent(kk::Element * element);
            
            virtual void exec(Context * context);
    
            BodyType bodyType;
            
            virtual duk_ret_t duk_position(duk_context * ctx);
            virtual duk_ret_t duk_setPosition(duk_context * ctx);
            
        DEF_SCRIPT_CLASS
        
        protected:
            Point _position;
            Float _zIndex;
            ::cpBody * _cpBody;
        };

    }
    
}

#endif /* GABody_h */
