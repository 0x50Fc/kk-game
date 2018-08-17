//
//  GAInCollision.cc
//  KKGame
//
//  Created by zhanghailong on 2018/8/17.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include "kk-config.h"
#include "GAShape.h"
#include "GAInCollision.h"

#if defined(KK_PLATFORM_IOS)

#include <KKObject/KKObject.h>

#else

#include "kk-string.h"

#endif

namespace kk {
    
    
    namespace GA {
    

        KK_IMP_ELEMENT_CREATE(InCollision)
        
        InCollision::InCollision(kk::Document * document,kk::CString name, kk::ElementKey elementId):kk::GA::Element(document,name,elementId) {
            
        }
        
        void InCollision::changedKey(String& key) {
            Element::changedKey(key);
            if(key == "inCollisionTypes") {
                std::vector<kk::String> vs;
                kk::CStringSplit(get(key.c_str()), " ", vs);
                _inCollisionTypes.clear();
                std::vector<kk::String>::iterator i = vs.begin();
                while(i != vs.end()) {
                    _inCollisionTypes.insert(intValue((*i).c_str()));
                    i ++;
                }
            }
        }
        
        void InCollision::addInCollisionType(kk::Int collisionType) {
            _inCollisionTypes.insert(collisionType);
        }
        
        void InCollision::removeInCollisionType(kk::Int collisionType) {
            std::set<kk::Int>::iterator i = _inCollisionTypes.find(collisionType);
            if(i != _inCollisionTypes.end()) {
                _inCollisionTypes.erase(i);
            }
        }
        
        kk::Boolean InCollision::inCollisionShape(Shape * a, Shape * b,Point n) {
            
            kk::Boolean v = true;
            
            kk::Element * e = firstChild();
            
            kk::Int collisionType = b->collisionType();
            
            while(e) {
                
                InCollision * act = dynamic_cast<InCollision *>(e);
                
                if(act && act->hasCollisionType(collisionType)) {
                    v = act->inCollisionShape(a,b,n) && v;
                }
                
                e = e->nextSibling();
            }
            
            return v;
        }
        
        void InCollision::outCollisionShape(Shape * a, Shape * b,Point n) {
            
            kk::Element * e = firstChild();
            
            kk::Int collisionType = b->collisionType();
            
            while(e) {
                
                InCollision * act = dynamic_cast<InCollision *>(e);
                
                if(act && act->hasCollisionType(collisionType)) {
                    act->outCollisionShape(a,b,n);
                }
                
                e = e->nextSibling();
            }
            
            
        }
        
        kk::Boolean InCollision::hasCollisionType(kk::Int collisionType) {
            return _inCollisionTypes.find(collisionType) != _inCollisionTypes.end();
        }
        
    }
    
}
