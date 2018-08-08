//
//  GLImageElement.c
//  KKGame
//
//  Created by zhanghailong on 2018/2/6.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include "kk-config.h"
#include "GLImageElement.h"
#include "GABody.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

namespace kk {
   
    namespace GL {
        
        IMP_SCRIPT_CLASS_BEGIN_NOALLOC(&Element::ScriptClass, ImageElement, GLImageElement)
        
        IMP_SCRIPT_CLASS_END
        
        KK_IMP_ELEMENT_CREATE(ImageElement)
        
        ImageElement::ImageElement(kk::Document * document,kk::CString name, kk::ElementKey elementId)
            :Element(document,name,elementId)
            ,size(0),anchor(0.5,0.5),capLeft(0),capTop(0),capRight(0),capBottom(0) {
            
        }
        
        kk::CString ImageElement::src() {
            return _src.c_str();
        }
        
        void ImageElement::setSrc(kk::CString src) {
            _src = src;
            _image = (Image *) nullptr;
        }
        
        void ImageElement::changedKey(String& key) {
            Element::changedKey(key);
            
            if(key == "src") {
                setSrc(get(key.c_str()));
            } else if(key == "width") {
                size.x = kk::GA::floatValue(get(key.c_str()));
                _points.clear();
            } else if(key == "height") {
                size.y = kk::GA::floatValue(get(key.c_str()));
                _points.clear();
            } else if(key == "anchor-x") {
                anchor.x = kk::GA::floatValue(get(key.c_str()));
                _points.clear();
            } else if(key == "anchor-y") {
                anchor.y = kk::GA::floatValue(get(key.c_str()));
                _points.clear();
            } else if(key == "cap-left") {
                capLeft = kk::GA::floatValue(get(key.c_str()));
                _points.clear();
            } else if(key == "cap-top") {
                capTop = kk::GA::floatValue(get(key.c_str()));
                _points.clear();
            } else if(key == "cap-bottom") {
                capBottom = kk::GA::floatValue(get(key.c_str()));
                _points.clear();
            } else if(key == "cap-right") {
                capRight = kk::GA::floatValue(get(key.c_str()));
                _points.clear();
            }
        }
        
        void ImageElement::addRect(Float l,Float t,Float r,Float b,Float sl,Float st,Float sr,Float sb) {
            
            TextureVertex p;
            
            p.position = {l,t,0};
            p.texCoord = { sl,st };
            _points.push_back(p);
            
            p.position = {r,t,0};
            p.texCoord = { sr,st };
            _points.push_back(p);
            
            p.position = {r,b,0};
            p.texCoord = { sr,sb };
            _points.push_back(p);
            
            p.position = {l,t,0};
            p.texCoord = { sl,st };
            _points.push_back(p);
            
            p.position = {l,b,0};
            p.texCoord = { sl,sb };
            _points.push_back(p);
            
            p.position = {r,b,0};
            p.texCoord = { sr,sb };
            _points.push_back(p);
            
        }
        
        void ImageElement::onDraw(Context * context) {
            Element::onDraw(context);
            
            {
                vec4 p[] = {{- anchor.x * size.x,- anchor.y * size.y,0,1},{(1.0f - anchor.x) * size.x,(1.0f - anchor.y) * size.y,0,1}};
                
                if(!context->isVisible(p, 2,vec4(0))) {
                    return;
                }
            }
            
            if(_image.get() == nullptr && _src.size() > 0) {
                Strong v = context->image(_src.c_str());
                _image = v.as<Image>();
            }
            
            Image * v = _animationImage.as<Image>();
            
            if(v == nullptr || v->status() != ImageStatusLoaded) {
                v = _image.as<Image>();
            }
            
            if(v != nullptr && v->status() == ImageStatusLoaded) {
                
                if(_points.empty()) {
                    
                    Float top = - anchor.y * size.y;
                    Float bottom =(1.0f - anchor.y) * size.y;
                    Float left = - anchor.x * size.x;
                    Float right = (1.0f - anchor.x) * size.x;
                    
                    if(size.x == 0.0f && size.y == 0.0f) {
                        top = - anchor.y * v->height();
                        bottom =(1.0f - anchor.y) * v->height();
                        left = - anchor.x * v->width();
                        right = (1.0f - anchor.x) * v->width();
                    }
                    
                    Float t = top + capTop;
                    Float b = bottom - capBottom ;
                    Float l = left + capLeft;
                    Float r = right - capRight;
                    
                    Float st = capTop / v->height();
                    Float sb = (v->height() - capBottom) / v->height();
                    Float sl = capLeft / v->width();
                    Float sr = (v->width() - capRight) / v->width();
                    
                    addRect(l, t, r, b, sl, st, sr, sb);
                    

                    if(capTop >0 && capLeft >0 ) {
                        addRect(left, top, l, t, 0, 0, sl, st);
                    }

                    if(capBottom >0 && capLeft >0 ) {
                        addRect(left, b, l, bottom, 0, sb, sl, 1.0f);
                    }

                    if(capTop >0 && capRight >0 ) {
                        addRect(r, top, right, t, sr, 0, 1.0f, st);
                    }

                    if(capBottom >0 && capRight >0 ) {
                        addRect(r, b, right, bottom, sr, sb, 1.0f, 1.0f);
                    }

                    if(capLeft > 0) {
                        addRect(left, t, l, b, 0, st, sl, sb);
                    }

                    if(capRight > 0) {
                        addRect(r, t, right, b, sr, st, 1.0f, sb);
                    }

                    if(capTop > 0) {
                        addRect(l, top, r, t, sl, 0, sr, st);
                    }

                    if(capBottom > 0) {
                        addRect(l, b, r, bottom, sl, sb, sr, 1.0f);
                    }
                    
                    
                }
                
#ifdef KK_PLATFORM_OSX
                glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA);
#else
                glBlendFunc(GL_ONE , GL_ONE_MINUS_SRC_ALPHA);
#endif
                
                context->drawTexture(v, GL_TRIANGLES, (TextureVertex *) _points.data(), (kk::Uint) _points.size());
            }
        }
        
        Float ImageElement::loadingProgress() {
            Image * v = _image.as<Image>();
            if(v == nullptr || (v->status() == ImageStatusLoaded || v->status() == ImageStatusFail)) {
                return 1.0f;
            }
            return 0.0f;
        }
        
        void ImageElement::setAnimationImage(Image * image) {
            _animationImage = image;
        }
        
    }
}
