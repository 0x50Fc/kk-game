//
//  GLContext.cpp
//  KKGame
//
//  Created by zhanghailong on 2018/2/5.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include "kk-config.h"
#include "GLContext.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include "GLSLTexture.h"
#include "GLSLFillColor.h"

#if defined(KK_PLATFORM_IOS)

#include <KKObject/KKObject.h>

#else

#include "kk-string.h"

#endif

#include "kk-y.h"
#include <algorithm>
#include <sstream>

#if defined(__ANDROID_API__)

typedef GLvoid (*glBindVertexArrayOESFunc)(GLuint array);
typedef GLvoid (*glDeleteVertexArraysOESFunc)(GLsizei n, const GLuint *arrays);
typedef GLvoid (*glGenVertexArraysOESFunc)(GLsizei n, GLuint *arrays);

static glBindVertexArrayOESFunc v_glBindVertexArrayOESFunc = nullptr;
static glDeleteVertexArraysOESFunc v_glDeleteVertexArraysOESFunc = nullptr;
static glGenVertexArraysOESFunc v_glGenVertexArraysOESFunc = nullptr;

static bool kk_glOES_is = 0;

static void kk_glOES(void) {
    if(kk_glOES_is == false) {
        v_glBindVertexArrayOESFunc = (glBindVertexArrayOESFunc)eglGetProcAddress ( "glGenVertexArraysOES" );
        v_glBindVertexArrayOESFunc = (glBindVertexArrayOESFunc)eglGetProcAddress ( "glBindVertexArrayOES" );
        v_glDeleteVertexArraysOESFunc = (glDeleteVertexArraysOESFunc)eglGetProcAddress ( "glDeleteVertexArraysOES" );
        kk_glOES_is = true;
    }
}

GLvoid glBindVertexArrayOES(GLuint array) {
    if(v_glBindVertexArrayOESFunc) {
        return v_glBindVertexArrayOESFunc(array);
    }
}
GLvoid glDeleteVertexArraysOES(GLsizei n, const GLuint *arrays) {
    if(v_glDeleteVertexArraysOESFunc) {
        v_glDeleteVertexArraysOESFunc(n,arrays);
    }
}

GLvoid glGenVertexArraysOES(GLsizei n, GLuint *arrays) {
    if(v_glGenVertexArraysOESFunc) {
        v_glGenVertexArraysOESFunc(n,arrays);
    }
}


#endif


namespace kk {
    
    namespace GL {
     
        Texture::Texture():_width(0),_height(0) {
            glGenTextures(1, &_texture);
        }
        
        Texture::~Texture() {
            if(_texture) {
                glDeleteTextures(1, &_texture);
                _texture = 0;
            }
        }
        
        GLuint Texture::texture() {
            return _texture;
        }
        
        void Texture::active(kk::Uint textureId) {
            if(_texture) {
                glActiveTexture(textureId);
                glBindTexture(GL_TEXTURE_2D, _texture);
            }
        }
        
        GLsizei Texture::width() {
            return _width;
        }
        
        GLsizei Texture::height() {
            return _height;
        }
        
        void Texture::setSize(GLsizei width,GLsizei height) {
            _width = width;
            _height = height;
        }
        
        Image::~Image() {
        }
        
        CString Image::uri() {
            return _uri.c_str();
        }
        
        ImageStatus Image::status() {
            return _status;
        }
        
        void Image::setStatus(ImageStatus status) {
            _status = status;
        }
        
        CString Image::error() {
            return _error.c_str();
        }
        
        void Image::setError(CString error) {
            _error = error;
        }
        
        Image::Image(CString uri):Texture(),_uri(uri),_status(ImageStatusNone) {

        }
        
        Context::Context() {
            
#if defined(__ANDROID_API__)
            kk_glOES();
#endif
            ContextState state;
            state.projection = glm::mat4(1.0f);
            state.view = glm::mat4(1.0f);
            state.opacity = 1.0;
            
            _states.push_back(state);
        
        }
        
        Context::~Context() {
            
        }
        
        void Context::init() {
            
            _GLSLTextureProgram = kk::GLSL::GLSLTextureCreate();
            _GLSLFillColorProgram = kk::GLSL::GLSLFillColorCreate();
            
            {
                Program * program = _GLSLTextureProgram.as<Program>();
                _GLSLTextureProgramUniform.position = program->attrib("position");
                _GLSLTextureProgramUniform.texCoord = program->attrib("texCoord");
                _GLSLTextureProgramUniform.projection = program->uniform("projection");
                _GLSLTextureProgramUniform.view = program->uniform("view");
                _GLSLTextureProgramUniform.texture = program->uniform("texture");
                _GLSLTextureProgramUniform.opacity = program->uniform("opacity");
                _programs["texture"] = program;
            }
            
            {
                Program * program = _GLSLFillColorProgram.as<Program>();
                _GLSLFillColorProgramUniform.position = program->attrib("position");
                _GLSLFillColorProgramUniform.projection = program->uniform("projection");
                _GLSLFillColorProgramUniform.view = program->uniform("view");
                _GLSLFillColorProgramUniform.color = program->uniform("color");
            }
            
        }
        
        ContextState& Context::state() {
            return * _states.begin();
        }
        
        void Context::store() {
            ContextState v = state();
            _states.insert(_states.begin(), v);
        }
        
        void Context::restore() {
            _states.erase(_states.begin());
        }
        

        static bool elementZIndexComp(kk::Element * pl, kk::Element * pr) {
            kk::GA::IPosition * vl = dynamic_cast<kk::GA::IPosition *>(pl);
            kk::GA::IPosition * vr = dynamic_cast<kk::GA::IPosition *>(pr);
            return vl->zIndex() < vr->zIndex();
        }
        
        static void ContextDrawElementWithZIndex(Context * context, std::vector<kk::Element *> & elements) {
            
            std::sort(elements.begin(),elements.end(),elementZIndexComp);
            
            std::vector<kk::Element *>::iterator i = elements.begin();
            
            while(i != elements.end()) {
                context->draw(* i);
                i ++;
            }
            
        }
        
        void Context::draw(kk::Element * element) {
        
            {
                Element * e = dynamic_cast<Element *>(element);
                
                if(e) {
                    e->draw(this);
                    return;
                }
            }
            
            {
                kk::GA::Element * e = dynamic_cast<kk::GA::Element *>(element);
                
                if(e) {
                    
                    store();
                    
                    {
                        kk::GA::IPosition * v = dynamic_cast<kk::GA::IPosition *>(e);
                        
                        if(v) {
                            
                            kk::GA::Point p = v->position();
                            
                            ContextState & state = this->state();
                            
                            state.view = glm::translate(state.view, vec3(p.x,p.y,v->zIndex()));
                            
                        }
                    }
                    
                    std::vector<kk::Element *> vs;
                    
                    kk::Element * p = e->firstChild();
                    
                    while(p) {
                        kk::GA::IPosition * v = dynamic_cast<kk::GA::IPosition *>(p);
                        if(v) {
                            vs.push_back(p);
                        } else {
                            if(!vs.empty()) {
                                ContextDrawElementWithZIndex(this,vs);
                                vs.clear();
                            }
                            draw(p);
                        }
                        p = p->nextSibling();
                    }
                    
                    if(!vs.empty()) {
                        ContextDrawElementWithZIndex(this,vs);
                    }
                    
                    restore();
                }
            }
            
        }
        
        Strong Context::image(CString uri) {
            
            if(uri == nullptr){
                return nullptr;
            }
            
            std::map<String,kk::Weak>::iterator i = _images.find(uri);
            
            if(i != _images.end()) {
                kk::Weak & weak = i->second;
                Image * v = weak.as<Image>();
                if(v != nullptr) {
                    return v;
                }
            }
            
            Image * v = new Image(uri);
            
            _images[uri] = v;
            
            if(v->status() == ImageStatusNone) {
                ContextGetImage(this, v);
            }
            
            return v;
        }
        
        Strong Context::program(CString path) {
            
            if(path == nullptr){
                return nullptr;
            }
            
            Program * v = nullptr;
            
            std::map<String,kk::Weak>::iterator i = _programs.find(path);
            
            if(i != _programs.end()) {
                kk::Weak & weak = i->second;
                v = weak.as<Program>();
                if(v != nullptr) {
                    return v;
                }
            }
            
            kk::String vv = getString(path);
            
            if(!vv.empty()) {
                
                
                kk::Strong jsContext = new kk::script::Context();
                
                duk_context * ctx = jsContext.as<kk::script::Context>()->jsContext();
                
                if(kk::script::decodeJSON(ctx, vv.c_str(), vv.size()) != DUK_EXEC_SUCCESS) {
                    kk::script::Error(ctx, -1);
                }
                
                if(duk_is_object(ctx, -1)) {
                    kk::CString vsh = Y_toString(ctx, -1, "vsh",nullptr);
                    kk::CString fsh = Y_toString(ctx, -1, "fsh",nullptr);
                    if(vsh && fsh) {
                        v = new Program(vsh,fsh);
                    }
                }
                
                duk_pop_n(ctx, 1);
                
            }
            
            if(v != nullptr) {
                _programs[path] = v;
            }
            
            return v;
        }
        
        void Context::drawTexture(Texture * texture,GLenum mode,TextureVertex * points,kk::Uint count) {
            
            Program * program = _GLSLTextureProgram.as<Program>();
            
            if(texture && program) {
                
                ContextState & state = this->state();
                
                program->use();
                
                texture->active(GL_TEXTURE0);
                
                program->setUniform(_GLSLTextureProgramUniform.texture, kk::Int(0));
                
                program->setUniform(_GLSLTextureProgramUniform.projection, state.projection );
            
                program->setUniform(_GLSLTextureProgramUniform.view, state.view );
                
                program->setUniform(_GLSLTextureProgramUniform.opacity, state.opacity);
             
                program->setAttrib(_GLSLTextureProgramUniform.position, 3, points, sizeof(TextureVertex));
                
                program->setAttrib(_GLSLTextureProgramUniform.texCoord, 2, (char *) points + sizeof(vec3), sizeof(TextureVertex));
                
                glDepthMask(GL_FALSE);
                
                glDrawArrays(mode, 0, count);
                
                glDepthMask(GL_TRUE);
            }
            
        }
        
        void Context::drawTexture(Texture * texture,GLenum mode,Buffer *data) {
            
            data->bind();
            
            drawTexture(texture,mode,NULL,(kk::Uint) (data->size() / sizeof(TextureVertex)));
            
            Buffer::unbind();
            
        }
        
        void Context::drawTexture(Texture * texture,vec4 dest,vec4 src, Float zIndex) {
            
            Float top = dest.y;
            Float bottom = dest.y + dest.w;
            Float left = dest.x;
            Float right = dest.x + dest.z;
            
            Float stop = src.y / texture->height();
            Float sbottom = stop + src.w / texture->height();
            Float sleft = src.x / texture->width();
            Float sright = sleft + src.z / texture->width();
            
            TextureVertex p[6] = {
                {{left,top,zIndex},{sleft,stop}},{{right,top,zIndex},{sright,stop}},{{left,bottom,zIndex},{sleft,sbottom}},{{left,bottom,zIndex},{sleft,sbottom}},{{right,top,zIndex},{sright,stop}},{{right,bottom,zIndex},{sright,sbottom}}};
            
            drawTexture(texture, GL_TRIANGLES, p, 6);
            
        }
        
        void Context::drawFillColor(GLenum mode,vec3 * points,kk::Uint count,vec4 color) {
            
            Program * program = _GLSLFillColorProgram.as<Program>();
            
            if(program) {
                
                ContextState & state = this->state();
                
                program->use();
   
                program->setUniform(_GLSLFillColorProgramUniform.projection, state.projection );
                
                program->setUniform(_GLSLFillColorProgramUniform.view, state.view );
                
                program->setUniform(_GLSLFillColorProgramUniform.color, color );
                
                program->setAttrib(_GLSLFillColorProgramUniform.position, 3, points, sizeof(vec3));
                
                glDepthMask(GL_FALSE);
                
                glDrawArrays(mode, 0, count);
                
                glDepthMask(GL_TRUE);
            }
            
        }
        
        kk::Boolean Context::isVisible(vec4 * points,kk::Uint count,vec4 margin) {
            
            vec4 box(INT32_MAX,INT32_MAX,INT32_MIN,INT32_MIN);
            
            ContextState & s = this->state();
            
            mat4 tranfrom = s.projection * s.view;
            
            while(points && count > 0) {
                
                vec4 p = tranfrom * points[0];
                
                box.x = MIN(p.x, box.x);
                box.y = MIN(p.y, box.y);
                box.z = MAX(p.x, box.z);
                box.w = MAX(p.y, box.w);
                
                points ++;
                count -- ;
            }
            
            kk::GA::Size &viewport = this->viewport();
            
            vec4 tbox(- 1.0f - (margin.x / viewport.width)
                      ,- 1.0f - (margin.y / viewport.height)
                      ,1.0f + (margin.x / viewport.width)
                      ,1.0f + (margin.y / viewport.height));
            
            if(box.x > tbox.z || box.z < tbox.x || box.y > tbox.w || box.w < tbox.x) {
                return false;
            }
            
            return true;
        }
        
        Float Context::loadingProgress(kk::Element * element) {
            
            Float v = 1.0f;
        
            LoadingProgress * p = dynamic_cast<LoadingProgress *>(element);
            
            if(p) {
                v *= p->loadingProgress();
            }
            
            kk::Element * e = element->firstChild();
            
            while(e) {
                
                v *= loadingProgress(e);
                
                e = e->nextSibling();
            }
        
            return v;
        }
        
        Buffer::Buffer(GLsizeiptr size, const GLvoid* data, GLenum usage):_value(0),_size(size) {
            glGenBuffers(1, &_value);
            glBindBuffer(GL_ARRAY_BUFFER, _value);
            glBufferData(GL_ARRAY_BUFFER, size,data,usage);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        
        void Buffer::set(GLintptr offset, GLsizeiptr size, const GLvoid* data) {
            glBindBuffer(GL_ARRAY_BUFFER, _value);
            glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        
        GLsizeiptr Buffer::size() {
            return _size;
        }
        
        Buffer::~Buffer() {
            if(_value) {
                glDeleteBuffers(1, &_value);
            }
        }
        
        void Buffer::unbind() {
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        
        void Buffer::bind() {
            glBindBuffer(GL_ARRAY_BUFFER, _value);
        }
        
        Program::Program(kk::CString vshCode,kk::CString fshCode) {
            
            _value = 0;
            
            GLint status = GL_TRUE;
            GLuint vshShader = 0,fshShader = 0;
            GLchar log[2048];
            GLint logLen = 0;
            
            vshShader = glCreateShader(GL_VERTEX_SHADER);
            
            glShaderSource(vshShader, 1, &vshCode, NULL);
            
            glCompileShader(vshShader);
            
            glGetShaderInfoLog(vshShader, sizeof(log), &logLen, log);
            
            if(logLen >0){
                kk::Log("Shader compile log: %s", log);
            }
            
            glGetShaderiv(vshShader, GL_COMPILE_STATUS, &status);
            
            if (status != GL_TRUE) {
                
                glDeleteShader(vshShader);
                
                kk::Log("Failed to vsh compile shader\n");
                
                return;
            }
            
            fshShader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fshShader, 1, &fshCode, NULL);
            glCompileShader(fshShader);
            
            glGetShaderInfoLog(fshShader, sizeof(log), &logLen, log);
            
            if(logLen >0){
                kk::Log("Shader compile log: %s", log);
            }
            
            glGetShaderiv(fshShader, GL_COMPILE_STATUS, &status);
            
            if (status != GL_TRUE) {
                
                glDeleteShader(vshShader);
                glDeleteShader(fshShader);
                
                kk::Log("Failed to fsh compile shader");
                
                return;
            }
            
            _value = glCreateProgram();
            
            glAttachShader(_value, vshShader);
            glAttachShader(_value, fshShader);
            
            glLinkProgram(_value);
            
            if (vshShader){
                glDeleteShader(vshShader);
            }
            
            if (fshShader){
                glDeleteShader(fshShader);
            }
            
            glGetProgramInfoLog(_value, sizeof(log), &logLen, log);
            
            if(logLen >0){
                kk::Log("Program link log: %s", log);
            }
            
            status = GL_TRUE;
            
            glGetProgramiv(_value, GL_LINK_STATUS, &status);
            
            if (status != GL_TRUE ){
                
                kk::Log("Program link error");
                glDeleteProgram(_value);
                _value = 0;
            }
            
        }
        
        Program::~Program() {
            
            if(_value) {
                glDeleteProgram(_value);
            }
            
        }
        
        void Program::use() {
            if(_value) {
                glUseProgram(_value);
            }
        }
        
        kk::Int Program::uniform(kk::CString name) {
            if(_value) {
                return glGetUniformLocation(_value, name);
            }
            return -1;
        }
        
        kk::Int Program::attrib(kk::CString name) {
            if(_value) {
                return glGetAttribLocation(_value, name);
            }
            return -1;
        }
        
        void Program::setUniform(kk::Int loc,Float value) {
            glUniform1f(loc,value);
        }
        
        void Program::setUniform(kk::Int loc,Float * value, kk::Int count) {
            if(_value) {
                glUniform1fv(loc,count,value);
            }
        }
        
        void Program::setUniform(kk::Int loc,vec2 value) {
            glUniform2f(loc,value.x,value.y);
        }
        
        void Program::setUniform(kk::Int loc,vec2 * value, kk::Int count) {
            if(_value) {
                glUniform2fv(loc,count,(Float *) value);
            }
        }
        
        void Program::setUniform(kk::Int loc,vec3 value) {
            glUniform3f(loc,value.x,value.y,value.z);
        }
        
        void Program::setUniform(kk::Int loc,vec3 * value, kk::Int count) {
            if(_value) {
                glUniform3fv(loc,count,(Float *) value);
            }
        }
        
        void Program::setUniform(kk::Int loc,vec4 value) {
            if(_value) {
                glUniform4f(loc,value.x,value.y,value.z,value.w);
            }
        }
        void Program::setUniform(kk::Int loc,vec4 * value, kk::Int count) {
            if(_value) {
                glUniform4fv(loc,count,(Float *) value);
            }
        }
        
        void Program::setUniform(kk::Int loc,kk::Int value) {
            glUniform1i(loc,value);
        }
        void Program::setUniform(kk::Int loc,kk::Int * value, kk::Int count) {
            if(_value) {
                glUniform1iv(loc,count,value);
            }
        }
        void Program::setUniform(kk::Int loc,mat3 value) {
            glUniformMatrix3fv(loc,1,GL_FALSE, (Float *) &value);
        }
        
        void Program::setUniform(kk::Int loc,mat3 * value, kk::Int count) {
            if(_value) {
                glUniformMatrix3fv(loc,count,GL_FALSE, (Float *) value);
            }
        }
        
        void Program::setUniform(kk::Int loc,mat4 value) {
            glUniformMatrix4fv(loc,1,GL_FALSE, (Float *) &value);
        }
        
        void Program::setUniform(kk::Int loc,mat4 * value, kk::Int count) {
            if(_value) {
                glUniformMatrix4fv(loc,count,GL_FALSE, (Float *) value);
            }
        }
        
        void Program::setAttrib(kk::Int loc,kk::Uint size,void * data,kk::Int stride) {
            glEnableVertexAttribArray(loc);
            glVertexAttribPointer(loc, size, GL_FLOAT, GL_FALSE, stride, data);
        }
        
        IMP_SCRIPT_CLASS_BEGIN_NOALLOC(&kk::GA::Element::ScriptClass, Element, GLElement)
        
        IMP_SCRIPT_CLASS_END

        KK_IMP_ELEMENT_CREATE(Element)

        Element::Element(kk::Document * document,kk::CString name, kk::ElementKey elementId):kk::GA::Element(document,name,elementId),position(0.0f),scale(1.0f),transform(1.0f),opacity(1.0f),hidden(false) {
            
        }
        
        void Element::draw(Context * context) {
            
            if(hidden || _removed) {
                return;
            }
    
            context->store();
            
            ContextState & s = context->state();
            
            s.opacity = s.opacity * opacity;
            s.view = glm::scale(glm::translate(s.view, position),scale) * transform;

            onDraw(context);
            
            kk::Element * e = firstChild();
            
            while(e) {
                context->draw(e);
                e = e->nextSibling();
            }
            
            context->restore();
        }
        
        void Element::onDraw(Context * context) {

        }
        
        void Element::changedKey(String& key) {
            kk::GA::Element::changedKey(key);
            
            if(key == "hidden") {
                hidden = kk::GA::booleanValue(get(key.c_str()));
            } else if(key == "x") {
                position.x = kk::GA::floatValue(get(key.c_str()));
            } else if(key == "y") {
                position.y = kk::GA::floatValue(get(key.c_str()));
            } else if(key == "z") {
                position.z = kk::GA::floatValue(get(key.c_str()));
            } else if(key == "scale") {
                std::vector<kk::String> vs;
                CStringSplit(get(key.c_str()), " ", vs);
                
                if(vs.size() == 1) {
                    scale.x = scale.y = kk::GA::floatValue(vs[0].c_str());
                } else if(vs.size() == 3) {
                    scale.x =  kk::GA::floatValue(vs[0].c_str());
                    scale.y =  kk::GA::floatValue(vs[1].c_str());
                    scale.z =  kk::GA::floatValue(vs[2].c_str());
                }
                
            } else if(key == "transform") {
                transform = TransformForString(get(key.c_str()));
            } else if(key == "opacity") {
                opacity = kk::GA::floatValue(get(key.c_str()));
            }
            
        }
        
        void Element::setAnimationTransform(mat4 transform) {
            this->transform = transform;
        }
        
        void Element::setAnimationImage(Image * image) {
            
        }
        
        void Element::setAnimationOpacity(Float opacity) {
            this->opacity = opacity;
        }
        
        mat4 TransformForString(kk::CString value) {
            mat4 v(1.0f);
            std::vector<kk::String> items;
            kk::CStringSplit(value, " ", items);
            std::vector<kk::String>::iterator i = items.begin();
            
            while(i != items.end()) {
                
                String& item = * i;
                
                if(CStringHasPrefix(item.c_str(), "translate")) {
                    vec3 vv(0);
                    sscanf(item.c_str(), "translate(%f,%f,%f)",&vv.x,&vv.y,&vv.z);
                    v = glm::translate(v, vv);
                } else if(CStringHasPrefix(item.c_str(), "scale")) {
                    vec3 vv(1.0f);
                    sscanf(item.c_str(), "scale(%f,%f,%f)",&vv.x,&vv.y,&vv.z);
                    v = glm::scale(v, vv);
                } else if(CStringHasPrefix(item.c_str(), "rotateX")) {
                    vec3 vv(1.0f,0.0f,0.0f);
                    Float angle = 0;
                    sscanf(item.c_str(), "rotateX(%f)",&angle);
                    angle = angle * M_PI / 180.0f;
                    v = glm::rotate(v, angle, vv);
                } else if(CStringHasPrefix(item.c_str(), "rotateY")) {
                    vec3 vv(0.0f,1.0f,0.0f);
                    Float angle = 0;
                    sscanf(item.c_str(), "rotateY(%f)",&angle);
                    angle = angle * M_PI / 180.0f;
                    v = glm::rotate(v, angle, vv);
                } else if(CStringHasPrefix(item.c_str(), "rotateZ")) {
                    vec3 vv(0.0f,0.0f,1.0f);
                    Float angle = 0;
                    sscanf(item.c_str(), "rotateZ(%f)",&angle);
                    angle = angle * M_PI / 180.0f;
                    v = glm::rotate(v, angle, vv);
                }
                
                i ++;
            }
            
            return v;
        }
        
        vec4 colorValue(CString value) {
            vec4 v(0);
            
            if(CStringHasPrefix(value, "#")) {
                size_t n = strlen(value);
                if(n == 9) {
                    {
                        kk::Int r=0,g=0,b=0,a=0;
                        sscanf(value, "#%02x%02x%02x%02x",&a,&r,&g,&b);
                        v.r = (Float) r / 255.0f;
                        v.g = (Float) g / 255.0f;
                        v.b = (Float) b / 255.0f;
                        v.a = (Float) a / 255.0f;
                    }
                } else if(n == 7) {
                    {
                        kk::Int r=0,g=0,b=0;
                        sscanf(value, "#%02x%02x%02x",&r,&g,&b);
                        v.r = (Float) r / 255.0f;
                        v.g = (Float) g / 255.0f;
                        v.b = (Float) b / 255.0f;
                        v.a = 1.0f;
                    }
                } else if(n == 4) {
                    {
                        kk::Int r=0,g=0,b=0;
                        sscanf(value, "#%1x%1x%1x",&r,&g,&b);
                        v.r = (Float) (r | (r << 4)) / 255.0f;
                        v.g = (Float) (g | (g << 4)) / 255.0f;
                        v.b = (Float) (b | (b << 4)) / 255.0f;
                        v.a = 1.0f;
                    }
                }
            }
            
            return v;
        }
        
        Shadow shadowValue(kk::CString value) {
            Shadow v = {vec4(0),0,0,0};
            std::vector<kk::String> vs;
            
            CStringSplit(value, " ", vs);
            
            size_t n = value ? strlen(value) : 0;
            
            if(n == 4) {
                v.x = kk::GA::floatValue(vs[0].c_str());
                v.y = kk::GA::floatValue(vs[1].c_str());
                v.radius = kk::GA::floatValue(vs[2].c_str());
                v.color = colorValue(vs[3].c_str());
            }
            
            return v;
        }
        
        Stroke strokeValue(kk::CString value) {
            Stroke v = {vec4(0),0};
            std::vector<kk::String> vs;
            
            CStringSplit(value, " ", vs);
            
            size_t n = value ? strlen(value) : 0;
            
            if(n == 2) {
                v.size = kk::GA::floatValue(vs[0].c_str());
                v.color = colorValue(vs[1].c_str());
            }
            return v;
        }
        
    }
}
