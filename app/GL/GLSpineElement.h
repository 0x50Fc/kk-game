//
//  GLSpineElement.hpp
//  KKGame
//
//  Created by zhanghailong on 2018/5/2.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef GLSpineElement_h
#define GLSpineElement_h

#include "GLContext.h"

struct spAtlas;
struct spSkeletonData;
struct spSkeleton;
struct spAnimationStateData;
struct spAnimationState;
struct spSkeletonClipping;

namespace kk {
    
    namespace GL {
        
        class SpineElement : public Element {
        public:
            SpineElement();
            virtual ~SpineElement();
            virtual void changedKey(String& key);
            virtual Image * image(kk::CString path);
            DEF_SCRIPT_CLASS
        protected:
            virtual void onDraw(Context * context);
            kk::Boolean _loaded;
            kk::Boolean _updatting;
            struct spAtlas * _spAtlas;
            struct spSkeletonData * _spSkeletonData;
            struct spSkeleton * _spSkeleton;
            struct spAnimationStateData * _spAnimationStateData;
            struct spAnimationState * _spAnimationState;
            struct spSkeletonClipping * _spClipping;
            Context * _context;
            std::map<kk::String,kk::Strong> _images;
            kk::GA::TimeInterval _prevTimeInterval;
        };
        
    }
    
}

#endif /* GLSpineElement_hpp */
