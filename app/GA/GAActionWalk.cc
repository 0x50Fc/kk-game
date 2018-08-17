//
//  GAActionWalk.cpp
//  KKGame
//
//  Created by zhanghailong on 2018/2/9.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include "kk-config.h"
#include "GAActionWalk.h"
#include "GABody.h"
#include "GAShape.h"
#include <chipmunk/chipmunk.h>

namespace kk {
    
    namespace GA {
        
        IMP_SCRIPT_CLASS_BEGIN_NOALLOC(&Action::ScriptClass, ActionWalk, GAActionWalk)
        
        static kk::script::Property propertys[] = {
            {"target",(kk::script::Function) &ActionWalk::duk_target,(kk::script::Function) &ActionWalk::duk_setTarget},
        };
        
        kk::script::SetProperty(ctx, -1, propertys, sizeof(propertys) / sizeof(kk::script::Property));
        
        IMP_SCRIPT_CLASS_END
        
        KK_IMP_ELEMENT_CREATE(ActionWalk)
        
        ActionWalk::ActionWalk(kk::Document * document,kk::CString name, kk::ElementKey elementId)
            :Action(document,name,elementId)
            ,x(0),y(0),speed(0),angle(0),_hasUpdate(false)
            ,_landing(true),_enabled(true),duration(0),_startTimeInterval(0)
            ,_navigateState(ActionWalkNavigateStateNone){
            
        }

        void ActionWalk::exec(Context * context) {
            Action::exec(context);
            
            if(_navigateState == ActionWalkNavigateStateNavigating) {
                
                if(_navigateTimeIterval == 0) {
                    _navigateTimeIterval = context->current();
                }
                
                if(context->current() - _navigateTimeIterval > 120) {
                    _navigateState = ActionWalkNavigateStateNone;
                    
                    Body * a = body();
                    
                    if(a != nullptr && !_navigateVelocitys.empty()) {
                        
                        ::cpBody * cpBody = a->cpBody();
                        
                        if(cpBody) {
                            cpBodySetVelocity(cpBody, {0,0});
                        }
                        
                    }
                } else {
                    return;
                }
            }
            
            Body * tBody = target.as<Body>();
            
            if(tBody) {
                
                Point p = tBody->position();
                
                if(p.x != this->x || p.y != this->y) {
                    this->x = p.x;
                    this->y = p.y;
                    _hasUpdate = true;
                }
                
                if(_startTimeInterval == 0) {
                    _startTimeInterval = context->current();
                }
                
                if(!_landing && duration >0 && context->current() - _startTimeInterval >= duration) {
                    
                    Body * body = this->body();
                    
                    if(body) {
                        
                        kk::Strong vv = new kk::ElementEvent();
                        kk::ElementEvent * e = vv.as<kk::ElementEvent>();
                        
                        e->element = body;
                        
                        body->emit("done", e);
                        
                    }
                    
                    _landing = true;
                    
                }
            }
            
            if(_hasUpdate) {
                
                Body * body = this->body();
                
                if(body) {
                    
                    ::cpBody * cpBody = body->cpBody();
                    
                    if(cpBody) {
                        
                        if(_enabled && speed > 0) {
                            
                            Point p = body->position();
                            
                            cpVect v = cpvmult(cpvnormalize({this->x - p.x,this->y - p.y}), speed);
                            
                            cpBodySetVelocity(cpBody, v);
                            
                        } else {
                            cpBodySetVelocity(cpBody, {0,0});
                        }
                        
                        _hasUpdate = false;
                    }
                }
            }
            
            if(!_landing && speed > 0.0f && _enabled && tBody == nullptr) {
                
                Body * body = this->body();
                
                if(body) {
                    
                    Point p = body->position();
                    
                    Float v = cpvlength({this->x - p.x,this->y - p.y});
                    
                    if(v > _distance || v < 5) {
                        
                        _landing = true;
                        
                        ::cpBody * cpBody = body->cpBody();
                        
                        if(cpBody) {
                            cpBodySetVelocity(cpBody, {0,0});
                            cpBodySetPosition(cpBody, {this->x,this->y});
                        }
                        
                        p.x = this->x;
                        p.y = this->y;
                        body->setPosition(p);
                        
                        kk::Strong vv = new kk::ElementEvent();
                        kk::ElementEvent * e = vv.as<kk::ElementEvent>();
                        
                        e->element = body;
                        
                        body->emit("done", e);
                        
                    } else {
                        _distance = v;
                    }
                }
            }
            
            Body * body = this->body();
            
            if(body) {
                _lastPosition = body->position();
            }
        
        }
        
        void ActionWalk::changedKey(String& key) {
            Action::changedKey(key);
            
            if(key == "speed") {
                speed = floatValue(get(key.c_str()));
            } else if(key == "x") {
                this->x = floatValue(get(key.c_str()));
            } else if(key == "y") {
                this->y = floatValue(get(key.c_str()));
            } else if(key == "enabled") {
                _enabled = booleanValue(get(key.c_str()));
            } else if(key == "duration") {
                duration = floatValue(get(key.c_str()));
                _startTimeInterval = 0;
            } else if(key == "target") {
                kk::ElementKey elementId = int64Value(get(key.c_str()));
                kk::Document * doc = document();
                if(doc) {
                    kk::Strong e = doc->element(elementId);
                    target = e.as<Body>();
                } else {
                    target = (kk::Object *) nullptr;
                }
            }
            
            _hasUpdate = true;
            _landing = false;
            _distance = MAXFLOAT;
        }
        
        duk_ret_t ActionWalk::duk_target(duk_context * ctx) {
            
            kk::script::PushObject(ctx, target.get());
            
            return 1;
        }
        
        duk_ret_t ActionWalk::duk_setTarget(duk_context * ctx) {
            
            int top = duk_get_top(ctx);
            
            if(top >0 && duk_is_object(ctx, -top)) {
                kk::Object * v = kk::script::GetObject(ctx, -top);
                if(v != nullptr) {
                    Body * body = dynamic_cast<kk::GA::Body *>(v);
                    if(body != nullptr) {
                        target = body;
                    } else {
                        target = (kk::Object *) nullptr;
                    }
                } else {
                    target = (kk::Object *) nullptr;
                }
            }
            
            return 0;
        }
        
        kk::Boolean ActionWalk::inCollisionShape(Shape * a, Shape * b,Point n) {
            
            kk::Boolean v = Action::inCollisionShape(a, b,n);
            
            if(a->body() == body()) {
                v = inCollisionShape(b,n) && v;
            }
            
            return v;
        }
        
        void ActionWalk::outCollisionShape(Shape * a, Shape * b,Point n) {
            Action::outCollisionShape(a, b, n);
            if(a->body() == body()) {
                outCollisionShape(b,n);
            }
        }
        
        kk::Boolean ActionWalk::inCollisionShape(Shape * shape,Point n) {
            
            Body * a = body();
            Body * b = shape->body();
            
            if(a && b) {
                
                cpVect p = cpvmult(cpv(-n.x,-n.y), 0.5f * speed);
                
                a->setPosition(_lastPosition);
                
                ::cpBody * cpBody = a->cpBody();
                
                if(cpBody) {
                    cpBodySetPosition(cpBody, {_lastPosition.x,_lastPosition.y});
                    cpBodySetVelocity(cpBody, {0,0});
                    cpBodyApplyImpulseAtLocalPoint(cpBody, p, {0,0});
                    
                    p = cpvmult(cpvnormalize({this->x - _lastPosition.x,this->y - _lastPosition.y}), 0.5f * speed);
                    
                    cpBodyApplyImpulseAtLocalPoint(cpBody, p, {0,0});
                    
                }
            }
            
            _navigateState = ActionWalkNavigateStateNavigating;
            _navigateTimeIterval = 0;
            
            return true;
        }
        
        void ActionWalk::outCollisionShape(Shape * shape,Point n) {

        }
        
    }
    
}

