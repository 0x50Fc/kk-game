//
//  GLViewElement.cc
//  KKGame
//
//  Created by zhanghailong on 2018/9/25.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//


#include "kk-config.h"
#include "GLViewElement.h"
#include "GLViewportElement.h"

namespace kk {
    
    namespace GL {
        
        
        
        IMP_SCRIPT_CLASS_BEGIN_NOALLOC(&Element::ScriptClass, ViewElement, GLViewElement)
        
        IMP_SCRIPT_CLASS_END
        
        KK_IMP_ELEMENT_CREATE(ViewElement)
        
        ViewElement::ViewElement(kk::Document * document,kk::CString name, kk::ElementKey elementId):Element(document,name,elementId)
            ,_left(Auto),_top(Auto),_right(Auto),_bottom(Auto),_width(Auto),_height(Auto)
        {
            
        }
        
        void ViewElement::exec(kk::GA::Context * context) {
            Element::exec(context);
            
            kk::Float width = Auto;
            kk::Float height = Auto;
            
            kk::Element * p = parent();
            
            ViewportElement * viewport = nullptr;
            
            while(p) {
                
                {
                    ViewportElement * v = dynamic_cast<ViewportElement *>(p);
                    if(v) {
                        viewport = v;
                        if(width == Auto) {
                            width = context->viewport().width;
                        }
                        if(height == Auto) {
                            height = context->viewport().height;
                        }
                        break;
                    }
                }
                
                {
                    ViewElement * v = dynamic_cast<ViewElement *>(p);
                    
                    if(v) {
                        if(width == Auto) {
                            width = v->_width;
                        }
                        if(height == Auto) {
                            height = v->_height;
                        }
                        break;
                    }
                }
                
                p = p->parent();
            }
            
            if(viewport) {
                
                kk::Float w = _width == Auto ? 0 : _width;
                kk::Float h = _height == Auto ? 0 : _height;
                kk::Float dx = w * 0.5f;
                kk::Float dy = h * 0.5f;
                
                if(_left == Auto) {
                    
                    if(_right == Auto) {
                        position.x = (width - w) * 0.5f + dx;
                    } else {
                        position.x = width - w - _right + dx;
                    }
                    
                } else {
                    position.x = _left + dx;
                }
                
                if(_top == Auto) {
                    
                    if(_bottom == Auto) {
                        position.y = (height - h) * 0.5f + dy;
                    } else {
                        position.y = height - h - _bottom + dy;
                    }
                    
                } else {
                    position.y = _top + dy;
                }
                
            }
            
            
        }
        
        void ViewElement::changedKey(String& key) {
            Element::changedKey(key);
            
            kk::CString v = get(key.c_str());
            
            if(key == "left") {
                if(v == nullptr || kk::CStringEqual(v, "auto")) {
                    _left = Auto;
                } else {
                    _left = kk::GA::floatValue(v);
                }
            } else if(key == "top") {
                if(v == nullptr || kk::CStringEqual(v, "auto")) {
                    _top = Auto;
                } else {
                    _top = kk::GA::floatValue(v);
                }
            } else if(key == "right") {
                if(v == nullptr || kk::CStringEqual(v, "auto")) {
                    _right = Auto;
                } else {
                    _right = kk::GA::floatValue(v);
                }
            } else if(key == "bottom") {
                if(v == nullptr || kk::CStringEqual(v, "auto")) {
                    _bottom = Auto;
                } else {
                    _bottom = kk::GA::floatValue(v);
                }
            } else if(key == "width") {
                if(v == nullptr || kk::CStringEqual(v, "auto")) {
                    _width = Auto;
                } else {
                    _width = kk::GA::floatValue(v);
                }
            } else if(key == "height") {
                if(v == nullptr || kk::CStringEqual(v, "auto")) {
                    _height = Auto;
                } else {
                    _height = kk::GA::floatValue(v);
                }
            }
        }
        
        ViewElement * ViewElement::findView(Float x,Float y) {
            
            Float dx = x - position.x;
            Float dy = y - position.y;
            
            if(_width != Auto && (dx < - 0.5f * _width || dx > 0.5f * _width)) {
                return nullptr;
            }
            
            if(_height != Auto && (dy < - 0.5f * _height || dy > 0.5f * _height)) {
                return nullptr;
            }
            
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
            
            if(r == nullptr) {
                return this;
            }
            
            return r;
        }

        
    }
    
}

