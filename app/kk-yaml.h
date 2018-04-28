//
//  kk-yaml.h
//  KKGame
//
//  Created by zhanghailong on 2018/4/28.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef kk_yaml_h
#define kk_yaml_h

#include <yaml.h>
#include "kk-object.h"

namespace kk {
    
    
    class YObject {
    public:
        virtual void set(kk::CString key,yaml_document_t * document,yaml_node_t * node) = 0 ;
    };
    
    typedef std::vector<kk::Strong> YObjectArray;

    typedef std::map<kk::String,yaml_node_t *> YMap;
    
    typedef std::map<kk::String,kk::String> YStringMap;
    
    typedef std::map<kk::String,kk::Strong> YObjectMap;
    
    typedef kk::Strong (*YObjectAllocFunc)(yaml_document_t * document,yaml_node_t * node);
    
    void YMapSet(YMap & map, yaml_document_t * document,yaml_node_t * node);
    
    void YStringMapSet(YStringMap & map, yaml_document_t * document,yaml_node_t * node);
    
    void YObjectArraySet(YObjectArray & array, YObjectAllocFunc alloc,yaml_document_t * document,yaml_node_t * node);
    
    void YObjectSet(YObject * object, yaml_document_t * document,yaml_node_t * node);
    
    void YObjectMapSet(YObjectMap & map, YObjectAllocFunc alloc,yaml_document_t * document,yaml_node_t * node);
    
    kk::Boolean Y_isString(yaml_node_t * node,kk::CString key);
    
    kk::CString Y_toString(yaml_node_t * node,kk::CString defaultValue);
    
    kk::Int Y_toInt(yaml_node_t * node);
    
    kk::Float Y_toFloat(yaml_node_t * node);
    
}

#endif /* kk_yaml_hpp */
