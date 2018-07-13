//
//  GAContext.cc
//  KKGame
//
//  Created by zhanghailong on 2018/2/1.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include "kk-config.h"
#include "GAContext.h"
#include "kk-string.h"
#include <chipmunk/chipmunk.h>
#include <sstream>

namespace kk {
    
    namespace GA {
        
        IMP_SCRIPT_CLASS_BEGIN(nullptr, Context, GAContext)
        
        static kk::script::Property propertys[] = {
            {"difference",(kk::script::Function) &Context::duk_difference,(kk::script::Function) &Context::duk_setDifference},
            {"current",(kk::script::Function) &Context::duk_current,(kk::script::Function)nullptr},
            {"frames",(kk::script::Function) &Context::duk_frames,(kk::script::Function)&Context::duk_setFrames},
        };
        
        kk::script::SetProperty(ctx, -1, propertys, sizeof(propertys) / sizeof(kk::script::Property));
        
        static kk::script::Method methods[] = {
            {"on",(kk::script::Function) &Context::duk_on},
        };
        
        kk::script::SetMethod(ctx, -1, methods, sizeof(methods) / sizeof(kk::script::Method));
        
        
        IMP_SCRIPT_CLASS_END
        
        TimeInterval GetTimeIntervalCurrent() {
            struct timeval tm;
            gettimeofday(&tm,NULL);
            TimeInterval v = (TimeInterval) tm.tv_sec * 1000LL + (TimeInterval) tm.tv_usec / 1000LL;
            return v;
        }
        
        
        Context::Context():_current(0),_frames(30),_count(0),_startTimeInterval(0),_difference(0) {

        }
        
        Context::~Context() {

        }
        
        TimeInterval Context::current() {
            return _current;
        }
        
        kk::Uint Context::speed() {
            if(_startTimeInterval != 0) {
                TimeInterval t = (GetTimeIntervalCurrent() - _startTimeInterval) / 1000;
                if(t > 0) {
                    return (kk::Uint) (_count / t);
                }
            }
            return 0;
        }
        
        kk::Uint Context::frames() {
            return _frames;
        }
        
        void Context::setFrames(kk::Uint frames) {
            _frames = frames > 0 ? frames : 30;
        }
        
        CString Context::basePath() {
            return _basePath.c_str();
        }
        
        void Context::setBasePath(CString basePath) {
            _basePath = basePath;
        }
        
        kk::String Context::absolutePath(CString path) {
            
            CString basePath = this->basePath();
            
            kk::String s;
            
            if(basePath) {
                
                s.append(basePath);
                
                if(!kk::CStringHasSuffix(basePath, "/")) {
                    s.append("/");
                }
                
            }
            
            s.append(path);
            
            return s;
        }
        
        kk::String Context::relativePath(CString path) {
            CString basePath = this->basePath();
            if(basePath && path && kk::CStringHasPrefix(path, basePath)) {
                char * p = (char *) path + strlen(basePath);
                while(*p == '/' && *p !=0) {
                    p ++;
                }
                return p;
            }
            return path;
        }
        
        kk::String Context::getString(CString path) {
            kk::String v;
            
            kk::String p = absolutePath(path);
            
            FILE * fd = fopen(p.c_str(), "r");
            
            if(fd) {
                
                char data[20480];
                size_t n;
                
                while((n = fread(data, 1, sizeof(data), fd)) > 0) {
                    v.append(data,0,n);
                }
            
                fclose(fd);
            } else{
                kk::Log("Not Open %s",p.c_str());
            }
            
            return v;
        }
        
        void Context::tick() {
            _count ++;
            if(_current == 0) {
                _current = _startTimeInterval = GetTimeIntervalCurrent() - _difference;
            } else {
                _current = GetTimeIntervalCurrent() - _difference;
            }
            {
                std::list<Strong>::iterator i = _funcs.begin();
                while(i != _funcs.end()) {
                    Function * func = (*i).as<Function>();
                    if(func && _current >= func->timeInterval) {
                        func->call(this);
                        i = _funcs.erase(i);
                    } else {
                        i ++;
                    }
                }
            }
        }
        
        void Context::exec(kk::Element * element) {
            
            Element * e = dynamic_cast<Element *>(element);
            
            if(e) {
                e->exec(this);
            }
            
        }
        
        Size& Context::viewport() {
            return _viewport;
        }
        
        void Context::setViewport(Size & size) {
            _viewport = size;
        }
        
        void Context::setViewport(Float width,Float height) {
            _viewport.width = width;
            _viewport.height = height;
        }
        
        void Context::setDifference(TimeInterval difference) {
            _difference = difference;
        }
        
        TimeInterval Context::difference() {
            return _difference;
        }
        
        void Context::on(Function * func) {
            _funcs.push_back(func);
        }
        
        void Context::on(TimeInterval timeInterval,CFunction func,void * userData) {
            kk::Strong fn = new Function(timeInterval,func,userData);
            on(fn.as<Function>());
        }
        
        duk_ret_t Context::duk_difference(duk_context * ctx) {
            duk_push_number(ctx, _difference);
            return 1;
        }
        
        duk_ret_t Context::duk_setDifference(duk_context * ctx) {
            
            int top = duk_get_top(ctx);
            
            if(top >0  && duk_is_number(ctx, -top)) {
                _difference = (TimeInterval) duk_to_number(ctx, -top);
            }
            
            return 0;
        }
        
        duk_ret_t Context::duk_on(duk_context * ctx) {
            
            int top = duk_get_top(ctx);
            
            if(top >1 && duk_is_number(ctx, -top) && duk_is_function(ctx, -top +1)) {
                
                TimeInterval timeInterval = duk_to_number(ctx, -top);
                kk::Strong func = new kk::script::Object(kk::script::GetContext(ctx),-top + 1);
                kk::Strong fn = new Function(timeInterval,func.as<kk::script::Object>());
                
                on(fn.as<Function>());
                
            }
            
            return 0;
        }
        
        duk_ret_t Context::duk_current(duk_context * ctx) {
            duk_push_number(ctx, current());
            return 1;
        }
        
        duk_ret_t Context::duk_frames(duk_context * ctx) {
            duk_push_uint(ctx, _frames);
            return 1;
        }
        
        duk_ret_t Context::duk_setFrames(duk_context * ctx) {
            int top = duk_get_top(ctx);
            if(top > 0 && duk_is_number(ctx, -top)) {
                setFrames(duk_to_uint(ctx, -top));
            }
            return 0;
        }
        
        Function::Function(TimeInterval timeInterval,CFunction cfunc,void * userData):timeInterval(timeInterval),cfunc(cfunc),userData(userData) {
            
        }
        
        Function::Function(TimeInterval timeInterval,kk::script::Object * jsfunc): timeInterval(timeInterval),cfunc(nullptr),userData(nullptr),jsfunc(jsfunc){
            
        }
        
        void Function::call(Context * context) {
            
            if(cfunc) {
                (*cfunc)(context,userData);
            }
            
            kk::script::Object * func = jsfunc.as<kk::script::Object>();
            
            if(func) {
                
                duk_context * ctx = func->jsContext();
                void * heapptr = func->heapptr();
                
                if(ctx && heapptr) {
                    
                    duk_push_heapptr(ctx, heapptr);
                    
                    if(duk_pcall(ctx, 0) != DUK_EXEC_SUCCESS) {
                        kk::script::Error(ctx, -1);
                    }
                    
                    duk_pop(ctx);
                }
            }
        }
        
    
        IMP_SCRIPT_CLASS_BEGIN(&kk::Element::ScriptClass, Element, GAElement)
        
        static kk::script::Method methods[] = {
            {"remove",(kk::script::Function) &Element::duk_remove},
        };
        
        kk::script::SetMethod(ctx, -1, methods, sizeof(methods) / sizeof(kk::script::Method));
        
        IMP_SCRIPT_CLASS_END
        
        
        Element::Element():_removed(false) {
            
        }
        
        void Element::exec(Context * context) {
            
            kk::Element * e = firstChild();
            
            while(e != nullptr) {
                
                Element * v = dynamic_cast<Element *>(e);
                
                if(v && v->_removed) {
                    e = e->nextSibling();
                    v->remove();
                    continue;
                }
                
                context->exec(e);
                e = e->nextSibling();
            }
        }
        
        void Element::setRemoved() {
            _removed = true;
        }
        
        duk_ret_t Element::duk_remove(duk_context * ctx) {
            _removed = true;
            return 0;
        }
        
        Float floatValue(String & value) {
            return atof(value.c_str());
        }
        
        kk::Int intValue(String & value) {
            return atoi(value.c_str());
        }
        
        Boolean booleanValue(String & value) {
            return value == "true" || value == "1" || value == "yes";
        }
        
        Float zIndexAutoY = 0x1.fffffep+127f;
    }
    
}
