//
//  GLContext.h
//  KKGame
//
//  Created by zhanghailong on 2018/2/5.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef GLContext_h
#define GLContext_h

#include "GAContext.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

#if defined(__APPLE__)

#if TARGET_OS_OSX
#include <OpenGL/OpenGL.h>
#include <OpenGL/gl.h>
#else
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#endif

#elif defined(__ANDROID_API__)

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>

extern GLvoid glBindVertexArrayOES(GLuint array);
extern GLvoid glDeleteVertexArraysOES(GLsizei n, const GLuint *arrays);
extern GLvoid glGenVertexArraysOES(GLsizei n, GLuint *arrays);

#endif

namespace kk {
    
    namespace GL {
        
        typedef bool Boolean;
        typedef float Float;
        typedef int Int;
        typedef glm::vec2 vec2;
        typedef glm::vec3 vec3;
        typedef glm::vec4 vec4;
        typedef glm::mat3 mat3;
        typedef glm::mat4 mat4;
        
        enum ImageStatus {
            ImageStatusNone,ImageStatusLoaded,ImageStatusLoading,ImageStatusFail
        };
        
        class Texture : public kk::EventEmitter {
        public:
            Texture();
            virtual GLuint texture();
            virtual ~Texture();
            virtual void active(kk::Uint textureId);
            virtual GLsizei width();
            virtual GLsizei height();
            virtual void setSize(GLsizei width,GLsizei height);
        private:
            GLuint _texture;
            GLsizei _width;
            GLsizei _height;
        };
        
        class Image : public Texture {
        public:
            virtual ~Image();
            virtual CString uri();
            virtual ImageStatus status();
            virtual void setStatus(ImageStatus status);
            virtual CString error();
            virtual void setError(CString error);
        private:
            Image(CString uri);
            String _uri;
            ImageStatus _status;
            String _error;
            friend class Context;
        };
        
        enum FontStyle {
            FontStyleNormal,FontStyleItalic
        };
        
        enum FontWeight {
            FontWeightNormal,FontWeightBold
        };
        
        struct Shadow {
            vec4 color;
            Float x;
            Float y;
            Float radius;
        };
        
        struct Stroke {
            vec4 color;
            Float size;
        };
        
        struct Paint {
            String fontFimlay;
            kk::GA::Float fontSize;
            FontStyle fontStyle;
            FontWeight fontWeight;
            vec4 textColor;
            Float maxWidth;
            Shadow textShadow;
            Stroke textStroke;
        };
        
        struct ContextState {
            mat4 projection;
            mat4 view;
            Float opacity;
        };
        
        struct TextureVertex {
            vec3 position;
            vec2 texCoord;
        };
        
        class LoadingProgress {
        public:
            virtual Float loadingProgress() = 0;
        };
        
        class Program;
        class Buffer;

        class Context : public kk::GA::Context {
        public:
            Context();
            virtual ~Context();
            virtual Strong image(CString uri);
            virtual Strong program(CString name);
            virtual Strong program(CString name,CString vsh, CString fsh);
            virtual void draw(kk::Element * element);
            virtual ContextState& state();
            virtual void store();
            virtual void restore();
            virtual void drawTexture(Texture * texture,vec4 dest,vec4 src, Float zIndex);
            virtual void drawTexture(Texture * texture,GLenum mode,TextureVertex * points,kk::Uint count);
            virtual void drawTexture(Texture * texture,GLenum mode,Buffer *data);
            virtual void drawFillColor(GLenum mode,vec3 * points,kk::Uint count,vec4 color);
            virtual void init();
            virtual kk::Boolean isVisible(vec4 * points,kk::Uint count,vec4 margin);
            virtual Float loadingProgress(kk::Element * element);
        private:
            std::map<String,kk::Weak> _images;
            std::map<String,kk::Weak> _programs;
            std::vector<ContextState> _states;
            kk::Strong _GLSLTextureProgram;
            kk::Strong _GLSLFillColorProgram;
            struct {
                kk::Int position;
                kk::Int texCoord;
                kk::Int projection;
                kk::Int view;
                kk::Int texture;
                kk::Int opacity;
            } _GLSLTextureProgramUniform;
            struct {
                kk::Int position;
                kk::Int projection;
                kk::Int view;
                kk::Int color;
            } _GLSLFillColorProgramUniform;
        };
        
        extern void ContextGetImage(Context * context, Image * image);
        
        extern void ContextGetStringTexture(Context * context,Texture * texture ,CString text, Paint & paint);
        

        class Buffer : public kk::Object {
        public:
            Buffer(GLsizeiptr size, const GLvoid* data, GLenum usage);
            virtual ~Buffer();
            virtual void set(GLintptr offset, GLsizeiptr size, const GLvoid* data);
            virtual void bind();
            virtual GLsizeiptr size();
            
            static void unbind();
        protected:
            GLuint _value;
            GLsizeiptr _size;
        };
        
        
        class Program : public kk::Object {
        public:
            Program(kk::CString vshCode,kk::CString fshCode);
            virtual ~Program();
            virtual void use();
            virtual kk::Int uniform(kk::CString name);
            virtual kk::Int attrib(kk::CString name);
            virtual void setUniform(kk::Int loc,Float value);
            virtual void setUniform(kk::Int loc,Float * value, kk::Int count);
            virtual void setUniform(kk::Int loc,vec2 value);
            virtual void setUniform(kk::Int loc,vec2 * value, kk::Int count);
            virtual void setUniform(kk::Int loc,vec3 value);
            virtual void setUniform(kk::Int loc,vec3 * value, kk::Int count);
            virtual void setUniform(kk::Int loc,vec4 value);
            virtual void setUniform(kk::Int loc,vec4 * value, kk::Int count);
            virtual void setUniform(kk::Int loc,kk::Int value);
            virtual void setUniform(kk::Int loc,kk::Int * value, kk::Int count);
            virtual void setUniform(kk::Int loc,mat3 value);
            virtual void setUniform(kk::Int loc,mat3 * value, kk::Int count);
            virtual void setUniform(kk::Int loc,mat4 value);
            virtual void setUniform(kk::Int loc,mat4 * value, kk::Int count);
            virtual void setAttrib(kk::Int loc,kk::Uint size,void * data,kk::Int stride);
            
        protected:
            GLuint _value;
        };
        
        
        
        class AnimationTarget {
        public:
            virtual void setAnimationTransform(mat4 transform) = 0;
            virtual void setAnimationOpacity(Float opacity) = 0;
            virtual void setAnimationImage(Image * image) = 0;
            virtual void setAnimationValue(Float value) = 0;
        };
        
        class Element : public kk::GA::Element , public AnimationTarget {
        public:

            virtual void draw(Context * context);
            virtual void changedKey(String& key);
            virtual void setAnimationOpacity(Float opacity);
            virtual void setAnimationTransform(mat4 transform);
            virtual void setAnimationImage(Image * image);
            virtual void setAnimationValue(Float value);
            
            vec3 position;
            vec3 scale;
            mat4 transform;
            Float opacity;
            Boolean hidden;
            
            KK_DEF_ELEMENT_CREATE(Element)
            
            DEF_SCRIPT_CLASS_NOALLOC
            
        protected:
            virtual void onDraw(Context * context);
        };

        mat4 TransformForString(kk::CString value);
        vec4 colorValue(kk::CString value);
        Shadow shadowValue(kk::CString value);
        Stroke strokeValue(kk::CString value);
        
        
        
    }
    
}

#endif /* GLContext_h */
