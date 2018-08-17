//
//  GAShape.h
//  KKGame
//
//  Created by zhanghailong on 2018/2/1.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef GAShape_h
#define GAShape_h

#include "GAContext.h"

struct cpShape;

namespace kk {
    
    namespace GA {
        
        class Body;
        
        enum ShapeType {
            ShapeTypeCircle,
            ShapeTypeSegment,
            ShapeTypePoly,
            ShapeTypeBox,
        };
        
        class Shape : public Element {
        public:
            virtual ~Shape();
            virtual void exec(Context * context);
            virtual Body * body();
            virtual ::cpShape * cpShape();
            virtual void changedKey(String& key);
            virtual void onWillRemoveFromParent(kk::Element * element);
            
            virtual Point position();
            virtual void setPosition(Point value);
            
            virtual duk_ret_t duk_position(duk_context * ctx);
            virtual duk_ret_t duk_setPosition(duk_context * ctx);
            
            virtual kk::Int collisionType();
            virtual void setCollisionType(kk::Int collisionType);
            
            ShapeType shapeType;
  
            DEF_SCRIPT_CLASS_NOALLOC
            
            KK_DEF_ELEMENT_CREATE(Shape)
            
        protected:
            Point _position;
            ::cpShape * _cpShape;
            kk::Int _collisionType;
        };
        
    }
    
}


#endif /* GAShape_h */
