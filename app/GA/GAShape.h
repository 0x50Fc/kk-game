//
//  GAShape.h
//  KKGame
//
//  Created by hailong11 on 2018/2/1.
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
            Shape();
            virtual ~Shape();
            virtual void exec(Context * context);
            virtual Body * body();
            virtual ::cpShape * cpShape();
            virtual void changedKey(String& key);
            virtual void onWillRemoveFromParent(kk::Element * element);
            
            ShapeType shapeType;
  
            DEF_SCRIPT_CLASS
        protected:
            ::cpShape * _cpShape;
        };
        
    }
    
}


#endif /* GAShape_h */
