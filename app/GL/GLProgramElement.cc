//
//  GLProgramElement.cc
//  KKGame
//
//  Created by hailong11 on 2018/4/25.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include "kk-config.h"
#include "GLProgramElement.h"
#include "kk-string.h"


namespace kk {
    
    namespace GL {
        
        IMP_SCRIPT_CLASS_BEGIN(&Element::ScriptClass, ProgramElement, GLProgramElement)
        
        IMP_SCRIPT_CLASS_END
        
        bool ProgramProperty::isAvailable() {
            return true;
        }
        
        void ProgramProperty::set(Program * program) {
            
        }
        
        void ProgramProperty::set(Context * context,CString value) {
            
        }
        
        void ProgramProperty::set(Context * context,Program * program,kk::String & basePath,yaml_document_t * document,kk::CString name,yaml_node_t * node) {
            yaml_node_pair_t * p = node->data.mapping.pairs.start;
            
            while(p && p != node->data.mapping.pairs.top) {
                
                yaml_node_t * key = yaml_document_get_node(document, p->key);
                yaml_node_t * value = yaml_document_get_node(document, p->value);
                
                if(key && value && key->type ==YAML_SCALAR_NODE && value->type == YAML_SCALAR_NODE) {
                    if(kk::CStringEqual((kk::CString) key->data.scalar.value, "type")) {
                        if(kk::CStringEqual((kk::CString) value->data.scalar.value, "attrib")) {
                            type = ProgramPropertyTypeAttrib;
                            loc = program->attrib(name);
                        }
                        else {
                            type = ProgramPropertyTypeUniform;
                            loc = program->uniform(name);
                        }
                        break;
                    } else if(kk::CStringEqual((kk::CString) key->data.scalar.value, "name")) {
                        name = (kk::CString) value->data.scalar.value;
                    }
                }
                p ++;
            }
            
        }
        
        void ProgramFloatPropertySetValue(Context * context,Program * program,yaml_document_t * document,yaml_node_t * node,kk::Uint size,std::vector<Float> & data) {
            
            std::vector<kk::String> vs;
            
            yaml_node_item_t * p = node->data.sequence.items.start;
            
            while(p && p != node->data.sequence.items.top) {
                
                yaml_node_t * item = yaml_document_get_node(document, * p);
                
                if(item && item->type ==YAML_SCALAR_NODE ) {
                    
                    vs.clear();
                    
                    kk::CStringSplit((kk::CString)item->data.scalar.value, " ", vs);
                    
                    kk::Uint n =0;
                    std::vector<kk::String>::iterator i = vs.begin();
                    
                    while(i != vs.end() && n < size) {
                        Float v = atof((*i).c_str());
                        data.push_back(v);
                        i ++;
                        n ++;
                    }
                    
                }
                p ++;
            }
            
        }
        
        void ProgramFloatProperty::set(Context * context,Program * program,kk::String & basePath,yaml_document_t * document,kk::CString name,yaml_node_t * node) {
            ProgramProperty::set(context, program, basePath, document, name, node);
            
            yaml_node_pair_t * p = node->data.mapping.pairs.start;
        
            while(p && p != node->data.mapping.pairs.top) {
                
                yaml_node_t * key = yaml_document_get_node(document, p->key);
                yaml_node_t * value = yaml_document_get_node(document, p->value);
                
                if(key && value && key->type ==YAML_SCALAR_NODE) {
                    if(kk::CStringEqual((kk::CString) key->data.scalar.value, "size") && value->type == YAML_SCALAR_NODE) {
                        size = atoi((kk::CString) value->data.scalar.value);
                    } else if(kk::CStringEqual((kk::CString) key->data.scalar.value, "value") && value->type == YAML_SEQUENCE_NODE) {
                        ProgramFloatPropertySetValue(context,program,document,value,size,this->value);
                    }
                }
                p ++;
            }
            
            if(type == ProgramPropertyTypeAttrib) {
                this->data = new Buffer(this->value.size() * sizeof(Float),this->value.data(),GL_STATIC_DRAW);
                this->value.clear();
            }
            
        }
        
        void ProgramFloatProperty::set(Program * program) {
            
            if(type == ProgramPropertyTypeAttrib) {
                
                Buffer * v = data.as<Buffer>();
                
                if(v == nullptr) {
                    return;
                }
                
                v->bind();
            
                program->setAttrib(loc, size, NULL, size * sizeof(Float));
                
                Buffer::unbind();
                
            } else {
                program->setUniform(loc, value.data(), (kk::Int) value.size());
            }
        
        }
        
        void ProgramImageProperty::set(Context * context,Program * program,kk::String & basePath,yaml_document_t * document,kk::CString name,yaml_node_t * node) {
            ProgramProperty::set(context, program,basePath, document, name, node);
            
            yaml_node_pair_t * p = node->data.mapping.pairs.start;
            
            while(p && p != node->data.mapping.pairs.top) {
                
                yaml_node_t * key = yaml_document_get_node(document, p->key);
                yaml_node_t * value = yaml_document_get_node(document, p->value);
                
                if(key && value && key->type ==YAML_SCALAR_NODE && value->type == YAML_SCALAR_NODE) {
                    if(kk::CStringEqual((kk::CString) key->data.scalar.value, "image")) {
                        kk::String path = basePath + (kk::CString) value->data.scalar.value;
                        kk::Strong v = context->image(path.c_str());
                        image = v.get();
                        break;
                    }
                }
                p ++;
            }
            
        }
        
        bool ProgramImageProperty::isAvailable() {
            Image * v = image.as<Image>();
            return v && v->status() == ImageStatusLoaded;
        }
        
        void ProgramImageProperty::set(Program * program) {
            Image * v = image.as<Image>();
            
            if(v && v->status() == ImageStatusLoaded) {
                v->active(GL_TEXTURE0);
                program->setUniform(loc, kk::Int(0));
            }
        }
        
        
        void ProgramImageProperty::set(Context * context,CString value) {
            kk::Strong v = context->image(value);
            image = v.get();
        }
        
        ProgramElement::ProgramElement():_mode(GL_TRIANGLES),_depthMask(GL_FALSE),_count(0),_vao(0) {
            glGenVertexArraysOES(1, &_vao);
        }
        
        ProgramElement::~ProgramElement() {
            glDeleteVertexArraysOES(1, &_vao);
        }
        
        void ProgramElement::changedKey(String& key) {
            Element::changedKey(key);
            if(_program.get() != nullptr) {
                _updateKeys.insert(key);
            }
        }
        
        static kk::Strong ProgramElementGetProgram(Context * context,yaml_document_t * document, yaml_node_t * node) {
            
            if(node->type == YAML_SCALAR_NODE) {
                return context->program((kk::CString) node->data.scalar.value);
            } else if(node->type == YAML_MAPPING_NODE) {
                kk::CString vsh = nullptr;
                kk::CString fsh = nullptr;
                
                yaml_node_pair_t * p = node->data.mapping.pairs.start;
                while(p && p != node->data.mapping.pairs.top) {
                    yaml_node_t * key = yaml_document_get_node(document, p->key);
                    yaml_node_t * value = yaml_document_get_node(document, p->value);
                    if(key && value && key->type ==YAML_SCALAR_NODE && value->type == YAML_SCALAR_NODE) {
                        if(kk::CStringEqual((kk::CString) key->data.scalar.value, "vsh")) {
                            vsh = (kk::CString) value->data.scalar.value;
                        } else if(kk::CStringEqual((kk::CString) key->data.scalar.value, "fsh")) {
                            fsh = (kk::CString) value->data.scalar.value;
                        }
                        if(vsh && fsh) {
                            break;
                        }
                    }
                    p ++;
                }
                
                if(vsh && fsh) {
                    return new Program(vsh,fsh);
                }
                
            }
            
            return (kk::Object *) nullptr;
        }
        
        static kk::Strong ProgramElementGetProperty(kk::Element * element,Context * context,Program * program,kk::String & basePath,yaml_document_t * document, kk::CString name ,yaml_node_t * node) {
            
            ProgramProperty * v = nullptr;
            
            yaml_node_pair_t * p = node->data.mapping.pairs.start;
            while(p && p != node->data.mapping.pairs.top) {
                
                yaml_node_t * key = yaml_document_get_node(document, p->key);
                yaml_node_t * value = yaml_document_get_node(document, p->value);
                
                if(key && value && key->type ==YAML_SCALAR_NODE && value->type == YAML_SCALAR_NODE) {
                    if(kk::CStringEqual((kk::CString) key->data.scalar.value, "class")) {
                        if(kk::CStringEqual((kk::CString) value->data.scalar.value, "image")) {
                            v = new ProgramImageProperty();
                            v->set(context, program, basePath, document, name, node);
                            break;
                        } else {
                            v = new ProgramFloatProperty();
                            v->set(context, program, basePath, document, name, node);
                            break;
                        }
                    }
                }
                p ++;
            }
            
            if(v != nullptr) {
                
                if(v->name.empty()) {
                    v->name = name;
                }
                
                kk::String& vv = element->get(v->name);
                
                if(&vv != &kk::Element::NotFound) {
                    v->set(context,vv.c_str());
                }
            }
            
            return v;
        }
        
        static void ProgramElementGetPropertys(kk::Element * element,Context * context,Program * program, kk::String& basePath,yaml_document_t * document, yaml_node_t * node,std::map<kk::String,kk::Strong>& propertys) {
            
            yaml_node_pair_t * p = node->data.mapping.pairs.start;
            while(p && p != node->data.mapping.pairs.top) {
                
                yaml_node_t * key = yaml_document_get_node(document, p->key);
                yaml_node_t * value = yaml_document_get_node(document, p->value);
                
                if(key && value && key->type ==YAML_SCALAR_NODE && value->type == YAML_MAPPING_NODE) {
                    kk::Strong v = ProgramElementGetProperty(element,context,program,basePath,document,(kk::CString) key->data.scalar.value, value);
                    ProgramProperty * p = v.as<ProgramProperty>();
                    if(p != nullptr) {
                        propertys[p->name] = v.get();
                    }
                }
                p ++;
            }
            
        }
        
        void ProgramElement::onDraw(Context * context) {
            Element::onDraw(context);
            
            Strong vv;
            
            Program * v = _program.as<Program>();
            
            if(v == nullptr) {
                
                kk::String& path = get("path");
                
                if(path.empty()) {
                    return;
                }
        
                kk::String basePath = path;
                
                size_t i = path.find_last_of("/");
                
                if(i != kk::String::npos) {
                    basePath = path.substr(0,i+1);
                }
                
                yaml_parser_t ps;
                
                yaml_parser_initialize(&ps);
                
                kk::String p = context->absolutePath(path.c_str());
                
                FILE * fd = nullptr;
                
                fd = fopen(p.c_str(), "rb");
                
                if(fd != nullptr) {
                    yaml_parser_set_input_file(&ps, fd);
                }
                
                if(fd) {

                    yaml_document_t document;
                    
                    if(yaml_parser_load(&ps, &document)) {
                        
                        yaml_node_t * n = yaml_document_get_root_node(&document);
                        
                        if(n && n->type == YAML_MAPPING_NODE) {
                            yaml_node_pair_t * p = n->data.mapping.pairs.start;
                            while(p && p != n->data.mapping.pairs.top) {
                                yaml_node_t * key = yaml_document_get_node(&document, p->key);
                                yaml_node_t * value = yaml_document_get_node(&document, p->value);
                                if(key && value
                                   && key->type == YAML_SCALAR_NODE) {
                                    if(kk::CStringEqual((kk::CString) key->data.scalar.value, "program")) {
                                        Strong t = ProgramElementGetProgram(context,&document,value);
                                        v = t.as<Program>();
                                        vv = v;
                                    } else if(kk::CStringEqual((kk::CString)key->data.scalar.value, "propertys")
                                              && value->type == YAML_MAPPING_NODE && v) {
                                        ProgramElementGetPropertys(this,context,v,basePath,&document,value,_propertys);
                                    } else if(kk::CStringEqual((kk::CString) key->data.scalar.value, "mode")
                                              && value->type == YAML_SCALAR_NODE) {
                                        if(kk::CStringEqual((kk::CString) value->data.scalar.value, "points")) {
                                            _mode = GL_POINTS;
                                        } else if(kk::CStringEqual((kk::CString) value->data.scalar.value, "lines")) {
                                            _mode = GL_LINES;
                                        } else if(kk::CStringEqual((kk::CString) value->data.scalar.value, "line-pool")) {
                                            _mode = GL_LINE_LOOP;
                                        } else if(kk::CStringEqual((kk::CString) value->data.scalar.value, "line-strip")) {
                                            _mode = GL_LINE_STRIP;
                                        } else if(kk::CStringEqual((kk::CString) value->data.scalar.value, "triangle-strip")) {
                                            _mode = GL_TRIANGLE_STRIP;
                                        } else if(kk::CStringEqual((kk::CString) value->data.scalar.value, "triangle-fan")) {
                                            _mode = GL_TRIANGLE_FAN;
                                        } else {
                                            _mode = GL_TRIANGLES;
                                        }
                                    } else if(kk::CStringEqual((kk::CString) key->data.scalar.value, "depth")
                                                && value->type == YAML_SCALAR_NODE) {
                                        _depthMask = kk::CStringEqual((kk::CString) value->data.scalar.value, "true");
                                    } else if(kk::CStringEqual((kk::CString) key->data.scalar.value, "count")
                                              && value->type == YAML_SCALAR_NODE) {
                                        _count = atoi((kk::CString) value->data.scalar.value);
                                    }
                                    
                                }
                                p ++;
                            }
                        }
                        
                    } else if(ps.error != YAML_NO_ERROR) {
                        kk::Log("[%d] %s",ps.error,ps.problem);
                    }
                    
                    yaml_document_delete(&document);
                    
                    if(ps.error != YAML_NO_ERROR) {
                        kk::Log("[%d] %s",ps.error,ps.problem);
                    }
         
                }
                
                yaml_parser_delete(&ps);

                if(fd != nullptr) {
                    fclose(fd);
                }
                
                _program = v;
                
                if(v != nullptr) {
                    _projection = v->uniform("projection");
                    _view = v->uniform("view");
                }
                
            }
            
            if(v != nullptr) {
                
                {
                    std::set<kk::String>::iterator i = _updateKeys.begin();
                    while(i != _updateKeys.end()) {
                        
                        kk::String key = * i;
                        std::map<kk::String,kk::Strong>::iterator ii = _propertys.find(key);
                        
                        if(ii != _propertys.end()) {
                            ProgramProperty * p = ii->second.as<ProgramProperty>();
                            if(p){
                                p->set(context, get(key).c_str());
                            }
                        }
                        
                        i ++;
                    }
                    _updateKeys.clear();
                }
                
                std::map<kk::String,kk::Strong>::iterator i = _propertys.begin();
                
                while(i != _propertys.end()) {
                    
                    ProgramProperty * p = i->second.as<ProgramProperty>();
                    
                    if(p && !p->isAvailable()) {
                        return;
                    }
                    
                    i ++;
                }
                
                v->use();
                
                glBindVertexArrayOES(_vao);
                
                i = _propertys.begin();
                
                while(i != _propertys.end()) {
                    
                    ProgramProperty * p = i->second.as<ProgramProperty>();
                    
                    if(p ) {
                        p->set(v);
                    }
                    
                    i ++;
                }
                
                ContextState & state = context->state();
                
                v->setUniform(_projection, state.projection);
                v->setUniform(_view, state.view );
                
                glDepthMask(_depthMask);
                
                glDrawArrays(_mode, 0, _count);
                
                glDepthMask(GL_TRUE);
                
                glBindVertexArrayOES(0);
            }

        }
        
    }
}

