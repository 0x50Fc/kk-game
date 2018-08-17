//
//  GACollision.h
//  KKGame
//
//  Created by zhanghailong on 2018/8/17.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef GACollision_h
#define GACollision_h


#include "GAContext.h"
#include "GAInCollision.h"

namespace kk {
    
    namespace GA {
        
        class Shape;
        
        class Collision : public InCollision {
        public:
            
            virtual void changedKey(String& key);
            
            virtual kk::Int collisionType();
            virtual void setCollisionType(kk::Int collisionType);
            virtual kk::Boolean inCollisionShape(Shape * a, Shape * b,Point n);
            virtual void outCollisionShape(Shape * a, Shape * b,Point n);

            DEF_SCRIPT_CLASS_NOALLOC
            
            KK_DEF_ELEMENT_CREATE(Collision)
            
        protected:
            kk::Int _collisionType;
        };
        
    }
    
}


#endif /* GACollision_h */
