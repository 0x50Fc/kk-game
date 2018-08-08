//
//  GLShapeElement.h
//  KKGame
//
//  Created by zhanghailong on 2018/4/27.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef GLShapeElement_h
#define GLShapeElement_h

#include "GLContext.h"


namespace kk {
    
    namespace GL {
        
        class ShapeElement : public Element {
        public:

            virtual void changedKey(String& key);
            virtual void onDraw(Context * context);
            
            vec4 color;
            
            KK_DEF_ELEMENT_CREATE(ShapeElement)
            DEF_SCRIPT_CLASS_NOALLOC
            
        protected:
            
        };
    }
    
}

#endif /* GLShapeElement_hpp */
