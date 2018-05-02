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
        
        class ViewportElement : public Element {
        public:
            ViewportElement();
            virtual void changedKey(String& key);
            virtual void exec(kk::GA::Context * context);
            virtual kk::GA::Scene * scene();
            DEF_SCRIPT_CLASS
        };
    }
    
}


#endif /* GLViewportElement_h */
