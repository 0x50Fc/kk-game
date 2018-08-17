//
//  GACollision.cc
//  KKGame
//
//  Created by zhanghailong on 2018/8/17.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//


#include "kk-config.h"
#include "GAShape.h"
#include "GACollision.h"

#if defined(KK_PLATFORM_IOS)

#include <KKObject/KKObject.h>

#else

#include "kk-string.h"

#endif

namespace kk {
    
    
    namespace GA {
        
        IMP_SCRIPT_CLASS_BEGIN_NOALLOC(&kk::GA::Element::ScriptClass, Collision, GACollision)
        
        IMP_SCRIPT_CLASS_END
        
        KK_IMP_ELEMENT_CREATE(Collision)
        
        Collision::Collision(kk::Document * document,kk::CString name, kk::ElementKey elementId):kk::GA::InCollision(document,name,elementId),_collisionType(0) {
            
        }
        
        void Collision::changedKey(String& key) {
            InCollision::changedKey(key);
            if(key == "collisionType") {
                _collisionType = intValue(get(key.c_str()));
            }
        }
        
        kk::Int Collision::collisionType() {
            return _collisionType;
        }
        
        void Collision::setCollisionType(kk::Int collisionType) {
            _collisionType = collisionType;
        }
        
        kk::Boolean Collision::inCollisionShape(Shape * a, Shape * b,Point n) {
            
            kk::Boolean v = InCollision::inCollisionShape(a, b,n);
            
            if(has("in")) {
                
                ElementEvent * e = new ElementEvent();
                
                Strong ee = e;
                
                e->element = this;
                
                setObject("a", a);
                setObject("b", b);
                
                emit("in", e);
                
                if(e->cancelBubble) {
                    v = false;
                }
            }
            
            return v;
        }
        
        void Collision::outCollisionShape(Shape * a, Shape * b,Point n) {
            
            InCollision::outCollisionShape(a, b,n);
            
            if(has("out")) {
                
                ElementEvent * e = new ElementEvent();
                
                Strong ee = e;
                
                e->element = this;
                
                setObject("a", a);
                setObject("b", b);
                
                emit("out", e);
                
            }
            
        }

    }
    
}
