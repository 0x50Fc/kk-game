//
//  kk-yaml.h
//  KKGame
//
//  Created by zhanghailong on 2018/4/28.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef kk_yaml_h
#define kk_yaml_h

#if defined(KK_PLATFORM_IOS)

#include <KKObject/KKObject.h>

#else

#include "kk-script.h"
#include "kk-object.h"

#endif



namespace kk {
    
    
    class YObject {
    public:
        virtual void set(kk::CString key,duk_context * ctx,duk_idx_t idx) = 0 ;
    };
    
    typedef std::vector<kk::Strong> YObjectArray;
    
    typedef std::map<kk::String,kk::String> YStringMap;
    
    typedef std::map<kk::String,kk::Strong> YObjectMap;
    
    typedef kk::Strong (*YObjectAllocFunc)(duk_context * ctx,duk_idx_t idx);
    
    void YStringMapSet(YStringMap & map, duk_context * ctx,duk_idx_t idx);
    
    void YObjectArraySet(YObjectArray & array, YObjectAllocFunc alloc,duk_context * ctx,duk_idx_t idx);
    
    void YObjectSet(YObject * object, duk_context * ctx,duk_idx_t idx);
    
    void YObjectMapSet(YObjectMap & map, YObjectAllocFunc alloc,duk_context * ctx,duk_idx_t idx);
    
    kk::Boolean Y_isString(duk_context * ctx,duk_idx_t idx,kk::CString key);
    
    kk::CString Y_toString(duk_context * ctx,duk_idx_t idx,kk::CString defaultValue);
    
    kk::CString Y_toString(duk_context * ctx,duk_idx_t idx,kk::CString key, kk::CString defaultValue);
    
    kk::Int Y_toInt(duk_context * ctx,duk_idx_t idx);
    
    kk::Int Y_toInt(duk_context * ctx,duk_idx_t idx,kk::CString key);
    
    kk::Float Y_toFloat(duk_context * ctx,duk_idx_t idx);
    
    kk::Float Y_toFloat(duk_context * ctx,duk_idx_t idx,kk::CString key);
    
    void * Y_toHeapptr(duk_context * ctx,duk_idx_t idx,kk::CString key);
    
    void * Y_toHeapptr(duk_context * ctx,void * heapptr,kk::CString key);

    kk::CString Y_toString(duk_context * ctx,void * heapptr,kk::CString key, kk::CString defaultValue);
    
    kk::Int Y_toInt(duk_context * ctx,void * heapptr,kk::CString key);
    
    kk::Float Y_toFloat(duk_context * ctx,void * heapptr,kk::CString key);
    
}

#endif /* kk_yaml_hpp */
