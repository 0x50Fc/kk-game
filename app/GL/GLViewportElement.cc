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
#include "GLViewElement.h"

namespace kk {
    
    namespace GL {
        
        kk::Float Auto = 0x1.fffffep+127f;
        
        IMP_SCRIPT_CLASS_BEGIN_NOALLOC(&Element::ScriptClass, ViewportElement, GLViewportElement)
        
        static kk::script::Method methods[] = {
            {"findView",(kk::script::Function) &ViewportElement::duk_findView},
        };
        
        kk::script::SetMethod(ctx, -1, methods, sizeof(methods) / sizeof(kk::script::Method));
        
        IMP_SCRIPT_CLASS_END
        
        KK_IMP_ELEMENT_CREATE(ViewportElement)
        
        ViewportElement::ViewportElement(kk::Document * document,kk::CString name, kk::ElementKey elementId):Element(document,name,elementId) ,_width(Auto) ,_height(Auto){
            
        }
        
        
        void ViewportElement::changedKey(String& key) {
            Element::changedKey(key);
            
            
        }
        
        kk::GA::Scene * ViewportElement::scene(){
            return dynamic_cast<kk::GA::Scene *>(parent());
        }
        
        void ViewportElement::exec(kk::GA::Context * context) {
            
            kk::GA::Size viewport = context->viewport();
            
            transform = glm::translate(mat4(1.0f), vec3(-viewport.width * 0.5f,-viewport.height * 0.5f,0.0f));
            
            _width = viewport.width;
            _height = viewport.height;
            
            Element::exec(context);
        }
        
        ViewElement * ViewportElement::findView(Float x,Float y) {
            
            ViewElement * r = nullptr;
            
            kk::Element * e = lastChild();
            
            while(e) {
                
                {
                    ViewElement * v = dynamic_cast<ViewElement *>(e);
                    
                    if(v) {
                        r = v->findView(x, y);
                        if(r != nullptr) {
                            break;
                        }
                    }
                    
                }
                e = e->prevSibling();
            }
            
            return r;
        }
        
        duk_ret_t ViewportElement::duk_findView(duk_context * ctx) {
            
            Float dx = 0;
            Float dy = 0;
            
            int top = duk_get_top(ctx);
            
            if(top > 1
               && duk_is_number(ctx, -top)
               && duk_is_number(ctx, -top + 1)
               && _width != Auto && _height != Auto) {
                
                dx = duk_to_number(ctx, -top);
                dy = duk_to_number(ctx, -top + 1);
                
                ViewElement * v = findView((dx + 1.0f) * _width * 0.5f , (dy + 1.0f) * _height * 0.5f);
                
                if(v == nullptr) {
                    duk_push_null(ctx);
                    return 1;
                }
                
                kk::script::PushObject(ctx, v);
                
                return 1;
                
            }
            
            return 0;
        }
        
        
    }
    
}
