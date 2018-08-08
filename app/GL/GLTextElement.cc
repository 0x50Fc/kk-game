//
//  GLTextElement.c
//  KKGame
//
//  Created by zhanghailong on 2018/3/6.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//


#include "kk-config.h"
#include "GLTextElement.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

namespace kk {
    
    namespace GL {
        
        IMP_SCRIPT_CLASS_BEGIN_NOALLOC(&Element::ScriptClass, TextElement, GLTextElement)
        
        IMP_SCRIPT_CLASS_END
        
        KK_IMP_ELEMENT_CREATE(TextElement)
        
        TextElement::TextElement(kk::Document * document,kk::CString name, kk::ElementKey elementId):Element(document,name,elementId),_display(false),anchor(0.5,0.5) {
            paint.fontStyle = FontStyleNormal;
            paint.fontWeight = FontWeightNormal;
            paint.fontSize = 14;
            paint.textColor = vec4(1);
            paint.textShadow = { vec4(0), 0,0,0};
            paint.maxWidth = 0.0f;
            paint.textStroke = {vec4(0), 0};
        }
        
        void TextElement::changedKey(String& key) {
            Element::changedKey(key);
            
            if(key == "font-fimlay") {
                paint.fontFimlay = get(key.c_str());
                _display = true;
            } else if(key == "font-weight") {
                kk::String v = get(key.c_str());
                if(v == "bold") {
                    paint.fontWeight =  FontWeightBold;
                } else {
                    paint.fontWeight = FontWeightNormal;
                }
                _display = true;
            } else if(key == "font-style") {
                kk::String v = get(key.c_str());
                if(v == "italic") {
                    paint.fontStyle =  FontStyleItalic;
                } else {
                    paint.fontStyle = FontStyleNormal;
                }
                _display = true;
            } else if(key == "font-size") {
                paint.fontSize = kk::GA::floatValue(get(key.c_str()));
                _display = true;
            } else if(key == "color") {
                paint.textColor = colorValue(get(key.c_str()));
                _display = true;
            } else if(key == "text-shadow") {
                paint.textShadow = shadowValue(get(key.c_str()));
                _display = true;
            } else if(key == "text-stroke") {
                paint.textStroke = strokeValue(get(key.c_str()));
                _display = true;
            } else if(key == "#text") {
                _display = true;
            } else if(key == "anchor-x") {
                anchor.x = kk::GA::floatValue(get(key.c_str()));
            } else if(key == "anchor-y") {
                anchor.y = kk::GA::floatValue(get(key.c_str()));
            }
        }
        
        void TextElement::onDraw(Context * context) {
            Element::onDraw(context);
            
            kk::CString text = get("#text");
            
            Texture * v = _texture.as<Texture>();
            
            if(_display && text != nullptr) {
                
                if(v == nullptr) {
                    v = new Texture();
                    _texture = v;
                }
                
                ContextGetStringTexture(context, v, text, paint);
            }
            
            if(v != nullptr && text != nullptr) {
                
                vec4 dest(0);
                vec4 src(0,0,v->width(),v->height());
                
                dest.x = - v->width() * anchor.x;
                dest.y = - v->height() * anchor.y;
                dest.z = v->width();
                dest.w = v->height();
                
                context->drawTexture(v, dest, src,0);
            }
            
            _display = false;
            
        }
        
    }
}

