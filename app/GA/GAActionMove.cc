//
//  GAActionMove.cpp
//  KKGame
//
//  Created by hailong11 on 2018/2/9.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include "kk-config.h"
#include "GAActionMove.h"
#include "GABody.h"
#include "GAShape.h"
#include <chipmunk/chipmunk.h>

namespace kk {
    
    namespace GA {
        
        IMP_SCRIPT_CLASS_BEGIN(&Action::ScriptClass, ActionMove, GAActionMove)
        
        IMP_SCRIPT_CLASS_END
        
        ActionMove::ActionMove()
            :move(),speed(0),angle(0),_hasUpdate(false),_landing(true) {
            
        }

        void ActionMove::exec(Context * context) {
            Action::exec(context);
            
            if(_hasUpdate) {
                
                Body * body = this->body();
                
                if(body) {
                    
                    ::cpBody * cpBody = body->cpBody();
                    
                    if(cpBody) {
                        
                        Point p = body->position();
                        
                        cpVect v = cpvclamp({move.x - p.x,move.y - p.y}, speed);
                    
                        cpBodySetVelocity(cpBody, v);
                        
                        _hasUpdate = false;
                    }
                }
            }
            
            if(!_landing) {
                
                Body * body = this->body();
                
                if(body) {
                    
                    Point p = body->position();
                    
                    Float v = cpvlength({move.x - p.x,move.y - p.y});
                    
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
        
        void ActionMove::changedKey(String& key) {
            Action::changedKey(key);
            
            if(key == "speed") {
                speed = floatValue(get(key));
            } else if(key == "move-x") {
                move.x = floatValue(get(key));
            } else if(key == "move-y") {
                move.y = floatValue(get(key));
            }
            
            _hasUpdate = true;
            _landing = false;
            _distance = INFINITY;
        }
        
    }
    
}

