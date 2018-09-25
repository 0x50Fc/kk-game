//
//  GLRenderElement.cc
//  KKGame
//
//  Created by zhanghailong on 2018/9/25.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//



#include "kk-config.h"
#include "GLRenderElement.h"

namespace kk {
    
    namespace GL {
        
        IMP_SCRIPT_CLASS_BEGIN_NOALLOC(&Element::ScriptClass, RenderElement, GLRenderElement)
        
        IMP_SCRIPT_CLASS_END
        
        KK_IMP_ELEMENT_CREATE(RenderElement)
        
        RenderElement::RenderElement(kk::Document * document,kk::CString name, kk::ElementKey elementId):Element(document,name,elementId)
            ,_mode(RenderModeTriangles),_count(0),_depth(false)
            ,_projectionLoc(-1),_viewLoc(-1),_opacityLoc(-1),_animationLoc(-1),_animationValue(0){
            
        }
        
        Program * RenderElement::program() {
            return _program.as<Program>();
        }
        
        void RenderElement::setAnimationValue(Float value) {
            _animationValue = value;
        }
        
        void RenderElement::onDraw(Context * context) {
            Element::onDraw(context);
            
            Program * v = program();
            
            kk::CString name = get("program");
            
            if(v == nullptr && name) {
                
                kk::Strong vv = context->program(name);
                
                v = vv.as<Program>();
                
                _program = v;
                
            }
            
            if(v != nullptr) {
                
                v->use();
                
                if(_projectionLoc == -1) {
                    _projectionLoc = v->uniform("projection");
                }
                
                if(_viewLoc == -1) {
                    _viewLoc = v->uniform("view");
                }
                
                if(_opacityLoc == -1) {
                    _opacityLoc = v->uniform("opacity");
                }
                
                if(_animationLoc == -1) {
                    _animationLoc = v->uniform("animation");
                }
                
                ContextState & state = context->state();
                
                if(_projectionLoc != -1) {
                    v->setUniform(_projectionLoc, state.projection );
                }
                
                if(_viewLoc != -1) {
                    v->setUniform(_viewLoc, state.view );
                }
                
                if(_opacityLoc != -1) {
                    v->setUniform(_opacityLoc, state.opacity );
                }
                
                if(_animationLoc != -1) {
                    v->setUniform(_animationLoc, _animationValue);
                }
                
                kk::Uint textureId = 0;
                
                VertexElement * vertex = nullptr;
                
                kk::Element * e = firstChild();
                
                while(e) {
                    
                    {
                        UniformElement * u = dynamic_cast<UniformElement *>(e);
                        if(u) {
                            textureId = u->use(context, program(), textureId);
                        }
                        
                    }
                    
                    if(vertex == nullptr) {
                        vertex = dynamic_cast<VertexElement *>(e);
                    } else {
                        AttribElement * a = dynamic_cast<AttribElement *>(e);
                        if(a) {
                            kk::Uint n = 0;
                            kk::Float * data = vertex->data(&n);
                            a->use(context, v, data, n);
                        }
                    }
                    
                    e = e->nextSibling();
                }
                
                if(_count > 0) {
                    
                    glDepthMask(_depth ? GL_TRUE : GL_FALSE);
                    
                    glDrawArrays(_mode, 0, _count);
                    
                    glDepthMask(GL_TRUE);
                    
                }
                
            }
        }
        
        void RenderElement::changedKey(String& key) {
            Element::changedKey(key);
            
            kk::CString v = get(key.c_str());
   
            if(key == "mode") {
                if(kk::CStringEqual(v, "points")) {
                    _mode = RenderModePoints;
                } else if(kk::CStringEqual(v, "lines")) {
                    _mode = RenderModeLines;
                } else if(kk::CStringEqual(v, "lineLoop")) {
                    _mode = RenderModeLineLoop;
                } else if(kk::CStringEqual(v, "lineStrip")) {
                    _mode = RenderModeLineStrip;
                } else if(kk::CStringEqual(v, "triangles")) {
                    _mode = RenderModeTriangles;
                } else if(kk::CStringEqual(v, "triangleStrip")) {
                    _mode = RenderModeTriangleStrip;
                } else if(kk::CStringEqual(v, "triangleFan")) {
                    _mode = RenderModeTriangleFan;
                } else  {
                    _mode = RenderModeTriangles;
                }
            } else if(key == "count") {
                _count = kk::GA::intValue(v);
            } else if(key == "depth") {
                _depth = kk::GA::booleanValue(v);
            }
        }
        
        Float RenderElement::loadingProgress() {
            
            Program * v = program();
            
            if(v == nullptr) {
                return 1.0f;
            }
            
            kk::Int c = 0;
            kk::Int n = 0;
            kk::Element * e = firstChild();
            
            while(e) {
                
                {
                    UniformElement * u =dynamic_cast<UniformElement *>(e);
                    if(u && u->type == UniformTypeImage) {
                        c ++;
                        Image * image = u->image.as<Image>();
                        if(image == nullptr || image->status() == ImageStatusLoaded || image->status() == ImageStatusFail) {
                            n ++;
                        }
                    }
                }
                
                e = e->nextSibling();
            }
            
            if(c == 0) {
                return 1.0f;
            }
            
            return (Float)(n) / (Float) (c);
        }
        
        IMP_SCRIPT_CLASS_BEGIN_NOALLOC(&kk::StyleElement::ScriptClass, UniformElement, GLUniformElement)
        
        IMP_SCRIPT_CLASS_END
        
        KK_IMP_ELEMENT_CREATE(UniformElement)
        
        UniformElement::UniformElement(kk::Document * document,kk::CString name, kk::ElementKey elementId):kk::StyleElement(document,name,elementId),type(UniformTypeFloat),_loc(-1) {
            
        }
        
        UniformElement::~UniformElement() {
            

        }
        
        kk::Uint UniformElement::use(Context * context,Program * program,kk::Uint textureId) {
            
            if(_loc == -1) {
                kk::CString name = get("name");
                if(name) {
                    _loc = program->uniform(name);
                }
            }
            
            if(_loc != -1) {
                switch (type) {
                    case UniformTypeFloat:
                        program->setUniform(_loc, (Float *) _data.data(), (kk::Int) _data.size());
                        break;
                    case UniformTypeVec2:
                        program->setUniform(_loc, (vec2 *) _data.data(), (kk::Int) _data.size() / 2);
                        break;
                    case UniformTypeVec3:
                        program->setUniform(_loc, (vec3 *) _data.data(), (kk::Int) _data.size() / 3);
                        break;
                    case UniformTypeVec4:
                        program->setUniform(_loc, (vec4 *) _data.data(), (kk::Int) _data.size() / 4);
                        break;
                    case UniformTypeMat3:
                        program->setUniform(_loc, (mat3 *) _data.data(), (kk::Int) _data.size() / 9);
                        break;
                    case UniformTypeMat4:
                        program->setUniform(_loc, (mat4 *) _data.data(), (kk::Int) _data.size() / 16);
                        break;
                    case UniformTypeImage:
                        Image * v = image.as<Image>();
                        if(v == nullptr) {
                            kk::CString src = get("src");
                            if(src) {
                                kk::Strong vv = context->image(src);
                                v = vv.as<Image>();
                                image = v;
                            }
                        }
                        if(v != nullptr && v->status() == ImageStatusLoaded) {
                            v->active(GL_TEXTURE0 + textureId);
                            program->setUniform(_loc, kk::Int(textureId));
                            textureId ++;
                        }
                        break;
                }
            }
            
            return textureId;
        }
        
        void UniformElement::changedKey(String& key) {
            kk::StyleElement::changedKey(key);
            
            kk::CString v = get(key.c_str());
            
            if(key == "type") {
                if(kk::CStringEqual(v, "vec2")) {
                    type = UniformTypeVec2;
                } else if(kk::CStringEqual(v, "vec3")) {
                    type = UniformTypeVec3;
                } else if(kk::CStringEqual(v, "vec4")) {
                    type = UniformTypeVec4;
                } else if(kk::CStringEqual(v, "mat3")) {
                    type = UniformTypeMat3;
                } else if(kk::CStringEqual(v, "mat4")) {
                    type = UniformTypeMat4;
                } else if(kk::CStringEqual(v, "image")) {
                    type = UniformTypeImage;
                } else  {
                    type = UniformTypeFloat;
                }
            } else if(key == "#text") {
                _data.clear();
                char * p = (char *) v;
                char * b = nullptr;
                Float v = 0;
                
                while(p) {
        
                    if((*p >= '0' && *p <= '9') || *p == '.' || *p == '-') {
                        
                        if(b == nullptr) {
                            b = p;
                        }
                        
                    } else {
                        
                        if(b && p - b > 0) {
                            sscanf(b,"%f",&v);
                            b = nullptr;
                            _data.push_back(v);
                        }
                    }
                    if(*p == 0) {
                        break;
                    }
                    p ++;
                }
            } else if(key == "name") {
                _loc = -1;
            } else if(key == "src") {
                image = nullptr;
            }
        }
        
        IMP_SCRIPT_CLASS_BEGIN_NOALLOC(&kk::StyleElement::ScriptClass,VertexElement, GLVertexElement)
        
        IMP_SCRIPT_CLASS_END
        
        KK_IMP_ELEMENT_CREATE(VertexElement)
        
        VertexElement::VertexElement(kk::Document * document,kk::CString name, kk::ElementKey elementId):kk::StyleElement(document,name,elementId){
            
        }
        
        void VertexElement::changedKey(String& key) {
            kk::StyleElement::changedKey(key);
            
            kk::CString v = get(key.c_str());
            
            if(key == "#text") {
                _data.clear();
                char * p = (char *) v;
                char * b = nullptr;
                Float v = 0;
                
                while(p) {
                    
                    if((*p >= '0' && *p <= '9') || *p == '.' || *p == '-') {
                        
                        if(b == nullptr) {
                            b = p;
                        }
                        
                    } else {
                        
                        if(b && p - b > 0) {
                            sscanf(b,"%f",&v);
                            b = nullptr;
                            _data.push_back(v);
                        }
                    }
                    
                    if(*p == 0) {
                        break;
                    }
                    p ++;
                }
            } 
        }
        
        kk::Float * VertexElement::data(kk::Uint * n) {
            if(n) {
                *n = (kk::Uint) _data.size();
            }
            return (kk::Float *) _data.data();
        }
        
        IMP_SCRIPT_CLASS_BEGIN_NOALLOC(&kk::StyleElement::ScriptClass,AttribElement, GLAttribElement)
        
        IMP_SCRIPT_CLASS_END
        
        KK_IMP_ELEMENT_CREATE(AttribElement)
        
        AttribElement::AttribElement(kk::Document * document,kk::CString name, kk::ElementKey elementId):kk::StyleElement(document,name,elementId),_size(0),_stride(0),_loc(-1),_off(0) {
            
        }
        
        void AttribElement::changedKey(String& key) {
            kk::StyleElement::changedKey(key);
            
            kk::CString v = get(key.c_str());
            
            if(key == "size") {
                _size = kk::GA::intValue(v);
            } else if(key == "stride") {
                _stride = kk::GA::intValue(v);
            } else if(key == "off") {
                _off = kk::GA::intValue(v);
            } else if(key == "name") {
                _loc = -1;
            }
        }
        
        void AttribElement::use(Context * context,Program * program,kk::Float * data,kk::Uint size) {
            
            if(_loc == -1) {
                kk::CString name = get("name");
                if(name) {
                    _loc = program->attrib(name);
                }
            }
            
            if(_loc != -1 && size > 0 && data && _size > 0 && _stride > 0 ) {
                program->setAttrib(_loc, _size, data + _off, _stride * sizeof(Float));
            }
            
        }
        
    }
    
}

