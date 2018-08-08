//
//  GLAnimation.cpp
//  KKGame
//
//  Created by zhanghailong on 2018/2/13.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include "kk-config.h"
#include "GLAnimation.h"

namespace kk {
    
    namespace GL {
        
        IMP_SCRIPT_CLASS_BEGIN_NOALLOC(&kk::GA::Element::ScriptClass, Animation, GLAnimation)
        
        IMP_SCRIPT_CLASS_END
        
        KK_IMP_ELEMENT_CREATE(Animation)
        
        Animation::Animation(kk::Document * document,kk::CString name, kk::ElementKey elementId)
            :kk::GA::Element(document,name,elementId)
            ,animationState(AnimationStateNone)
            ,afterDelay(0)
            ,duration(0)
            ,start(0)
            ,repeatCount(0)
            ,autoreverses(false)
            ,enabled(true){
            
        }
        
        void Animation::exec(kk::GA::Context * context) {
            kk::GA::Element::exec(context);
            
            Context * v = dynamic_cast<Context *>(context);
            
            if(v) {
                onExec(v);
            }
        }
        
        void Animation::onExec(Context * context) {
            
            if(!enabled || duration <= 0 ) {
                return;
            }
            
            while(true) {
                
                if(animationState == AnimationStateNone) {
                    start = context->current();
                    _count = 0;
                    animationState = AnimationStateWait;
                }
                
                kk::GA::TimeInterval dt = context->current() - start;
                
                if(animationState == AnimationStateWait && dt >= afterDelay) {
                    animationState = AnimationStateExecute;
                    if(_count == 0) {
                        onStart(context);
                    }
                }
                
                if(duration > 0 && dt > duration + afterDelay) {
                    
                    _count ++ ;
                    
                    if(repeatCount <0 || _count <= repeatCount) {
                        start = context->current();
                        animationState = AnimationStateWait;
                        continue;
                    }
                    
                    onExecute(context, 1.0f);
                    
                    _count = 0;
                    animationState = AnimationStateNone;
                    enabled = false;
                    onDone(context);
                }
                
                if(animationState == AnimationStateExecute) {
                    Float v = (Float) (dt - afterDelay) / duration;
                    if(autoreverses && _count % 2 == 1) {
                        v = (1.0f - v);
                    }
                    onExecute(context, v);
                }
                
                break;
            }
            
            
        }
        
        AnimationTarget * Animation::target() {
            return dynamic_cast<AnimationTarget *>(parent());
        }
        
        void Animation::onDone(Context * context) {
            
        }
        
        void Animation::onStart(Context * context) {
        
        }
        
        void Animation::onExecute(Context * context,Float value) {
            
            AnimationTarget * e = target();
            
            if(e == nullptr) {
                return ;
            }
            
            Float idx = 100 * value;
            Float cur = 0;
            
            AnimationItem * prev = nullptr, * n = nullptr;
            kk::Element * p = firstChild();
            
            while(p) {
                
                n = dynamic_cast<AnimationItem *>(p);
                
                if(n) {
                    
                    cur = n->value;
                    
                    if(cur > idx) {
                        break;
                    }
                    
                    prev = n;
                }
                
                p = p->nextSibling();
            }
            
            if(n == nullptr) {
                return;
            }
            
            if(prev == nullptr) {
                e->setAnimationOpacity(n->opacity);
                e->setAnimationTransform(n->transform);
                e->setAnimationImage(n->image(context));
            } else {
                Float bv = prev->value;
                Float ev = cur;
                if(ev - bv == 0) {
                    e->setAnimationOpacity(n->opacity);
                    e->setAnimationTransform(n->transform);
                    e->setAnimationImage(n->image(context));
                } else {
                    Float v = (idx - bv) / (ev - bv);
                    e->setAnimationOpacity(prev->opacity + (n->opacity - prev->opacity) * v);
                    e->setAnimationTransform(prev->transform + (n->transform - prev->transform) * v);
                    if(v >= 1.0f) {
                        e->setAnimationImage(n->image(context));
                    } else {
                        e->setAnimationImage(prev->image(context));
                    }
                }
            }
            
        }

        void Animation::changedKey(String& key) {
            kk::GA::Element::changedKey(key);
            
            if(key == "enabled") {
                kk::Boolean v = kk::GA::booleanValue(get(key.c_str()));
                if(v != enabled) {
                    enabled = v;
                    animationState = AnimationStateNone;
                    if(!enabled) {
                        AnimationTarget * e = target();
                        if(e) {
                            e->setAnimationTransform(mat4(1.0f));
                            e->setAnimationImage(nullptr);
                        }
                    }
                }
                
            } else if(key == "duration") {
                duration = kk::GA::floatValue(get(key.c_str()));
            } else if(key == "after-delay") {
                afterDelay = kk::GA::floatValue(get(key.c_str()));
            } else if(key == "repeat-count") {
                repeatCount = kk::GA::intValue(get(key.c_str()));
            } else if(key == "autoreverses") {
                autoreverses = kk::GA::booleanValue(get(key.c_str()));
            }
        }
        
        IMP_SCRIPT_CLASS_BEGIN_NOALLOC(&kk::StyleElement::ScriptClass, AnimationItem, GLAnimationItem)
        
        IMP_SCRIPT_CLASS_END
        
        KK_IMP_ELEMENT_CREATE(AnimationItem)
        
        AnimationItem::AnimationItem(kk::Document * document,kk::CString name, kk::ElementKey elementId):kk::StyleElement(document,name,elementId),transform(1.0f),value(0),opacity(1) {
            
        }
        
        void AnimationItem::changedKey(String& key) {
            kk::StyleElement::changedKey(key);
            
            if(key == "transform") {
                transform = TransformForString(get(key.c_str()));
            } else if(key == "value") {
                value = kk::GA::floatValue(get(key.c_str()));
            } else if(key == "image") {
                _image = (kk::Object *) nullptr;
            } else if(key == "opacity") {
                opacity = kk::GA::floatValue(get(key.c_str()));
            }
        }
        
        Image * AnimationItem::image(Context * context) {
            
            kk::CString v = get("image");
            
            if(v != nullptr) {
                
                Image * image = _image.as<Image>();
                
                if(image == nullptr) {
                    Strong vv = context->image(v);
                    image = vv.as<Image>();
                    _image = image;
                }
                
                return image;
                
            }
            
            return nullptr;
        }
        
        Float AnimationItem::loadingProgress() {
            Image * v = _image.as<Image>();
            if(v == nullptr || (v->status() == ImageStatusLoaded || v->status() == ImageStatusFail)) {
                return 1.0f;
            }
            return 0.0f;
        }
        
    }
    
}
