//
//  kk-script.c
//  KKGame
//
//  Created by zhanghailong on 2018/2/5.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include "kk-config.h"
#include "kk-script.h"

namespace kk {
    
    namespace script {
        
        static void Context_fatal_function (void *udata, const char *msg) {
            kk::Log("%s",msg);
        }
        
        static duk_ret_t Context_print_function (duk_context * ctx) {
            
            int top = duk_get_top(ctx);
            
            for(int i=0;i<top;i++) {
                
                if(duk_is_string(ctx, - top + i)) {
                    kk::Log("%s",duk_to_string(ctx, - top + i));
                } else if(duk_is_number(ctx, - top + i)) {
                    kk::Log("%g",duk_to_number(ctx, - top + i));
                } else if(duk_is_boolean(ctx, - top + i)) {
                    kk::Log("%s",duk_to_boolean(ctx, - top + i) ? "true":"false");
                } else if(duk_is_buffer_data(ctx, - top + i)) {
                    kk::Log("[bytes]:");
                    {
                        size_t n;
                        unsigned char * bytes = (unsigned char *) duk_get_buffer_data(ctx, - top + i, &n);
                        while(n >0) {
                            printf("%u",*bytes);
                            bytes ++;
                            n --;
                            if(n != 0) {
                                printf(",");
                            }
                        }
                        printf("\n");
                    }
                } else if(duk_is_function(ctx, - top + i)) {
                    kk::Log("[function]");
                } else if(duk_is_undefined(ctx, - top + i)) {
                    kk::Log("[undefined]");
                } else if(duk_is_null(ctx, - top + i)) {
                    kk::Log("[null]");
                } else {
                    kk::Log("%s",duk_json_encode(ctx, - top + i));
                    duk_pop(ctx);
                }
            
            }
            
            return 0;
        }
        
        Context::Context() {
            
            _jsContext = duk_create_heap(nullptr, nullptr, nullptr, nullptr, Context_fatal_function);
            
            duk_push_global_object(_jsContext);
            
            duk_push_string(_jsContext, "__jsContext");
            duk_push_pointer(_jsContext, this);
            duk_put_prop(_jsContext, -3);
            
            duk_push_string(_jsContext, "print");
            duk_push_c_function(_jsContext, Context_print_function, DUK_VARARGS);
            duk_put_prop(_jsContext, -3);
            
            duk_pop(_jsContext);
            
        }
        
        Context::~Context() {
            
            if(_jsContext) {
                duk_destroy_heap(_jsContext);
            }
            
        }
        
        duk_context * Context::jsContext() {
            return _jsContext;
        }
        
        kk::Object * Context::object(kk::CString key) {
            std::map<kk::String,kk::Strong>::iterator i = _objects.find(key);
            if(i != _objects.end()){
                return i->second.get();
            }
            return nullptr;
        }
        
        void Context::setObject(kk::CString key,kk::Object * object) {
            if(object == nullptr) {
                std::map<kk::String,kk::Strong>::iterator i = _objects.find(key);
                if(i != _objects.end()){
                    _objects.erase(i);
                }
            } else {
                _objects[key] = object;
            }
        }
        
        Object::Object(Context * context,duk_idx_t idx){
            duk_context * ctx = context->jsContext();
            _context = context;
            _heapptr = duk_get_heapptr(ctx, idx);
            if(_heapptr) {
                duk_push_global_object(ctx);
                duk_push_sprintf(ctx, "0x%x",_heapptr);
                duk_push_heapptr(ctx, _heapptr);
                duk_put_prop(ctx, -3);
                duk_pop(ctx);
            }
        }
        
        Object::~Object() {
            
            Context * context = _context.as<Context>();
            
            if(context && _heapptr) {
                duk_context * ctx = context->jsContext();
                duk_push_global_object(ctx);
                duk_push_sprintf(ctx, "0x%x",_heapptr);
                duk_del_prop(ctx, -2);
                duk_pop(ctx);
            }

        }
        
        Context * Object::context() {
            return _context.as<Context>();
        }
        
        duk_context * Object::jsContext() {
            
            Context * context = _context.as<Context>();
            
            if(context) {
                return context->jsContext();
            }
            return nullptr;
        }
        
        void * Object::heapptr() {
            return _heapptr;
        }
        
        Context * GetContext(duk_context * jsContext) {
            
            Context * v = nullptr;
            
            duk_push_global_object(jsContext);
            
            duk_push_string(jsContext, "__jsContext");
            duk_get_prop(jsContext, -2);
            
            if(duk_is_pointer(jsContext, -1)) {
                v = (Context *) duk_to_pointer(jsContext, -1);
            }
            
            duk_pop_n(jsContext, 2);
            
            return v;
        }
        
        static duk_ret_t ScriptObjectDeallocFunc(duk_context * ctx) {
            
            duk_push_string(ctx, "__object");
            duk_get_prop(ctx, -2);
            
            if(duk_is_pointer(ctx, -1)) {
                kk::Object * v = (kk::Object *) duk_to_pointer(ctx, -1);
                v->release();
            }
            
            duk_pop(ctx);
            
            return 0;
        }
        
        void PushObject(duk_context * ctx,kk::Object * object) {
            
            if(object == nullptr) {
                duk_push_null(ctx);
                return;
            }
            
            {
                Object * v = dynamic_cast<Object *>(object);
                if(v) {
                    duk_push_heapptr(ctx, v->heapptr());
                    return;
                }
            }
            
            duk_push_object(ctx);
            InitObject(ctx, -1, object);
        }
        
        void InitObject(duk_context * ctx,duk_idx_t idx,kk::Object * object) {
            
            duk_push_string(ctx, "__object");
            duk_push_pointer(ctx, object);
            duk_def_prop(ctx, idx - 2, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_HAVE_WRITABLE | DUK_DEFPROP_CLEAR_WRITABLE);
            
            object->retain();
            
            if(GetPrototype(ctx, object)) {
                duk_set_prototype(ctx, idx - 1);
            }
            
            duk_push_c_function(ctx, ScriptObjectDeallocFunc, 1);
            duk_set_finalizer(ctx, idx - 1);
            
        }
        
        kk::Object * GetObject(duk_context * ctx,duk_idx_t idx) {
            
            kk::Object * v = nullptr;
            
            duk_push_string(ctx, "__object");
            duk_get_prop(ctx, idx -1 );
            
            if(duk_is_pointer(ctx, -1)) {
                v = (kk::Object *) duk_to_pointer(ctx, -1);
            }
            
            duk_pop(ctx);
            
            return v;
        }
        
        bool ObjectEqual(Object * a, Object * b) {
            if(a == b) {
                return true;
            }
            if(a == nullptr || b == nullptr) {
                return false;
            }
            return a->heapptr() == b->heapptr();
        }
        
        void SetPrototype(duk_context * ctx,Class * isa) {
            
            duk_push_global_object(ctx);
            
            duk_push_string(ctx, isa->name);
            duk_push_object(ctx);
            
            duk_push_string(ctx, "alloc");
            duk_push_c_function(ctx, isa->alloc, 0);
            duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE |
                         DUK_DEFPROP_HAVE_WRITABLE | DUK_DEFPROP_CLEAR_WRITABLE |
                         DUK_DEFPROP_HAVE_ENUMERABLE | DUK_DEFPROP_SET_CONFIGURABLE);
            
            if(isa->prototype) {
                (*isa->prototype)(ctx);
            }
            
            if(isa->isa) {
                if(GetPrototype(ctx, isa->isa)) {
                    duk_set_prototype(ctx, -2);
                }
            }
            
            duk_put_prop(ctx, -3);
            
            duk_pop(ctx);
            
        }
        
        bool GetPrototype(duk_context * ctx,Class * isa) {
            
            if(isa == nullptr) {
                return false;
            }
            
            duk_push_global_object(ctx);
            
            duk_push_string(ctx, isa->name);
            duk_get_prop(ctx, -2);
            
            if(duk_is_object(ctx, -1)) {
                duk_remove(ctx, -2);
                return true;
            }
            
            duk_pop(ctx);
            
            duk_push_object(ctx);
            
            duk_push_string(ctx, "alloc");
            duk_push_c_function(ctx, isa->alloc, 0);
            duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE |
                         DUK_DEFPROP_HAVE_WRITABLE | DUK_DEFPROP_CLEAR_WRITABLE |
                         DUK_DEFPROP_HAVE_ENUMERABLE);
            
            if(isa->prototype) {
                (*isa->prototype)(ctx);
            }
            
            if(isa->isa) {
                if(GetPrototype(ctx, isa->isa)) {
                    duk_set_prototype(ctx, -2);
                }
            }
            
            duk_push_string(ctx, isa->name);
            duk_dup(ctx, -2);
            
            duk_put_prop(ctx, -4);
            
            duk_remove(ctx, -2);
            return true;
        }
        
        bool GetPrototype(duk_context * ctx,kk::Object * object) {
            
            if(object == nullptr) {
                return false;
            }
            
            IObject * v = dynamic_cast<IObject*>(object);
            
            if(v == nullptr) {
                return false;
            }
            
            return GetPrototype(ctx,v->getScriptClass());
        }
        
        static duk_ret_t ScriptObjectGetterFunc(duk_context * ctx) {
            
            Property * p = nullptr;
            
            duk_push_current_function(ctx);
            
            duk_push_string(ctx, "__object");
            
            duk_get_prop(ctx, -2);
            
            if(duk_is_pointer(ctx, -1)) {
                p = (Property *) duk_to_pointer(ctx, -1);
            }
            
            duk_pop_n(ctx, 2);
            
            duk_push_this(ctx);
            
            kk::Object * object = GetObject(ctx, -1);
            
            duk_pop(ctx);
            
            if(object && p && p->getter) {
                return (object->*p->getter)(ctx);
            }
            
            return 0;
        }
        
        static duk_ret_t ScriptObjectSetterFunc(duk_context * ctx) {
            
            Property * p = nullptr;
            
            duk_push_current_function(ctx);
            
            duk_push_string(ctx, "__object");
            
            duk_get_prop(ctx, -2);
            
            if(duk_is_pointer(ctx, -1)) {
                p = (Property *) duk_to_pointer(ctx, -1);
            }
            
            duk_pop_n(ctx, 2);
            
            duk_push_this(ctx);
            
            kk::Object * object = GetObject(ctx, -1);
            
            duk_pop(ctx);
            
            if(object && p && p->setter) {
                return (object->*p->setter)(ctx);
            }
            
            return 0;
        }
        
        void SetProperty(duk_context * ctx, duk_idx_t idx, Property * propertys, kk::Uint count) {
            Property * p = propertys;
            kk::Uint c = count;
            while(c >0 && p) {
                
                duk_push_string(ctx, p->name);
                
                duk_push_c_function(ctx, ScriptObjectGetterFunc, 0);
                duk_push_string(ctx, "__object");
                duk_push_pointer(ctx, p);
                duk_def_prop(ctx, -3,
                             DUK_DEFPROP_HAVE_VALUE |
                             DUK_DEFPROP_HAVE_WRITABLE | DUK_DEFPROP_CLEAR_WRITABLE |
                             DUK_DEFPROP_HAVE_ENUMERABLE);
                
                duk_push_c_function(ctx, ScriptObjectSetterFunc, 1);
                duk_push_string(ctx, "__object");
                duk_push_pointer(ctx, p);
                duk_def_prop(ctx, -3,
                             DUK_DEFPROP_HAVE_VALUE |
                             DUK_DEFPROP_HAVE_WRITABLE | DUK_DEFPROP_CLEAR_WRITABLE |
                             DUK_DEFPROP_HAVE_ENUMERABLE);
                
                duk_def_prop(ctx, -4, DUK_DEFPROP_HAVE_GETTER | DUK_DEFPROP_HAVE_SETTER |
                             DUK_DEFPROP_HAVE_ENUMERABLE | DUK_DEFPROP_SET_ENUMERABLE);
                
                c --;
                p ++;
            }
        }
        
        static duk_ret_t ScriptObjectInvokeFunc(duk_context * ctx) {
            
            Method * p = nullptr;
            
            duk_push_current_function(ctx);
            
            duk_push_string(ctx, "__object");
            
            duk_get_prop(ctx, -2);
            
            if(duk_is_pointer(ctx, -1)) {
                p = (Method *) duk_to_pointer(ctx, -1);
            }
            
            duk_pop_n(ctx, 2);
            
            duk_push_this(ctx);
            
            kk::Object * object= GetObject(ctx, -1);
            
            duk_pop(ctx);
            
            if(object && p && p->invoke) {
                kk::script::Function fn = p->invoke;
                return (object->*fn)(ctx);
            }
            
            return 0;
        }
        
        
        void SetMethod(duk_context * ctx, duk_idx_t idx, Method * methods, kk::Uint count) {
            
            Method * p = methods;
            Uint c = count;
            while(c >0 && p) {
                
                duk_push_string(ctx, p->name);
                
                duk_push_c_function(ctx, ScriptObjectInvokeFunc, DUK_VARARGS);
                duk_push_string(ctx, "__object");
                duk_push_pointer(ctx, p);
                duk_def_prop(ctx, -3,
                             DUK_DEFPROP_HAVE_VALUE |
                             DUK_DEFPROP_HAVE_WRITABLE | DUK_DEFPROP_CLEAR_WRITABLE |
                             DUK_DEFPROP_HAVE_ENUMERABLE);
                
                duk_def_prop(ctx, -3,
                             DUK_DEFPROP_HAVE_VALUE |
                             DUK_DEFPROP_HAVE_WRITABLE | DUK_DEFPROP_CLEAR_WRITABLE |
                             DUK_DEFPROP_HAVE_ENUMERABLE);
                
                c --;
                p ++;
            }
            
        }
        
        void Error(duk_context * ctx, duk_idx_t idx) {
            Error(ctx,idx,"");
        }
        
        void Error(duk_context * ctx, duk_idx_t idx, kk::CString prefix) {
            if(duk_is_error(ctx, idx)) {
                duk_get_prop_string(ctx, idx, "lineNumber");
                int lineNumber = duk_to_int(ctx, -1);
                duk_pop(ctx);
                duk_get_prop_string(ctx, idx, "stack");
                const char * error = duk_to_string(ctx, -1);
                duk_pop(ctx);
                duk_get_prop_string(ctx, idx, "fileName");
                const char * fileName = duk_to_string(ctx, -1);
                duk_pop(ctx);
                kk::Log("%s%s(%d): %s",prefix,fileName,lineNumber,error);
            } else {
                kk::Log("%s%s",prefix,duk_to_string(ctx, idx));
            }
        }
        
    }
}
