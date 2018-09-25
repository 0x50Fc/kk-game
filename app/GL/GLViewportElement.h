//
//  GLViewportElement.h
//  KKGame
//
//  Created by zhanghailong on 2018/3/12.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef GLViewportElement_h
#define GLViewportElement_h

#include "GLContext.h"
#include "GAScene.h"

namespace kk {
    
    namespace GL {
        
        class ViewElement;
        
        extern kk::Float Auto;
        
        class ViewportElement : public Element {
        public:

            virtual void changedKey(String& key);
            virtual void exec(kk::GA::Context * context);
            virtual kk::GA::Scene * scene();
            
            KK_DEF_ELEMENT_CREATE(ViewportElement)
            DEF_SCRIPT_CLASS_NOALLOC
            
            
            virtual ViewElement * findView(Float x,Float y);
            
            virtual duk_ret_t duk_findView(duk_context * ctx);
            
        private:
            Float _width;
            Float _height;
        };
    }
    
}


#endif /* GLViewportElement_h */
