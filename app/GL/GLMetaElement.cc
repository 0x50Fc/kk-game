//
//  GLMetaElement.cc
//  KKGame
//
//  Created by hailong11 on 2018/5/7.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include "kk-config.h"
#include "GLMetaElement.h"

namespace kk {
    
    namespace GL {
        
        IMP_SCRIPT_CLASS_BEGIN(&Element::ScriptClass, MetaElement, GLMetaElement)
        
        IMP_SCRIPT_CLASS_END
        
        MetaElement::MetaElement():_loaded(false) {
            
        }
        
        Float MetaElement::loadingProgress() {
            Image * v = _image.as<Image>();
            if(v == nullptr || (v->status() == ImageStatusLoaded || v->status() == ImageStatusFail)) {
                return 1.0f;
            }
            return 0.0f;
        }
        
        void MetaElement::onDraw(Context * context) {
            Element::onDraw(context);
            
            if(!_loaded) {
                kk::String& type = get("type");
                if(type == "image") {
                    kk::String& src = get("src");
                    if(!src.empty()) {
                        kk::Strong v = context->image(src.c_str());
                        _image = v.get();
                        kk::Log("[MEAT] %s",src.c_str());
                    }
                }
                _loaded = true;
            }
        }
    }
    
}
