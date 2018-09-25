//
//  GLViewElement.h
//  KKGame
//
//  Created by hailong11 on 2018/9/25.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef GLViewElement_h
#define GLViewElement_h


#include "GLContext.h"

namespace kk {
    
    namespace GL {
        
        
        
        class ViewElement : public Element {
        public:
            
            
            KK_DEF_ELEMENT_CREATE(ViewElement)
            DEF_SCRIPT_CLASS_NOALLOC
            
            virtual void changedKey(String& key);
            
            virtual ViewElement * findView(Float x,Float y);
            
        private:
            virtual void exec(kk::GA::Context * context);
            kk::Float _left;
            kk::Float _top;
            kk::Float _right;
            kk::Float _bottom;
            kk::Float _width;
            kk::Float _height;
        };
    }
    
}


#endif /* GLViewElement_h */
