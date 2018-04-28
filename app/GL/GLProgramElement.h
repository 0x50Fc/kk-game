//
//  GLProgramElement.h
//  KKGame
//
//  Created by hailong11 on 2018/4/25.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef GLProgramElement_h
#define GLProgramElement_h

#include "GLContext.h"
#include "yaml.h"
#include <set>

namespace kk {
    
    namespace GL {
        
        
        enum ProgramPropertyType {
            ProgramPropertyTypeUniform,
            ProgramPropertyTypeAttrib
        };
        
        class ProgramProperty : public kk::Object {
        public:
            ProgramPropertyType type;
            kk::Int loc;
            kk::String name;
            virtual void set(Context * context,CString value);
            virtual void set(Program * program);
            virtual void set(Context * context,Program * program,kk::String & basePath, yaml_document_t * document,kk::CString name,yaml_node_t * node);
            virtual bool isAvailable();
        };
        
        class ProgramFloatProperty : public ProgramProperty {
        public:
            kk::Strong data;
            kk::Uint size;
            std::vector<Float> value;
            virtual void set(Program * program);
            virtual void set(Context * context,Program * program,kk::String & basePath,yaml_document_t * document,kk::CString name,yaml_node_t * node);
        };
        
        class ProgramImageProperty : public ProgramProperty {
        public:
            kk::Strong image;
            virtual void set(Context * context,CString value);
            virtual void set(Program * program);
            virtual void set(Context * context,Program * program,kk::String & basePath,yaml_document_t * document,kk::CString name,yaml_node_t * node);
            virtual bool isAvailable();
        };
        
        class ProgramElement : public Element {
        public:
            ProgramElement();
            virtual ~ProgramElement();
            virtual void changedKey(String& key);
        
            DEF_SCRIPT_CLASS
        protected:
            virtual void onDraw(Context * context);
            kk::Int _projection;
            kk::Int _view;
            kk::Strong _program;
            std::map<kk::String,kk::Strong> _propertys;
            GLenum _mode;
            GLboolean _depthMask;
            GLsizei _count;
            std::set<kk::String> _updateKeys;
            GLuint _vao;
        };
        
    }
    
}

#endif /* GLProgramElement_hpp */
