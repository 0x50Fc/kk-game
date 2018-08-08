//
//  GLSliceMapElement.h
//  KKGame
//
//  Created by zhanghailong on 2018/2/24.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef GLSliceMapElement_h
#define GLSliceMapElement_h

#include "GLContext.h"

namespace kk {
    
    namespace GL {
        
        class SliceMapElement : public Element , public LoadingProgress {
        public:

            virtual CString path();
            virtual void setPath(CString path);
            virtual void changedKey(String& key);
            virtual Float loadingProgress();
            
            vec2 size;
            vec2 slice;
            
            KK_DEF_ELEMENT_CREATE(SliceMapElement)
            DEF_SCRIPT_CLASS_NOALLOC
            
        protected:
            virtual void onDraw(Context * context);
        protected:
            String _path;
            std::map<String,Strong> _images;
        };
    }
}

#endif /* GLSliceMapElement_h */
