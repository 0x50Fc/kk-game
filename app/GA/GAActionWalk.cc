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
        
        IMP_SCRIPT_CLASS_BEGIN(&Action::ScriptClass, ActionWalk, GAActionWalk)
        
        static kk::script::Property propertys[] = {
            {"target",(kk::script::Function) &ActionWalk::duk_target,(kk::script::Function) &ActionWalk::duk_setTarget},
        };
        
        kk::script::SetProperty(ctx, -1, propertys, sizeof(propertys) / sizeof(kk::script::Property));
        
        IMP_SCRIPT_CLASS_END
        
        ActionWalk::ActionWalk()
            :x(0),y(0),speed(0),angle(0),_hasUpdate(false)
            ,_landing(true),_enabled(true),duration(0),_startTimeInterval(0){
            
        }

        void ActionWalk::exec(Context * context) {
            Action::exec(context);
            
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
                    
                    if(v > _distance) {
                        
                        kk::Strong vv = new kk::ElementEvent();
                        kk::ElementEvent * e = vv.as<kk::ElementEvent>();
                        
                        e->element = body;
                        
                        body->emit("done", e);
                        
                        _landing = true;
                    } else {
                        _distance = v;
                    }
                }
            }
        
        }
        
        void ActionWalk::changedKey(String& key) {
            Action::changedKey(key);
            
            if(key == "speed") {
                speed = floatValue(get(key));
            } else if(key == "x") {
                this->x = floatValue(get(key));
            } else if(key == "y") {
                this->y = floatValue(get(key));
            } else if(key == "enabled") {
                _enabled = booleanValue(get(key));
            } else if(key == "duration") {
                duration = floatValue(get(key));
                _startTimeInterval = 0;
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
        
    }
    
}

