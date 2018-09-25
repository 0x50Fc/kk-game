//
//  GLProgramElement.h
//  KKGame
//
//  Created by zhanghailong on 2018/9/25.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef GLProgramElement_h
#define GLProgramElement_h


#include "GLContext.h"

namespace kk {
    
    namespace GL {
        
        class ProgramElement : public kk::GA::Element {
        public:
            
            
            KK_DEF_ELEMENT_CREATE(ProgramElement)
            DEF_SCRIPT_CLASS_NOALLOC
            
            virtual Program * program();
            
        private:
            virtual void exec(kk::GA::Context * context);
            kk::Strong _program;
        };
    }
    
}

#endif /* GLProgramElement_h */
