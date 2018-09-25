//
//  GLProgramElement.cpp
//  KKGame
//
//  Created by zhanghailng on 2018/9/25.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//


#include "kk-config.h"
#include "GLProgramElement.h"

namespace kk {
    
    namespace GL {
        
        IMP_SCRIPT_CLASS_BEGIN_NOALLOC(&kk::GA::Element::ScriptClass, ProgramElement, GLProgramElement)
        
        IMP_SCRIPT_CLASS_END
        
        KK_IMP_ELEMENT_CREATE(ProgramElement)
        
        ProgramElement::ProgramElement(kk::Document * document,kk::CString name, kk::ElementKey elementId):kk::GA::Element(document,name,elementId) {
            
        }
        
        void ProgramElement::exec(kk::GA::Context * context) {
            kk::GA::Element::exec(context);
            
            kk::GL::Context * GLContext = dynamic_cast<kk::GL::Context *>(context);
            
            Program * v = program();
            
            kk::CString name = get("name");
            
            if(GLContext && v == nullptr && name) {
                
                kk::String fsh;
                kk::String vsh;
                
                kk::Element * e = firstChild();
                
                while(e) {
                    
                    kk::CString n = e->get("#name");
                    
                    if(kk::CStringEqual(n, "fsh")) {
                        fsh = e->get("#text");
                    } else if(kk::CStringEqual(n, "vsh")) {
                        vsh = e->get("#text");
                    }
                    
                    e = e->nextSibling();
                }
            
                kk::Strong vv = GLContext->program(name,vsh.c_str(),fsh.c_str());
                
                v = vv.as<Program>();
                
                _program = v;
                
                
            }
        }
        
        Program * ProgramElement::program() {
            return _program.as<Program>();
        }
        
    }
    
}
