//
//  GLMinimapElement.cpp
//  KKGame
//
//  Created by zuowu on 2018/3/9.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include "kk-config.h"
#include "GLMinimapElement.h"
#include "GAScene.h"
#include "GABody.h"
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

using namespace std;
namespace kk {
    
    namespace GL
    {
        IMP_SCRIPT_CLASS_BEGIN_NOALLOC(&Element::ScriptClass, MinimapElement, GLMinimapElement)
        
        IMP_SCRIPT_CLASS_END
        
        KK_IMP_ELEMENT_CREATE(MinimapElement)
        
        MinimapElement::MinimapElement(kk::Document * document,kk::CString name, kk::ElementKey elementId):Element(document,name,elementId), size(0) {
            
        }
        
        void MinimapElement::changedKey(String& key) {
            Element::changedKey(key);
            
            if(key == "width") {
                size.x = kk::GA::floatValue(get(key.c_str()));
            } else if(key == "height") {
                size.y = kk::GA::floatValue(get(key.c_str()));
            } 
        }
        
        kk::GA::Scene * MinimapElement::scene() {
            kk::Element * p = parent();
            while(p) {
                kk::GA::IScene * v = dynamic_cast<kk::GA::IScene *>(p);
                if(v) {
                    return v->scene();
                }
                p = p->parent();
            }
            return nullptr;
        }
        
        void MinimapElement::onDraw(Context * context) {
            Element::onDraw(context);
            
            {
                vec4 p[] = {{- 0.5f * size.x,- 0.5f * size.y,0,1}
                    ,{(1.0f - 0.5f) * size.x,(1.0f - 0.5f) * size.y,0,1}};
                
                if(!context->isVisible(p, 2,vec4(0))) {
                    return;
                }
            }
 
            kk::GA::Scene * scene = this->scene();
            
            if (scene) {
                
                kk::Element * e = scene->firstChild();
                
                while(e){
                    
                    kk::GA::Body * v = dynamic_cast<kk::GA::Body *>(e);
                    
                    if (v) {
                        
                        kk::CString path = v->get("mini-src");
                        
                        if(v && path != nullptr) {
                            
                            float x = v->position().x * size.x / scene->size.width;
                            float y = v->position().y * size.y / scene->size.height;
                            
                            Image * image = nullptr;
                            
                            std::map<String,Strong>::iterator i = _images.find(path);
                            
                            if(i != _images.end()) {
                                image = i->second.as<Image>();
                            } else {
                                Strong vv = context->image(path);
                                image = vv.as<Image>();
                                _images[path] = image;
                            }
                            
                            if(image != nullptr && image->status() == ImageStatusLoaded) {
                                
                                context->store();
                                
                                ContextState & state = context->state();
                                
                                state.view = glm::rotate(glm::translate(state.view, vec3(x + size.x * 0.5f,y + size.y * 0.5f,0)), v->angle, vec3(0,0,-1));
                                
                                vec4 dest( - image->width() *0.5f,
                                           - image->height() *0.5f,
                                          image->width(),image->height());
                                vec4 src(0,0,image->width(),image->height());
                                context->drawTexture(image, dest, src,0);
                                
                                context->restore();
                                
                            }
                        }
                    }
                    
                    e = e->nextSibling();
                }
            }

        }
    }
    
}
