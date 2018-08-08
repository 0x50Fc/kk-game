//
//  GADocument.c
//  KKGame
//
//  Created by zhanghailong on 2018/3/12.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//


#include "kk-config.h"
#include "GADocument.h"
#include <typeinfo>

namespace kk {
    
    namespace GA {
        
        IMP_SCRIPT_CLASS_BEGIN_NOALLOC(&kk::GA::Element::ScriptClass, Document, GADocument)
        
        IMP_SCRIPT_CLASS_END
        
        KK_IMP_ELEMENT_CREATE(Document)

        Document::Document(kk::Document * document,kk::CString name, kk::ElementKey elementId):kk::GA::Element(document,name,elementId) {
            
        }
        
        Scene * Document::scene() {
            return _scene.as<Scene>();
        }
        
        void Document::onDidAddChildren(kk::Element * element) {
            Element::onDidAddChildren(element);
            Scene * v = dynamic_cast<Scene *>(element);
            if(v) {
                _scene = v;
            }
        }
        
        void Document::onWillRemoveChildren(kk::Element * element) {
            Element::onWillRemoveChildren(element);
            Scene * v = dynamic_cast<Scene *>(element);
            if(v && _scene.as<Scene>() == v) {
                _scene = (kk::Object *) nullptr;
            }
        }
        
    }
    
}
