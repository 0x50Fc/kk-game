//
//  GLMinimapElement.hpp
//  KKGame
//
//  Created by zuowu on 2018/3/9.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef GLMinimapElement_hpp
#define GLMinimapElement_hpp

#include "GLContext.h"
#include "GAScene.h"

namespace kk {
    
    namespace GL {
        
        class MinimapElement : public Element, public kk::GA::IScene {
        public:
    
            virtual void changedKey(String& key);
            virtual kk::GA::Scene * scene();
            vec2 size;
            
            KK_DEF_ELEMENT_CREATE(MinimapElement)
            DEF_SCRIPT_CLASS_NOALLOC
            
        protected:
            virtual void onDraw(Context * context);
            std::map<String,Strong> _images;
        };
    }
    
}

#endif /* GLMinimapElement_hpp */
