//
//  GLSliceMapElement.h
//  KKGame
//
//  Created by hailong11 on 2018/2/24.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef GLSliceMapElement_h
#define GLSliceMapElement_h

#include "GLContext.h"

namespace kk {
    
    namespace GL {
        
        class SliceMapElement : public Element , public LoadingProgress {
        public:
            SliceMapElement();
            virtual CString path();
            virtual void setPath(CString path);
            virtual void changedKey(String& key);
            virtual Float loadingProgress();
            
            vec2 size;
            vec2 slice;
            
            DEF_SCRIPT_CLASS
            
        protected:
            virtual void onDraw(Context * context);
        protected:
            String _path;
            std::map<String,Strong> _images;
        };
    }
}

#endif /* GLSliceMapElement_h */
