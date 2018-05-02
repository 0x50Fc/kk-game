//
//  GLViewportElement.cpp
//  KKGame
//
//  Created by zhanghailong on 2018/3/12.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//


#include "kk-config.h"
#include "GLViewportElement.h"
#include "GAScene.h"
#include "GABody.h"
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

namespace kk {
    
    namespace GL
    {
        IMP_SCRIPT_CLASS_BEGIN(&Element::ScriptClass, ViewportElement, GLViewportElement)
        
        IMP_SCRIPT_CLASS_END
        
        ViewportElement::ViewportElement() {
            
        }
        
        
        void ViewportElement::changedKey(String& key) {
            Element::changedKey(key);
            
            
        }
        
        kk::GA::Scene * ViewportElement::scene(){
            return dynamic_cast<kk::GA::Scene *>(parent());
        }
        
        void ViewportElement::exec(kk::GA::Context * context) {
            Element::exec(context);
            
            kk::GA::Size viewport = context->viewport();
            
            transform = glm::translate(mat4(1.0f), vec3(-viewport.width * 0.5f,-viewport.height * 0.5f,0.0f));
            
        }
        
    }
    
}
