//
//  kk-element.h
//  KKGame
//
//  Created by hailong11 on 2018/2/1.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef kk_element_h
#define kk_element_h

#include "kk-event.h"
#include <map>
#include <set>

namespace kk {
    
    class Element ;
    
    class ElementEvent : public Event {
    public:
        ElementEvent();
        ElementEvent(Element * element);
     
        virtual duk_ret_t duk_element(duk_context * ctx);
        virtual duk_ret_t duk_setElement(duk_context * ctx);
        virtual duk_ret_t duk_isCancelBubble(duk_context * ctx);
        virtual duk_ret_t duk_setCancelBubble(duk_context * ctx);
        virtual duk_ret_t duk_data(duk_context * ctx);
        virtual duk_ret_t duk_setData(duk_context * ctx);
        
        DEF_SCRIPT_CLASS
        
        Strong data;
        Strong element;
        bool cancelBubble;

    };
    
    class Element : public EventEmitter {
    public:
        Element();
        virtual ~Element();
        virtual Element * firstChild();
        virtual Element * lastChild();
        virtual Element * nextSibling();
        virtual Element * prevSibling();
        virtual Element * parent();
        virtual void append(Element * element);
        virtual void before(Element * element);
        virtual void after(Element * element);
        virtual void remove();
        virtual void appendTo(Element * element);
        virtual void beforeTo(Element * element);
        virtual void afterTo(Element * element);
        virtual String& get(String& key);
        virtual String& get(CString key);
        virtual void set(String& key,String& value);
        virtual void set(CString key,String& value);
        virtual void set(CString key,CString value);
        virtual void changedKey(String& key);
        virtual void changedKeys(std::set<String>& keys);
        virtual std::map<String,String> & style(String& name);
        virtual String& status();
        virtual void setStatus(String& status);
        virtual void addStatus(CString status);
        virtual void removeStatus(CString status);
        virtual Boolean hasStatus(CString status);
        virtual void changedStatus();
        static String NotFound;
        virtual void emit(String name,Event * event);
        virtual kk::Boolean has(String name);
        virtual String toString();
        virtual Int depth();
        virtual String uniqueId();
        virtual kk::Object * object(CString key);
        virtual void setObject(CString key,kk::Object * object);
        
        virtual duk_ret_t duk_uniqueId(duk_context * ctx);
        virtual duk_ret_t duk_set(duk_context * ctx);
        virtual duk_ret_t duk_get(duk_context * ctx);
        virtual duk_ret_t duk_firstChild(duk_context * ctx);
        virtual duk_ret_t duk_lastChild(duk_context * ctx);
        virtual duk_ret_t duk_nextSibling(duk_context * ctx);
        virtual duk_ret_t duk_prevSibling(duk_context * ctx);
        virtual duk_ret_t duk_parent(duk_context * ctx);
        virtual duk_ret_t duk_toString(duk_context * ctx);
        virtual duk_ret_t duk_append(duk_context * ctx);
        virtual duk_ret_t duk_before(duk_context * ctx);
        virtual duk_ret_t duk_after(duk_context * ctx);
        virtual duk_ret_t duk_remove(duk_context * ctx);
        virtual duk_ret_t duk_appendTo(duk_context * ctx);
        virtual duk_ret_t duk_beforeTo(duk_context * ctx);
        virtual duk_ret_t duk_afterTo(duk_context * ctx);
        virtual duk_ret_t duk_object(duk_context * ctx);
        virtual duk_ret_t duk_setObject(duk_context * ctx);
        virtual duk_ret_t duk_addStatus(duk_context * ctx);
        virtual duk_ret_t duk_removeStatus(duk_context * ctx);
        virtual duk_ret_t duk_changedStatus(duk_context * ctx);
        virtual duk_ret_t duk_hasStatus(duk_context * ctx);
        
        DEF_SCRIPT_CLASS
        
    protected:
        virtual void onDidAddChildren(Element * element);
        virtual void onDidAddToParent(Element * element);
        virtual void onWillRemoveChildren(Element * element);
        virtual void onWillRemoveFromParent(Element * element);
    protected:
        Strong _firstChild;
        Strong _lastChild;
        Strong _nextSibling;
        Weak _prevSibling;
        Weak _parent;
        std::map<String,String> _attributes;
        std::map<String,std::map<String,String>> _styles;
        std::map<String,Strong> _objects;
        Int _depth;
        Int _uniqueId;
    };
    
    
}

#endif /* kk_element_h */
