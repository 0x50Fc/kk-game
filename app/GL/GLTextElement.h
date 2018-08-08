//
//  GLTextElement.h
//  KKGame
//
//  Created by zhanghailong on 2018/3/6.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef GLTextElement_h
#define GLTextElement_h

#include "GLContext.h"

namespace kk {
    
    namespace GL {
        
        class TextElement : public Element {
        public:

            virtual void changedKey(String& key);
            
            Paint paint;
            vec2 anchor; // 0~1.0
            
            KK_DEF_ELEMENT_CREATE(TextElement)
            DEF_SCRIPT_CLASS_NOALLOC
            
        protected:
            virtual void onDraw(Context * context);
            Strong _texture;
            bool _display;
        };
    }
}

#endif /* GLTextElement_h */
