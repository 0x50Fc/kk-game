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
        
        IMP_SCRIPT_CLASS_END
        
        ActionWalk::ActionWalk()
            :x(0),y(0),speed(0),angle(0),_hasUpdate(false),_landing(true),_enabled(true) {
            
        }

        void ActionWalk::exec(Context * context) {
            Action::exec(context);
            
            if(_hasUpdate) {
                
                Body * body = this->body();
                
                if(body) {
                    
                    ::cpBody * cpBody = body->cpBody();
                    
                    if(cpBody) {
                        
                        if(_enabled) {
                            
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
            
            if(!_landing && speed > 0.0f && _enabled) {
                
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
            }
            
            _hasUpdate = true;
            _landing = false;
            _distance = MAXFLOAT;
        }
        
    }
    
}

