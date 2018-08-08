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
        
        IMP_SCRIPT_CLASS_BEGIN_NOALLOC(&Element::ScriptClass, MetaElement, GLMetaElement)
        
        IMP_SCRIPT_CLASS_END
        
        KK_IMP_ELEMENT_CREATE(MetaElement)
        
        MetaElement::MetaElement(kk::Document * document,kk::CString name, kk::ElementKey elementId):Element(document,name,elementId),_loaded(false) {
            
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
                kk::String type = get("type");
                if(type == "image") {
                    kk::CString src = get("src");
                    if(src != nullptr) {
                        kk::Strong v = context->image(src);
                        _image = v.get();
                        kk::Log("[MEAT] %s",src);
                    }
                }
                _loaded = true;
            }
        }
    }
    
}
