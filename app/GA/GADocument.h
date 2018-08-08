//
//  GADocument.h
//  KKGame
//
//  Created by zhanghailong on 2018/3/12.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef GADocument_h
#define GADocument_h

#include "GAContext.h"
#include "GAScene.h"

namespace kk {
    
    namespace GA {
        
        class Document : public Element , public IScene {
            
        public:

            virtual Scene * scene();
            virtual void onDidAddChildren(kk::Element * element);
            virtual void onWillRemoveChildren(kk::Element * element);
            
            DEF_SCRIPT_CLASS_NOALLOC
            
            KK_DEF_ELEMENT_CREATE(Document)
            
        protected:
            Strong _scene;
        };
        
    }
    
}


#endif /* GADocument_h */
