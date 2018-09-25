//
//  GLRenderElement.h
//  KKGame
//
//  Created by zhanghailong on 2018/9/25.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef GLRenderElement_h
#define GLRenderElement_h

#include "GLContext.h"

namespace kk {
    
    namespace GL {
        
        enum UniformType {
            UniformTypeFloat,
            UniformTypeVec2,
            UniformTypeVec3,
            UniformTypeVec4,
            UniformTypeMat3,
            UniformTypeMat4,
            UniformTypeImage,
        };
        
        class UniformElement : public kk::StyleElement{
            
        public:
            KK_DEF_ELEMENT_CREATE(UniformElement)
            DEF_SCRIPT_CLASS_NOALLOC
            
            virtual ~UniformElement();
            virtual kk::Uint use(Context * context,Program * program,kk::Uint textureId);
            virtual void changedKey(String& key);

            kk::Strong image;
            UniformType type;
            
        private:
            kk::String _name;
            std::vector<kk::Float> _data;
            kk::Int _loc;
        };
        
        class VertexElement : public kk::StyleElement{
            
        public:
            KK_DEF_ELEMENT_CREATE(VertexElement)
            DEF_SCRIPT_CLASS_NOALLOC
            
            virtual void changedKey(String& key);
            virtual kk::Float * data(kk::Uint * n);
            
        private:
            std::vector<kk::Float> _data;
        };
        
        class AttribElement : public kk::StyleElement{
            
        public:
            KK_DEF_ELEMENT_CREATE(AttribElement)
            DEF_SCRIPT_CLASS_NOALLOC
            
            virtual void changedKey(String& key);
            virtual void use(Context * context,Program * program,kk::Float * data,kk::Uint size);
        private:
            kk::Uint _off;
            kk::Uint _size;
            kk::Int _stride;
            kk::Int _loc;
        };
        
        enum RenderMode {
            RenderModePoints = GL_POINTS,
            RenderModeLines = GL_LINES,
            RenderModeLineLoop = GL_LINE_LOOP,
            RenderModeLineStrip = GL_LINE_STRIP,
            RenderModeTriangles = GL_TRIANGLES,
            RenderModeTriangleStrip = GL_TRIANGLE_STRIP,
            RenderModeTriangleFan = GL_TRIANGLE_FAN,
        };
        
        class RenderElement : public Element , public LoadingProgress {
        public:
            
            
            KK_DEF_ELEMENT_CREATE(RenderElement)
            DEF_SCRIPT_CLASS_NOALLOC
            
            virtual void setAnimationValue(Float value);
            
            virtual Float loadingProgress();
            
            virtual Program * program();
            
            virtual void changedKey(String& key);
            
        private:
            virtual void onDraw(Context * context);
            kk::Strong _program;
            RenderMode _mode;
            kk::Uint _count;
            kk::Boolean _depth;
            kk::Int _projectionLoc;
            kk::Int _viewLoc;
            kk::Int _opacityLoc;
            kk::Int _animationLoc;
            kk::Float _animationValue;
        };
    }
    
}

#endif /* GLRenderElement_h */
