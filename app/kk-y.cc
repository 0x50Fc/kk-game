//
//  kk-yaml.cc
//  KKGame
//
//  Created by zhanghailong on 2018/4/28.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include "kk-config.h"
#include "kk-y.h"
#include "kk-string.h"

namespace kk {
    
    
    void YStringMapSet(YStringMap & map, duk_context * ctx, duk_idx_t idx) {
        if(duk_is_object(ctx, idx)) {
            duk_enum(ctx, idx, DUK_ENUM_INCLUDE_SYMBOLS);
            while(duk_next(ctx, -1, 1)) {
                map[Y_toString(ctx,-2, "")] = Y_toString(ctx, -1, "");
                duk_pop_2(ctx);
            }
            duk_pop(ctx);
        }
    }
    
    void YObjectArraySet(YObjectArray & array, YObjectAllocFunc alloc
                         ,duk_context * ctx, duk_idx_t idx) {
        
        if(duk_is_array(ctx, idx)) {
            duk_size_t n = duk_get_length(ctx, idx);
            for(duk_idx_t i = 0;i<n;i++ ){
                duk_get_prop_index(ctx, -1, i);
                kk::Strong v = (*alloc)(ctx,-1);
                if(v.get()) {
                    array.push_back(v.get());
                }
                duk_pop(ctx);
            }
        }
    }
    
    void YObjectSet(YObject * object, duk_context * ctx, duk_idx_t idx) {
        
        if(duk_is_object(ctx, idx)) {
            duk_enum(ctx, idx, DUK_ENUM_INCLUDE_SYMBOLS);
            while(duk_next(ctx, -1, 1)) {
                object->set(Y_toString(ctx,-2, ""), ctx,-1);
                duk_pop_2(ctx);
            }
            duk_pop(ctx);
        }

        object->set(nullptr, ctx,idx);
        
    }
    
    void YObjectMapSet(YObjectMap & map, YObjectAllocFunc alloc,duk_context * ctx, duk_idx_t idx){
        
        if(duk_is_object(ctx, idx)) {
            duk_enum(ctx, idx, DUK_ENUM_INCLUDE_SYMBOLS);
            while(duk_next(ctx, -1, 1)) {
                kk::Strong v = (*alloc)(ctx,-1);
                if(v.get()) {
                    map[Y_toString(ctx,-2, "")] = v.get();
                }
                duk_pop_2(ctx);
            }
            duk_pop(ctx);
        }
    
    }
    
    kk::Boolean Y_isString(duk_context * ctx, duk_idx_t idx, kk::CString key) {
        return kk::CStringEqual(key, Y_toString(ctx, idx, nullptr)) ;
    }
    
    kk::CString Y_toString(duk_context * ctx, duk_idx_t idx,kk::CString defaultValue) {
        return duk_is_string(ctx, idx) ? duk_to_string(ctx, idx)  : defaultValue;
    }
    
    kk::CString Y_toString(duk_context * ctx,duk_idx_t idx,kk::CString key, kk::CString defaultValue) {
        duk_get_prop_string(ctx, idx, key);
        kk::CString v = Y_toString(ctx, -1, defaultValue);
        duk_pop(ctx);
        return v;
    }
    
    kk::Int Y_toInt(duk_context * ctx,duk_idx_t idx) {
        if(duk_is_number(ctx, -1)) {
            return duk_to_int(ctx, idx);
        } else if(duk_is_string(ctx, -1)) {
            return atoi(duk_to_string(ctx, -1));
        }
        return 0;
    }
    
    kk::Int Y_toInt(duk_context * ctx,duk_idx_t idx,kk::CString key) {
        duk_get_prop_string(ctx, idx, key);
        kk::Int v = Y_toInt(ctx, -1);
        duk_pop(ctx);
        return v;
    }
    
    kk::Float Y_toFloat(duk_context * ctx,duk_idx_t idx) {
        if(duk_is_number(ctx, -1)) {
            return duk_to_number(ctx, idx);
        } else if(duk_is_string(ctx, -1)) {
            return atof(duk_to_string(ctx, -1));
        }
        return 0;
    }
    
    kk::Float Y_toFloat(duk_context * ctx,duk_idx_t idx,kk::CString key) {
        duk_get_prop_string(ctx, idx, key);
        kk::Float v = Y_toFloat(ctx, -1);
        duk_pop(ctx);
        return v;
    }
    
    void * Y_toHeapptr(duk_context * ctx,duk_idx_t idx,kk::CString key) {
        void* v = nullptr;
        duk_get_prop_string(ctx, idx, key);
        if(duk_is_object(ctx, -1)) {
            v = duk_get_heapptr(ctx, -1);
        }
        duk_pop(ctx);
        return v;
    }
    
    void * Y_toHeapptr(duk_context * ctx,void * heapptr,kk::CString key) {
        void * v = nullptr;
        if(heapptr) {
            duk_push_heapptr(ctx, heapptr);
            v = Y_toHeapptr(ctx, -1, key);
            duk_pop(ctx);
        }
        return v;
    }
    
    kk::CString Y_toString(duk_context * ctx,void * heapptr,kk::CString key, kk::CString defaultValue) {
        kk::CString v = nullptr;
        if(heapptr) {
            duk_push_heapptr(ctx, heapptr);
            v = Y_toString(ctx, -1, key,defaultValue);
            duk_pop(ctx);
        }
        return v;
    }
    
    kk::Int Y_toInt(duk_context * ctx,void * heapptr,kk::CString key) {
        kk::Int v = 0;
        if(heapptr) {
            duk_push_heapptr(ctx, heapptr);
            v = Y_toInt(ctx, -1, key);
            duk_pop(ctx);
        }
        return v;
    }
    
    kk::Float Y_toFloat(duk_context * ctx,void * heapptr,kk::CString key) {
        kk::Float v = 0;
        if(heapptr) {
            duk_push_heapptr(ctx, heapptr);
            v = Y_toFloat(ctx, -1, key);
            duk_pop(ctx);
        }
        return v;
    }
    
}
