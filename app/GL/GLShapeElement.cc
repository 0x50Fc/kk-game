//
//  GLShapeElement.ccc
//  KKGame
//
//  Created by zhanghailong on 2018/4/27.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include "kk-config.h"
#include "GLShapeElement.h"
#include "GAShape.h"
#include <chipmunk/chipmunk.h>

#if defined(KK_PLATFORM_IOS)

#include <KKObject/KKObject.h>

#else

#include "kk-string.h"

#endif

namespace kk {
    
    namespace GL {
        
        IMP_SCRIPT_CLASS_BEGIN_NOALLOC(&Element::ScriptClass, ShapeElement, GLShapeElement)
        
        IMP_SCRIPT_CLASS_END
        
        KK_IMP_ELEMENT_CREATE(ShapeElement)
        
        ShapeElement::ShapeElement(kk::Document * document,kk::CString name, kk::ElementKey elementId):Element(document,name,elementId),color(1,0,0,1) {
            
        }
        
        void ShapeElement::changedKey(String& key) {
            Element::changedKey(key);
            
            if(key == "color") {
                color = colorValue(get(key.c_str()));
            }
        }
        
        void ShapeElement::onDraw(Context * context) {
            Element::onDraw(context);
            kk::Element * p = parent();
            
            if(p != nullptr) {
                
                kk::Element * e = p->firstChild();
                while(e){
                    
                    kk::GA::Shape * v = dynamic_cast<kk::GA::Shape *>(e);
                    
                    if(v != nullptr) {
                        
                        switch (v->shapeType) {
                            case kk::GA::ShapeTypeCircle:
                                
                            {
                                std::vector<vec3> ps;
                                
                                Float radius = kk::GA::floatValue(e->get("radius"));
                                Float dx = kk::GA::floatValue(e->get("x"));
                                Float dy = kk::GA::floatValue(e->get("y"));
                                
                                if(radius <= 0.0f) {
                                    return;
                                }
                                
                                ps.push_back(vec3(dx,dy,0));
                                
                                for(Float x = -radius;x <= radius; x += 1.0f) {
                                    
                                    Float r = acos(x / radius);
                                
                                    Float y = radius * sinf(r);
                                    
                                    ps.push_back(vec3(dx + x,dy + y,0.0f));
                                    
                                }
                                
                                for(Float x = radius;x >= -radius; x -= 1.0f) {
                                    
                                    Float r = acos(x / radius);
                                    
                                    Float y = - radius * sinf(r);
                                    
                                    ps.push_back(vec3(dx + x,dy + y,0.0f));
                                    
                                }
                                
                                context->drawFillColor(GL_TRIANGLE_FAN, ps.data(), (kk::Uint) ps.size(), color);
                            }
                                
                                break;
                            case kk::GA::ShapeTypeBox :
                            {
                                
                                Float width = kk::GA::floatValue(e->get("width"));
                                Float height = kk::GA::floatValue(e->get("height"));
                                Float dx = kk::GA::floatValue(e->get("x"));
                                Float dy = kk::GA::floatValue(e->get("y"));
                                Float l = dx - width * 0.5f;
                                Float t = dy - height * 0.5f;
                                Float r = l + width;
                                Float b = t + height;
                                
                                vec3 ps[] = {
                                    {l,t,0},{l,b,0},{r,t,0},{r,b,0}
                                };
                                
                                context->drawFillColor(GL_TRIANGLE_STRIP, ps, 4, color);
                            }
                                break;
                            case kk::GA::ShapeTypePoly :
                            {
                                
                                float x,y;
                                std::vector<vec3> ps;
                                std::vector<kk::String> vs;
                                kk::CStringSplit(e->get("points"), " ", vs);
                                
                                std::vector<kk::String>::iterator i = vs.begin();
                                
                                while(i != vs.end()) {
                                    x = 0;
                                    y = 0;
                                    sscanf((*i).c_str(), "%f,%f",&x,&y);
                                    ps.push_back({x,y,0});
                                    i ++;
                                }
                                
                                if(ps.size() > 2) {
                                    context->drawFillColor(GL_TRIANGLE_FAN, ps.data(), (kk::Uint) ps.size(), color);
                                }
                            }
                                break;
                                
                            default:
                                break;
                        }
                    }
                    
                    e = e->nextSibling();
                }
                
            }
            
        }
        
        
    }
}

