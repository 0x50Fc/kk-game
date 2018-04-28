//
//  GA.h
//  KKGame
//
//  Created by hailong11 on 2018/2/1.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef GA_h
#define GA_h

#include "kk-element.h"

namespace kk {
    
    namespace GA {
        
        typedef float Float;
        typedef bool Boolean;
        
        struct Point {
            Float x,y;
            Point():x(0),y(0){}
            Point(Float x,Float y):x(x),y(y){}
            Point & operator +=(Point &v) {
                x += v.x;
                y += v.y;
                return *this;
            }
        };
        
        struct Size {
            Float width,height;
            Size():width(0),height(0){}
            Size(Float width,Float height):width(width),height(height){}
        };
        
        Float floatValue(String & value);
        
        kk::Int intValue(String & value);
        
        Boolean booleanValue(String & value);
        
        typedef long TimeInterval;
        
        TimeInterval GetTimeIntervalCurrent();
        
        class Context;
        
        typedef void (*CFunction) (Context * context, void * userData);
        
        class Function : public kk::Object {
        public:
            Function(TimeInterval timeInterval,CFunction cfunc,void * userData);
            Function(TimeInterval timeInterval,kk::script::Object * jsfunc);
            void call(Context * context);
            TimeInterval timeInterval;
            CFunction cfunc;
            void * userData;
            kk::Strong jsfunc;
        };
        
        class Context : public kk::Object ,public kk::script::IObject {
        public:
            Context();
            virtual ~Context();
            virtual TimeInterval current();         //当时间
            virtual kk::Uint speed();               //速度
            virtual kk::Uint frames();              //帧数／秒
            virtual void setFrames(kk::Uint frames);
            virtual void tick();
            virtual CString basePath();
            virtual void setBasePath(CString basePath);
            virtual kk::String absolutePath(CString path);
            virtual void exec(kk::Element * element);
            virtual Size& viewport();
            virtual void setViewport(Size & size);
            virtual void setViewport(Float width,Float height);
            virtual void setDifference(TimeInterval difference); //时差
            virtual TimeInterval difference();
            virtual void on(TimeInterval timeInterval,CFunction func,void * userData);
            
            DEF_SCRIPT_CLASS
            
            virtual duk_ret_t duk_current(duk_context * ctx);
            virtual duk_ret_t duk_difference(duk_context * ctx);
            virtual duk_ret_t duk_setDifference(duk_context * ctx);
            virtual duk_ret_t duk_on(duk_context * ctx);
            
        protected:
            virtual void on(Function * func);
            TimeInterval _current;
            TimeInterval _startTimeInterval;
            TimeInterval _difference;
            kk::Uint _frames;
            kk::Uint64 _count;
            kk::String _basePath;
            Size _viewport;
            std::list<Strong> _funcs;
        };
        
        class Element : public kk::Element {
        public:
            Element();
            virtual void exec(Context * context);
            virtual void setRemoved();
            
            virtual duk_ret_t duk_setRemoved(duk_context * ctx);
            
            DEF_SCRIPT_CLASS
        protected:
            kk::Boolean _removed;
        };
        
        extern Float zIndexAutoY;
        
        class IPosition {
        public:
            virtual Point position() = 0;
            virtual Float zIndex() = 0;

        };
    }
    
}

#endif /* GA_h */
