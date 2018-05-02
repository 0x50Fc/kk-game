//
//  GLTileMapElement.h
//  KKGame
//
//  Created by zhanghailong on 2018/4/25.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef GLTileMapElement_h
#define GLTileMapElement_h

#include "GLContext.h"
#include "GATileMap.h"

namespace kk {
    
    namespace GL {
        

        class TileMapElement : public Element {
        public:
            TileMapElement();
            virtual void onDraw(Context * context);
            virtual kk::GA::TileMap * tileMap();
            virtual Image * image(Context * context,kk::CString path);
            DEF_SCRIPT_CLASS
        protected:
            std::map<kk::String,kk::Strong> _images;
        };
    }
    
}


#endif /* GLTileMapElement_h */
