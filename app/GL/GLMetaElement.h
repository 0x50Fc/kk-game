//
//  GLMetaElement.h
//  KKGame
//
//  Created by hailong11 on 2018/5/7.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef GLMetaElement_h
#define GLMetaElement_h

#include "GLContext.h"

namespace kk {
    
    namespace GL {
        
        class MetaElement : public Element, public LoadingProgress {
        public:
            MetaElement();
            virtual Float loadingProgress();
            DEF_SCRIPT_CLASS
        private:
            virtual void onDraw(Context * context);
            kk::Strong _image;
            kk::Boolean _loaded;
        };
    }
    
}

#endif /* GLMetaElement_h */
