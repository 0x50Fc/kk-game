//
//  kk-yaml.cc
//  KKGame
//
//  Created by zhanghailong on 2018/4/28.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include "kk-config.h"
#include "kk-yaml.h"
#include "kk-string.h"

namespace kk {
    
    
    void YMapSet(YMap & map, yaml_document_t * document,yaml_node_t * node) {
        if(node->type == YAML_MAPPING_NODE) {
            yaml_node_pair_t * p = node->data.mapping.pairs.start;
            while(p != node->data.mapping.pairs.top) {
                yaml_node_t * key = yaml_document_get_node(document, p->key);
                map[Y_toString(key, "")] = yaml_document_get_node(document, p->value);
                p ++;
            }
        }
    }
    
    void YStringMapSet(YStringMap & map, yaml_document_t * document,yaml_node_t * node) {
        if(node->type == YAML_MAPPING_NODE) {
            yaml_node_pair_t * p = node->data.mapping.pairs.start;
            while(p != node->data.mapping.pairs.top) {
                yaml_node_t * key = yaml_document_get_node(document, p->key);
                yaml_node_t * value = yaml_document_get_node(document, p->value);
                map[Y_toString(key, "")] = Y_toString(value, "");
                p ++;
            }
        }
    }
    
    void YObjectArraySet(YObjectArray & array, YObjectAllocFunc alloc
                         ,yaml_document_t * document,yaml_node_t * node) {
        
        if(node->type == YAML_SEQUENCE_NODE) {
            
            yaml_node_item_t * p = node->data.sequence.items.start;
            
            while(p != node->data.sequence.items.top) {
                
                yaml_node_t * item = yaml_document_get_node(document, *p);
                
                kk::Strong v = (*alloc)(document,item);
                
                if(v.get()) {
                    array.push_back(v.get());
                }
                
                p ++;
            }
        }
    }
    
    void YObjectSet(YObject * object, yaml_document_t * document,yaml_node_t * node) {
        
        if(node->type == YAML_MAPPING_NODE) {
            yaml_node_pair_t * p = node->data.mapping.pairs.start;
            while(p != node->data.mapping.pairs.top) {
                yaml_node_t * key = yaml_document_get_node(document, p->key);
                object->set(Y_toString(key, ""), document,yaml_document_get_node(document, p->value));
                p ++;
            }
        }
        
        object->set(nullptr, document,node);
        
    }
    
    void YObjectMapSet(YObjectMap & map, YObjectAllocFunc alloc,yaml_document_t * document,yaml_node_t * node){
        if(node->type == YAML_MAPPING_NODE) {
            yaml_node_pair_t * p = node->data.mapping.pairs.start;
            while(p != node->data.mapping.pairs.top) {
                yaml_node_t * key = yaml_document_get_node(document, p->key);
                kk::Strong v = (*alloc)(document, yaml_document_get_node(document, p->value));
                if(v.get()) {
                    map[Y_toString(key, "")] = v.get();
                }
                p ++;
            }
        }
    }
    
    kk::Boolean Y_isString(yaml_node_t * node,kk::CString key) {
        return kk::CStringEqual(Y_toString(node, nullptr), key);
    }
    
    kk::CString Y_toString(yaml_node_t * node,kk::CString defaultValue) {
        if(node->type == YAML_SCALAR_NODE) {
            return (kk::CString) node->data.scalar.value;
        }
        return nullptr;
    }
    
    kk::Int Y_toInt(yaml_node_t * node) {
        return atoi(Y_toString(node, "0"));
    }
    
    kk::Float Y_toFloat(yaml_node_t * node) {
        return atof(Y_toString(node, "0"));
    }
}
