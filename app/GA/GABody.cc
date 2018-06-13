//
//  GABody.cpp
//  KKGame
//
//  Created by zhanghailong on 2018/2/1.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//


#include "kk-config.h"
#include "GABody.h"
#include "GAScene.h"
#include "GAAction.h"
#include "GAShape.h"
#include <typeinfo>
#include <chipmunk/chipmunk.h>


namespace kk {
    
    namespace GA {
        
        IMP_SCRIPT_CLASS_BEGIN(&kk::GA::Element::ScriptClass, Body, GABody)
        
        static kk::script::Property propertys[] = {
            {"position",(kk::script::Function) &Body::duk_position,(kk::script::Function)  &Body::duk_setPosition},
        };
        
        kk::script::SetProperty(ctx, -1, propertys, sizeof(propertys) / sizeof(kk::script::Property));
        
        IMP_SCRIPT_CLASS_END
        
        Body::Body():bodyType(BodyTypeFixed),_zIndex(zIndexAutoY),_cpBody(nullptr) {
        }
        
        Body::~Body() {
            if(_cpBody) {
                cpBodyFree(_cpBody);
            }
        }
        
        Point Body::position() {
            return _position;
        }
        
        Float Body::zIndex() {
            if(_zIndex == zIndexAutoY) {
                return _position.y;
            }
            return _zIndex;
        }
        
        void Body::setPosition(Point value){
            _position = value;
        }
        
        Scene * Body::scene() {
            
            kk::Element * p = parent();
            
            while(p) {
                
                Scene * v = dynamic_cast<Scene *>(p);
                
                if(v) {
                    return v;
                }
                
                p = p->parent();
            }
            
            return nullptr;
        }
        
        void Body::changedKey(String& key) {
            Element::changedKey(key);
            if(key == "x") {
                _position.x = floatValue(get(key));
                if(_cpBody != nullptr) {
                    cpVect p = {_position.x,_position.y};
                    cpBodySetPosition(_cpBody, p);
                }
            } else if(key == "y") {
                _position.y = floatValue(get(key));
                if(_cpBody != nullptr) {
                    cpVect p = {_position.x,_position.y};
                    cpBodySetPosition(_cpBody, p);
                }
            } else if(key == "z") {
                String& v = get(key);
                if(v == "auto-y") {
                    _zIndex = zIndexAutoY;
                } else {
                    _zIndex = floatValue(v);
                }
            } else if(key == "focus") {
                Scene * v = scene();
                if(v && booleanValue(get(key))) {
                    v->setFocus(this);
                }
            } else if(key == "type") {
                String& v = get(key);
                if(v == "movable") {
                    bodyType = BodyTypeMovable;
                } else {
                    bodyType = BodyTypeFixed;
                }
            } else if(key == "mass") {
                if(_cpBody != nullptr) {
                    cpBodySetMass(_cpBody, floatValue(get(key))); //质量
                }
            }
    
        }
        
        void Body::exec(Context * context) {
            
            if(_cpBody == nullptr) {
                
                switch (bodyType) {
                    case BodyTypeFixed:
                    {
                        _cpBody = cpBodyNewStatic();
                    }
                        break;
                    case BodyTypeMovable:
                    {
                        cpFloat mass = floatValue(get("mass"));
                        
                        if(mass <= 0) {
                            mass = 1;
                        }
                        
                        cpFloat r = floatValue(get("radius"));
                        
                        if(r <=0 ){
                            r = 5;
                        }
                        
                        _cpBody = cpBodyNew(mass, cpMomentForCircle(mass, 0, r, {0,0}));

                        {
                            Scene * v = scene();
                            if(v) {
                                cpSpaceAddBody(v->cpSpace(), _cpBody);
                            }
                        }
                    }
                }
                
                if(_cpBody != nullptr ) {
                    cpVect p = {_position.x,_position.y};
                    cpBodySetPosition(_cpBody, p);
                    cpBodySetUserData(_cpBody, this);
                }
                
            }
            
            cpVect p = cpBodyGetPosition(_cpBody);
            _position.x = p.x;
            _position.y = p.y;
            
            Element::exec(context);
        }
    
        cpBody * Body::cpBody() {
            return _cpBody;
        }
        
        duk_ret_t Body::duk_position(duk_context * ctx) {
            
            duk_push_object(ctx);
            
            duk_push_string(ctx, "x");
            duk_push_number(ctx, _position.x);
            duk_put_prop(ctx, -3);
            
            duk_push_string(ctx, "y");
            duk_push_number(ctx, _position.y);
            duk_put_prop(ctx, -3);
            
            return 1;
        }
        
        duk_ret_t Body::duk_setPosition(duk_context * ctx) {
            
            int top = duk_get_top(ctx);
            
            if(top > 0 && duk_is_object(ctx, -top)) {
                duk_get_prop_string(ctx, -top, "x");
                if(duk_is_number(ctx, -1)) {
                    _position.x = duk_to_number(ctx, -1);
                }
                duk_pop(ctx);
                duk_get_prop_string(ctx, -top, "y");
                if(duk_is_number(ctx, -1)) {
                    _position.y = duk_to_number(ctx, -1);
                }
                duk_pop(ctx);
                if(_cpBody != nullptr) {
                    cpVect p = {_position.x,_position.y};
                    cpBodySetPosition(_cpBody, p);
                    cpBodySetVelocity(_cpBody, {0,0});
                }
            }

            return 0;
        }
        
        void Body::onWillRemoveFromParent(kk::Element * element) {
            
            Scene * v = scene();
            
            if(v != nullptr && _cpBody != nullptr) {
                
                ::cpSpace * cpSpace = v->cpSpace();
                
                if(cpBodyGetSpace(_cpBody) == cpSpace) {
                    cpSpaceRemoveBody(cpSpace, _cpBody);
                }
                
                kk::Element * e = firstChild();
                
                while(e) {
                    
                    Shape * shape = dynamic_cast<Shape * >(e);
                    
                    if(shape && shape->cpShape()) {
                        ::cpShape * cpShape = shape->cpShape();
                    
                        if(cpShapeGetSpace(cpShape) == cpSpace) {
                            cpSpaceRemoveShape(cpSpace, cpShape);
                        }
                        
                    }
                    
                    e = e->nextSibling();
                }
            }
            
            Element::onWillRemoveFromParent(element);
        }
        
    }
    
}
