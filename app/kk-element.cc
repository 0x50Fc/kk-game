//
//  kk-element.c
//  KKGame
//
//  Created by zhanghailong on 2018/2/1.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//


#include "kk-element.h"
#include "kk-crypto.h"
#include <sstream>
#include "kk-string.h"

namespace kk {
    
    IMP_SCRIPT_CLASS_BEGIN(&Event::ScriptClass, ElementEvent, ElementEvent)
    
    static kk::script::Property propertys[] = {
        {"cancelBubble",(kk::script::Function) &ElementEvent::duk_isCancelBubble,(kk::script::Function) &ElementEvent::duk_setCancelBubble},
        {"data",(kk::script::Function) &ElementEvent::duk_data,(kk::script::Function) &ElementEvent::duk_setData},
        {"element",(kk::script::Function) &ElementEvent::duk_element,(kk::script::Function) &ElementEvent::duk_setElement},
    };
    
    kk::script::SetProperty(ctx, -1, propertys, sizeof(propertys) / sizeof(kk::script::Property));
    
    IMP_SCRIPT_CLASS_END
    
    ElementEvent::ElementEvent():element(nullptr),cancelBubble(false),data(nullptr) {
        
    }
    
    ElementEvent::ElementEvent(Element * element):element(element),cancelBubble(false),data(nullptr) {
        
    }
    
    duk_ret_t ElementEvent::duk_element(duk_context * ctx) {
        
        Element * e = element.as<Element>();
        
        if(e) {
            kk::script::PushObject(ctx, e);
            return 1;
        }
        
        return 0;
    }
    
    duk_ret_t ElementEvent::duk_setElement(duk_context * ctx) {
        
        int top = duk_get_top(ctx);
        
        if(top >0 && duk_is_object(ctx, -top)) {
            kk::Object * v = kk::script::GetObject(ctx, -top);
            kk::Element * e = dynamic_cast<kk::Element *>(v);
            element = e;
        }
        
        return 0;
    }
    
    duk_ret_t ElementEvent::duk_isCancelBubble(duk_context * ctx) {
        duk_push_boolean(ctx, cancelBubble);
        return 1;
    }
    
    duk_ret_t ElementEvent::duk_setCancelBubble(duk_context * ctx) {
        
        int top = duk_get_top(ctx);
        
        if(top >0 && duk_is_boolean(ctx, -top)) {
            cancelBubble = duk_to_boolean(ctx, -top);
        }
        
        return 0;
    }
    
    duk_ret_t ElementEvent::duk_data(duk_context * ctx) {
        kk::script::PushObject(ctx, data.get());
        return 1;
    }
    
    duk_ret_t ElementEvent::duk_setData(duk_context * ctx) {
        
        int top = duk_get_top(ctx);
        
        if(top >0 && duk_is_object(ctx, - top)) {
            Object * v = kk::script::GetObject(ctx, - top);
            if(v == nullptr) {
                data = new kk::script::Object(kk::script::GetContext(ctx), - top);
            } else {
                data = v;
            }
        }
        
        return 0;
    }
    
    IMP_SCRIPT_CLASS_BEGIN(&EventEmitter::ScriptClass, Element, Element)
    
    static kk::script::Method methods[] = {
        {"uniqueId",(kk::script::Function) &Element::duk_uniqueId},
        {"get",(kk::script::Function) &Element::duk_get},
        {"set",(kk::script::Function) &Element::duk_set},
        {"toString",(kk::script::Function) &Element::duk_toString},
        {"append",(kk::script::Function) &Element::duk_append},
        {"before",(kk::script::Function) &Element::duk_before},
        {"after",(kk::script::Function) &Element::duk_after},
        {"remove",(kk::script::Function) &Element::duk_remove},
        {"appendTo",(kk::script::Function) &Element::duk_appendTo},
        {"beforeTo",(kk::script::Function) &Element::duk_beforeTo},
        {"afterTo",(kk::script::Function) &Element::duk_afterTo},
        {"object",(kk::script::Function) &Element::duk_object},
        {"setObject",(kk::script::Function) &Element::duk_setObject},
        {"addStatus",(kk::script::Function) &Element::duk_addStatus},
        {"removeStatus",(kk::script::Function) &Element::duk_removeStatus},
        {"hasStatus",(kk::script::Function) &Element::duk_hasStatus},
        {"changedStatus",(kk::script::Function) &Element::duk_changedStatus},
    };
    
    kk::script::SetMethod(ctx, -1, methods, sizeof(methods) / sizeof(kk::script::Method));
    
    static kk::script::Property propertys[] = {
        {"firstChild",(kk::script::Function) &Element::duk_firstChild,(kk::script::Function) nullptr},
        {"lastChild",(kk::script::Function) &Element::duk_lastChild,(kk::script::Function) nullptr},
        {"nextSibling",(kk::script::Function) &Element::duk_nextSibling,(kk::script::Function) nullptr},
        {"prevSibling",(kk::script::Function) &Element::duk_prevSibling,(kk::script::Function) nullptr},
        {"parent",(kk::script::Function) &Element::duk_parent,(kk::script::Function) nullptr},
    };
    
    kk::script::SetProperty(ctx, -1, propertys, sizeof(propertys) / sizeof(kk::script::Property));
    
    
    IMP_SCRIPT_CLASS_END
    
    
    Element::Element():_depth(0),_uniqueId(0) {
        
    }
    
    Element::~Element() {
        
    }
    
    Element * Element::firstChild() {
        return (Element *) _firstChild.get();
    }
    
    Element * Element::lastChild() {
        return (Element *) _lastChild.get();
    }
    
    Element * Element::nextSibling() {
        return (Element *) _nextSibling.get();
    }
    
    Element * Element::prevSibling() {
        return (Element *) _prevSibling.get();
    }
    
    Element * Element::parent() {
        return (Element *) _parent.get();
    }
    
    void Element::append(Element * element) {
        
        if(element == nullptr) {
            return;
        }
        
        Strong e(element);
        
        element->remove();
        
        Element * lastChild = this->lastChild();
        
        if(lastChild) {
            lastChild->_nextSibling = element;
            element->_prevSibling = lastChild;
            _lastChild = element;
            element->_parent = this;
        } else {
            _firstChild = _lastChild = element;
            element->_parent = this;
        }
        
        onDidAddChildren(element);
        
    }
    
    void Element::before(Element * element) {
        
        if(element == nullptr) {
            return;
        }
        
        Strong e(element);
        
        element->remove();
        
        Element * prevSibling = this->prevSibling();
        Element * parent = this->parent();
        
        if(prevSibling) {
            prevSibling->_nextSibling = element;
            element->_prevSibling = prevSibling;
            element->_nextSibling = this;
            element->_parent = parent;
            _prevSibling = element;
        } else if(parent) {
            element->_nextSibling = this;
            element->_parent = parent;
            _prevSibling = element;
            parent->_firstChild = element;
        }
        
        if(parent) {
            parent->onDidAddChildren(element);
        }
    }
    
    void Element::after(Element * element) {
        
        if(element == nullptr){
            return;
        }
        
        Strong e(element);
        
        element->remove();
        
        Element * nextSibling = this->nextSibling();
        Element * parent = this->parent();
        
        if(nextSibling) {
            nextSibling->_prevSibling = element;
            element->_nextSibling = nextSibling;
            element->_prevSibling = this;
            element->_parent = parent;
            _nextSibling = element;
        } else if(parent) {
            element->_prevSibling = this;
            element->_parent = parent;
            _nextSibling = element;
            parent->_lastChild = element;
        }
        
        if(parent) {
            parent->onDidAddChildren(element);
        }
    }
    
    void Element::remove() {
        
        Strong e(this);
        
        Element * prevSibling = this->prevSibling();
        Element * nextSibling = this->nextSibling();
        Element * parent = this->parent();
        
        if(prevSibling) {
            parent->onWillRemoveChildren(this);
            
            prevSibling->_nextSibling = nextSibling;
            if(nextSibling) {
                nextSibling->_prevSibling = prevSibling;
            } else {
                parent->_lastChild = prevSibling;
            }
        } else if(parent) {
            parent->onWillRemoveChildren(this);
            
            parent->_firstChild = nextSibling;
            if(nextSibling) {
                nextSibling->_prevSibling = (Element *) nullptr;
            } else {
                parent->_lastChild = (Element *) nullptr;
            }
        }
    }
    
    void Element::appendTo(Element * element) {
        if(element != nullptr) {
            element->append(this);
        }
    }
    
    void Element::beforeTo(Element * element) {
        if(element != nullptr) {
            element->before(this);
        }
    }
    
    void Element::afterTo(Element * element) {
        if(element != nullptr) {
            element->after(this);
        }
    }
    
    void Element::onDidAddChildren(Element * element) {
        element->_depth = _depth + 1;
        element->onDidAddToParent(this);
    }
    
    void Element::onWillRemoveChildren(Element * element) {
        element->_depth = 0;
        element->onWillRemoveFromParent(this);
    }
    
    void Element::onDidAddToParent(Element * element) {
        
    }
    
    void Element::onWillRemoveFromParent(Element * element) {
        
    }
    
    String& Element::get(CString key) {
        String skey(key);
        return get(skey);
    }
    
    String& Element::get(String& key) {
        {
            std::map<String,String>::iterator i = _attributes.find(key);
            if(i != _attributes.end()) {
                return i->second;
            }
        }
        {
            String& status = this->status();
            std::vector<String> vs;
            CStringSplit(status.c_str(), " ", vs);
            vs.push_back("");
            
            std::vector<String>::iterator vi = vs.begin();
            
            while(vi != vs.end()) {
                
                std::map<String,std::map<String,String>>::iterator i = _styles.find(*vi);
                
                if(i != _styles.end()) {
                    std::map<String,String>& attrs = i->second;
                    std::map<String,String>::iterator ii = attrs.find(key);
                    if(ii != attrs.end()) {
                        return ii->second;
                    }
                }
                
                vi ++;
            }
            
        }
        return Element::NotFound;
    }
    
    void Element::set(CString key,String& value) {
        String skey(key);
        set(skey,value);
    }
    
    void Element::set(CString key,CString value) {
        String skey(key);
        String svalue(value);
        set(skey,svalue);
    }
    
    void Element::set(String& key,String& value) {
        
        {
            Boolean css = false;
            String name = "";
            if(key == "style") {
                css = true;
            } else if(CStringHasPrefix(key.c_str(), "style:")) {
                name = key.substr(6);
                css = true;
            }
            
            if(css) {
                
                std::set<String> keys;
                std::map<String,String> & style = this->style(name);
                std::vector<String> items;
                CStringSplit(value.c_str(), ";", items);
                
                std::vector<String>::iterator i = items.begin();
                
                while(i != items.end()) {
                    std::vector<String> kv;
                    CStringSplit((* i).c_str(), ":", kv);
                    if(kv.size() > 1) {
                        String& key = CStringTrim(kv[0]);
                        keys.insert(key);
                        style[key] = CStringTrim(kv[1]);
                    }
                    i ++;
                }
                
                changedKeys(keys);
                
                return;
            }
        }
        
        if(&value == & Element::NotFound) {
            std::map<String, String>::iterator i =_attributes.find(key);
            if(i != _attributes.end()) {
                _attributes.erase(i);
            }
        } else {
            _attributes[key] = value;
        }
        
        if(key == "status" || key == "in-status") {
            changedStatus();
        } else {
            std::set<String> keys;
            keys.insert(key);
            changedKeys(keys);
        }
    }
    
    Int Element::depth() {
        return _depth;
    }
    
    String Element::uniqueId() {
        std::ostringstream s;
        s << get("#id") ;
        String& v = get("name");
        if(&v == &Element::NotFound) {
            _uniqueId ++;
            s << "[" << _uniqueId << "]" << std::ends;
        } else {
            s << v << std::ends;
        }
        return Crypto_MD5(s.str().c_str());
    }
    
    String Element::toString() {
        
        String ss;
        
        String name = get("#name");
        
        if(name == "") {
            name = "element";
        }
        
        for(int j = 0;j < _depth;j++) {
            ss.append("\t");
        }
        
        ss.append("<").append(name);
        
        {
            std::map<String, String>::iterator i = _attributes.begin();
            
            while(i != _attributes.end()) {
                if(!CStringHasPrefix(i->first.c_str(), "#")) {
                    ss.append(" ").append(i->first).append("=\"");
                    ss.append(i->second).append("\"");
                }
                i ++;
            }
        }
        
        {
            std::map<String, std::map<String, String>>::iterator i = _styles.begin();
            
            while(i != _styles.end()) {
                
                if(i->first == "") {
                    ss.append(" style=\"");
                } else {
                    ss.append(" style:").append(i->first).append("=\"");
                }
                
                std::map<String, String> & attrs = i->second;
                std::map<String, String>::iterator n = attrs.begin();
                
                while(n != attrs.end()) {
                    ss.append(n->first).append(": ").append(n->second).append("; ");
                    n ++;
                }
                
                ss.append("\"");
                i ++;
            }
        }
        
        ss.append(">");
        
        Element * p = firstChild();
        
        if(p) {
            
            ss.append("\n");
            
            while(p) {
                ss.append(p->toString()).append("\n");
                p = p->nextSibling();
            }
            
            for(int j = 0;j < _depth;j++) {
                ss.append("\t");
            }
            
        } else {
            ss.append(get("#text"));
        }
        
        ss.append("</").append(name).append(">");
        
        return ss;
    }
    
    duk_ret_t Element::duk_uniqueId(duk_context * ctx) {
        duk_push_string(ctx, uniqueId().c_str());
        return 1;
    }
    
    duk_ret_t Element::duk_set(duk_context * ctx) {
        
        int nargs = duk_get_top(ctx);
        
        const char * key = nullptr;
        const char * value = nullptr;
        
        if(nargs > 0 && duk_is_string(ctx, - nargs)) {
            key = duk_to_string(ctx, - nargs);
        }
        
        if(nargs > 1 && duk_is_string(ctx, - nargs + 1)) {
            value = duk_to_string(ctx, - nargs + 1);
        }
        
        if(key) {
            
            String skey(key);
            
            if(value) {
                String svalue(value);
                set(skey, svalue);
            } else {
                set(skey, Element::NotFound);
            }
        }
        
        return 0;
    }
    
    duk_ret_t Element::duk_get(duk_context * ctx) {
        
        int nargs = duk_get_top(ctx);
        
        const char * key = nullptr;
        
        if(nargs > 0 && duk_is_string(ctx, - nargs)) {
            key = duk_to_string(ctx, - nargs);
        }
        
        
        if(key) {
            
            String skey(key);
            
            String& v = get(skey);
            
            if(&v != &Element::NotFound) {
                duk_push_string(ctx, v.c_str());
                return 1;
            }
        }
        
        return 0;
    }
    
    duk_ret_t Element::duk_firstChild(duk_context * ctx) {
        Element * e = firstChild();
        if(e) {
            kk::script::PushObject(ctx,e);
            return 1;
        }
        return 0;
    }
    
    duk_ret_t Element::duk_lastChild(duk_context * ctx) {
        Element * e = lastChild();
        if(e) {
            kk::script::PushObject(ctx,e);
            return 1;
        }
        return 0;
    }
    
    duk_ret_t Element::duk_nextSibling(duk_context * ctx) {
        Element * e = nextSibling();
        if(e) {
            kk::script::PushObject(ctx,e);
            return 1;
        }
        return 0;
    }
    
    duk_ret_t Element::duk_prevSibling(duk_context * ctx) {
        Element * e = prevSibling();
        if(e) {
            kk::script::PushObject(ctx,e);
            return 1;
        }
        return 0;
    }
    
    duk_ret_t Element::duk_parent(duk_context * ctx) {
        Element * e = parent();
        if(e) {
            kk::script::PushObject(ctx,e);
            return 1;
        }
        return 0;
    }
    
    duk_ret_t Element::duk_append(duk_context * ctx) {
        int top = duk_get_top(ctx);
        if(top > 0 && duk_is_object(ctx, -top)) {
            kk::Element * e =dynamic_cast<kk::Element *>(kk::script::GetObject(ctx, -top));
            if(e) {
                append(e);
            }
        }
        return 0;
    }
    
    duk_ret_t Element::duk_before(duk_context * ctx) {
        int top = duk_get_top(ctx);
        if(top > 0 && duk_is_object(ctx, -top)) {
            kk::Element * e =dynamic_cast<kk::Element *>(kk::script::GetObject(ctx, -top));
            if(e) {
                before(e);
            }
        }
        return 0;
    }
    
    duk_ret_t Element::duk_after(duk_context * ctx) {
        int top = duk_get_top(ctx);
        if(top > 0 && duk_is_object(ctx, -top)) {
            kk::Element * e =dynamic_cast<kk::Element *>(kk::script::GetObject(ctx, -top));
            if(e) {
                after(e);
            }
        }
        return 0;
    }
    
    duk_ret_t Element::duk_remove(duk_context * ctx) {
        remove();
        return 0;
    }
    
    duk_ret_t Element::duk_appendTo(duk_context * ctx) {
        int top = duk_get_top(ctx);
        if(top > 0 && duk_is_object(ctx, -top)) {
            kk::Element * e =dynamic_cast<kk::Element *>(kk::script::GetObject(ctx, -top));
            if(e) {
                appendTo(e);
            }
        }
        return 0;
    }
    
    duk_ret_t Element::duk_beforeTo(duk_context * ctx) {
        int top = duk_get_top(ctx);
        if(top > 0 && duk_is_object(ctx, -top)) {
            kk::Element * e =dynamic_cast<kk::Element *>(kk::script::GetObject(ctx, -top));
            if(e) {
                beforeTo(e);
            }
        }
        return 0;
    }
    
    duk_ret_t Element::duk_afterTo(duk_context * ctx) {
        int top = duk_get_top(ctx);
        if(top > 0 && duk_is_object(ctx, -top)) {
            kk::Element * e =dynamic_cast<kk::Element *>(kk::script::GetObject(ctx, -top));
            if(e) {
                afterTo(e);
            }
        }
        return 0;
    }
    
    duk_ret_t Element::duk_toString(duk_context * ctx) {
        duk_push_string(ctx, toString().c_str());
        return 1;
    }
    
    String& Element::status() {
        std::map<String, String>::iterator i = _attributes.find("status");
        if(i != _attributes.end()) {
            return i->second;
        }
        i = _attributes.find("in-status");
        if(i != _attributes.end()) {
            return i->second;
        }
        return Element::NotFound;
    }
    
    void Element::setStatus(String& status) {
        String key("status");
        set(key, status);
    }
    
    void Element::addStatus(CString status) {
        String s(status);
        
        std::set<String> vs;
        {
            std::map<String, String>::iterator i = _attributes.find("status");
            if(i != _attributes.end()) {
                CStringSplit(i->second.c_str(), " ", vs);
            }
        }
        {
            std::set<String>::iterator i = vs.find(s);
            if(i == vs.end()) {
                vs.insert(s);
                _attributes["status"] = CStringJoin(vs, " ");
            }
        }
    }
    
    void Element::removeStatus(CString status) {
        String s(status);
        std::set<String> vs;
        {
            std::map<String, String>::iterator i = _attributes.find("status");
            if(i != _attributes.end()) {
                CStringSplit(i->second.c_str(), " ", vs);
            }
        }
        {
            std::set<String>::iterator i = vs.find(s);
            if(i != vs.end()) {
                vs.erase(i);
                _attributes["status"] = CStringJoin(vs, " ");
            }
        }
    }
    
    Boolean Element::hasStatus(CString status) {
        std::set<String> vs;
        {
            std::map<String, String>::iterator i = _attributes.find("status");
            if(i != _attributes.end()) {
                CStringSplit(i->second.c_str(), " ", vs);
            }
        }
        {
            std::set<String>::iterator i = vs.find(status);
            return i != vs.end();
        }
        return false;
    }
    
    void Element::changedStatus() {
        std::set<String> keys;
 
        String & value = this->status();
        
        std::vector<String> vs;
        
        vs.push_back("");
        
        CStringSplit(value.c_str(), " ", vs);
        
        std::vector<String>::iterator vi = vs.begin();
        
        while(vi != vs.end()) {
            std::map<String,std::map<String,String>>::iterator i = _styles.find(*vi);
            if(i != _styles.end()) {
                std::map<String,String> & attrs = i->second;
                std::map<String,String>::iterator ikey = attrs.begin();
                while(ikey != attrs.end()) {
                    keys.insert(ikey->first);
                    ikey ++;
                }
            }
            vi ++;
        }
        
        changedKeys(keys);
        
        {
            Element * e = firstChild();
            while(e) {
                String & v = e->get("status");
                if(&v == &Element::NotFound) {
                    e->set("in-status",value);
                }
                e = e->nextSibling();
            }
        }
    }
    
    void Element::changedKey(String& key) {
        
    }
    
    void Element::changedKeys(std::set<String>& keys) {
        std::set<String>::iterator i = keys.begin();
        while(i != keys.end()) {
            String v = *i;
            changedKey(v);
            i ++;
        }
    }
    
    std::map<String,String> & Element::style(String& name) {
        std::map<String,std::map<String,String>>::iterator i = _styles.find(name);
        if(i == _styles.end()) {
            _styles[name] = std::map<String,String>();
        }
        return _styles.find(name)->second;
    }
    
    String Element::NotFound;
    
    void Element::emit(String name,Event * event) {
        
        ElementEvent * e = dynamic_cast<ElementEvent *>(event);
        
        if(e && e->element.get() == nullptr)  {
            e->element = this;
        }
        
        EventEmitter::emit(name, event);
        
        if(e && !e->cancelBubble) {
            Element * p = parent();
            if(p) {
                p->emit(name, event);
            }
        }
    }
    
    kk::Boolean Element::has(String name) {
        
        if(EventEmitter::has(name)) {
            return true;
        }
        
        Element * p = parent();
        
        if(p != nullptr) {
            return p->has(name);
        }
        
        return false;
    }
    
    kk::Object * Element::object(CString key) {
        
        std::map<String,Strong>::iterator i = _objects.find(key);
        
        if(i != _objects.end()) {
            return i->second.get();
        }
        
        return nullptr;
    }
    
    void Element::setObject(CString key,kk::Object * object) {
        if(object == nullptr) {
            std::map<String,Strong>::iterator i = _objects.find(key);
            if(i != _objects.end()) {
                _objects.erase(i);
            }
        } else {
            _objects[key] = object;
        }
    }
    
    duk_ret_t Element::duk_object(duk_context * ctx) {
        
        int top = duk_get_top(ctx);
        
        if(top > 0 && duk_is_string(ctx, -top)) {
            kk::Object * v = this->object(duk_to_string(ctx, -top));
            if(v) {
                kk::script::PushObject(ctx, v);
                return 1;
            }
        }
        
        return 0;
    }
    
    duk_ret_t Element::duk_setObject(duk_context * ctx) {
        
        int top = duk_get_top(ctx);
        
        if(top > 1 && duk_is_string(ctx, -top)) {
            CString key = duk_to_string(ctx, -top);
            Strong v = new kk::script::Object(kk::script::GetContext(ctx),-top +1);
            setObject(key, v.get());
        }
        
        return 0;
    }
    
    duk_ret_t Element::duk_addStatus(duk_context * ctx) {
        
        int top = duk_get_top(ctx);
        
        if(top > 0 && duk_is_string(ctx, -top)) {
            addStatus(duk_to_string(ctx, -top));
        }
        
        return 0;
    }
    
    duk_ret_t Element::duk_removeStatus(duk_context * ctx) {
        
        int top = duk_get_top(ctx);
        
        if(top > 0 && duk_is_string(ctx, -top)) {
            removeStatus(duk_to_string(ctx, -top));
        }
        
        return 0;
    }
    
    duk_ret_t Element::duk_changedStatus(duk_context * ctx) {
        
        changedStatus();
        
        return 0;
    }
    
    duk_ret_t Element::duk_hasStatus(duk_context * ctx) {
        
        int top = duk_get_top(ctx);
        
        if(top > 0 && duk_is_string(ctx, -top)) {
            duk_push_boolean(ctx, hasStatus(duk_to_string(ctx, -top)));
            return 1;
        }
        
        return 0;
    }
    
}
