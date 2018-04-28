//
//  GLImageElement.h
//  KKGame
//
//  Created by hailong11 on 2018/2/6.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef GLImageElement_h
#define GLImageElement_h

#include "GLContext.h"

namespace kk {
    
    namespace GL {
        
        class ImageElement : public Element, public LoadingProgress {
        public:
            ImageElement();
            virtual kk::CString src();
            virtual void setSrc(kk::CString src);
            virtual void changedKey(String& key);
            virtual void onDraw(Context * context);
            virtual void setAnimationImage(Image * image);
            Float loadingProgress();
            vec2 size;
            vec2 anchor; // 0~1.0
            Float capLeft;
            Float capTop;
            Float capRight;
            Float capBottom;
            
            DEF_SCRIPT_CLASS
        private:
            kk::String _src;
            Strong _animationImage;
            Strong _image;
            std::vector<TextureVertex> _points;
            void addRect(Float l,Float t,Float r,Float b,Float sl,Float st,Float sr,Float sb);
        };
    }
    
}

#endif /* GLImageElement_h */
