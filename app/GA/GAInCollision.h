//
//  GAInCollision.h
//  KKGame
//
//  Created by zhanghailong on 2018/8/17.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef GAInCollision_h
#define GAInCollision_h

#include "GAContext.h"

namespace kk {
    
    namespace GA {
        
        class Shape;
        
        class InCollision : public Element {
        public:
   
            virtual void changedKey(String& key);
            virtual void addInCollisionType(kk::Int collisionType);
            virtual void removeInCollisionType(kk::Int collisionType);
            virtual kk::Boolean inCollisionShape(Shape * a, Shape * b,Point n);
            virtual void outCollisionShape(Shape * a, Shape * b,Point n);
            virtual kk::Boolean hasCollisionType(kk::Int collisionType);
           
            KK_DEF_ELEMENT_CREATE(InCollision)
            
        protected:
            std::set<kk::Int> _inCollisionTypes;
        };
        
    }
    
}

#endif /* GAInCollision_h */
