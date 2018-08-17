//
//  GAScene.cc
//  KKGame
//
//  Created by zhanghailong on 2018/2/1.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include "kk-config.h"
#include "GAScene.h"
#include "GAShape.h"
#include "GABody.h"
#include <chipmunk/chipmunk.h>
#include <algorithm>

namespace kk {
    
    namespace GA {
        
        IMP_SCRIPT_CLASS_BEGIN_NOALLOC(&kk::GA::Element::ScriptClass, Scene, GAScene)
        
        static kk::script::Property propertys[] = {
            {"focus",(kk::script::Function) &Scene::duk_focus,(kk::script::Function) &Scene::duk_setFocus},
        };
        
        kk::script::SetProperty(ctx, -1, propertys, sizeof(propertys) / sizeof(kk::script::Property));
        
        static kk::script::Method methods[] = {
            {"convert",(kk::script::Function) &Scene::duk_convert},
            {"nearby",(kk::script::Function) &Scene::duk_nearby},
        };
        
        kk::script::SetMethod(ctx, -1, methods, sizeof(methods) / sizeof(kk::script::Method));
        
        IMP_SCRIPT_CLASS_END
        
        
        static cpBool Scene_cpCollisionBegin(::cpArbiter *arb, ::cpSpace *space, ::cpDataPointer userData) {
            
            cpBool r = cpTrue;
            
            Scene * scene = (Scene *) cpSpaceGetUserData(space);
            
            ::cpShape * a = nullptr, * b = nullptr;
            
            cpArbiterGetShapes(arb, &a, &b);
            
            cpVect n = cpArbiterGetNormal(arb);
            
            Point np(n.x,n.y);
            
            if(a && b) {
                
                kk::Int a_collisionType = (kk::Int) cpShapeGetCollisionType(a);
                kk::Int b_collisionType = (kk::Int) cpShapeGetCollisionType(b);
                Shape * a_shape = (Shape *) cpShapeGetUserData(a);
                Shape * b_shape = (Shape *) cpShapeGetUserData(b);
                
                if(a_shape && b_shape) {
                    
                    {
                        kk::Element * e = scene->firstChild();
                        
                        while(e) {
                            
                            Body * b = dynamic_cast<Body *>(e);
                            
                            Collision * v = dynamic_cast<Collision *>(e);
                            
                            if(b == nullptr && v != nullptr) {
                                
                                if(v->collisionType() == a_collisionType && v->hasCollisionType(b_collisionType)) {
                                    r = v->inCollisionShape(a_shape, b_shape,np) && r;
                                }
                                
                                if(v->collisionType() == b_collisionType && v->hasCollisionType(a_collisionType)) {
                                    r = v->inCollisionShape(b_shape, a_shape,np) && r;
                                }
                                
                            } else {
                                break;
                            }
                            
                            e = e->nextSibling();
                        }
                    }
                    
                    Body * a_body = a_shape->body();
                    Body * b_body = b_shape->body();
                    
                    if(a_body) {
                        r = a_body->inCollisionShape(a_shape, b_shape,np) && r;
                    }
                    
                    if(b_body) {
                        r = b_body->inCollisionShape(b_shape, a_shape,np) && r;
                    }
                    
                }
            }

            return r;
        }
        
        static cpBool Scene_cpCollisionPreSolve(::cpArbiter *arb, ::cpSpace *space, ::cpDataPointer userData) {
            
            return cpTrue;
        }
        
        static void Scene_cpCollisionPostSolve(::cpArbiter *arb, ::cpSpace *space, ::cpDataPointer userData) {

        }
        
        static void Scene_cpCollisionSeparate(::cpArbiter *arb, ::cpSpace *space, ::cpDataPointer userData) {
            
            Scene * scene = (Scene *) cpSpaceGetUserData(space);
            
            ::cpShape * a = nullptr, * b = nullptr;
            
            cpArbiterGetShapes(arb, &a, &b);
            
            cpVect n = cpArbiterGetNormal(arb);
            
            Point np(n.x,n.y);
            
            if(a && b) {
                
                kk::Int a_collisionType = (kk::Int) cpShapeGetCollisionType(a);
                kk::Int b_collisionType = (kk::Int) cpShapeGetCollisionType(b);
                Shape * a_shape = (Shape *) cpShapeGetUserData(a);
                Shape * b_shape = (Shape *) cpShapeGetUserData(b);
                
                if(a_shape && b_shape) {
                    
                    {
                        kk::Element * e = scene->firstChild();
                        
                        while(e) {
                            
                            Body * b = dynamic_cast<Body *>(e);
                            
                            Collision * v = dynamic_cast<Collision *>(e);
                            
                            if(b == nullptr && v != nullptr) {
                                
                                if(v->collisionType() == a_collisionType && v->hasCollisionType(b_collisionType)) {
                                    v->outCollisionShape(a_shape, b_shape,np);
                                }
                                
                                if(v->collisionType() == b_collisionType && v->hasCollisionType(a_collisionType)) {
                                    v->outCollisionShape(b_shape, a_shape,np);
                                }
                                
                            } else {
                                break;
                            }
                            
                            e = e->nextSibling();
                        }
                    }
                    
                    Body * a_body = a_shape->body();
                    Body * b_body = b_shape->body();
                    
                    if(a_body) {
                        a_body->outCollisionShape(a_shape, b_shape,np);
                    }
                    
                    if(b_body) {
                        b_body->outCollisionShape(b_shape, a_shape,np);
                    }
                    
                }
            }
       
        }
        
        KK_IMP_ELEMENT_CREATE(Scene)
        
        Scene::Scene(kk::Document * document,kk::CString name, kk::ElementKey elementId):kk::GA::Element(document,name,elementId),_prevTimeInterval(0) {
                
            _cpSpace = cpSpaceNew();
            cpSpaceSetUserData(_cpSpace, this);
            cpSpaceSetGravity(_cpSpace, {0,0});
            
            {
                cpCollisionHandler * v = cpSpaceAddDefaultCollisionHandler(_cpSpace);
                v->beginFunc = Scene_cpCollisionBegin;
                v->preSolveFunc = Scene_cpCollisionPreSolve;
                v->postSolveFunc = Scene_cpCollisionPostSolve;
                v->separateFunc = Scene_cpCollisionSeparate;
                
                v->userData = this;
            }
        }
        
        Scene::~Scene() {
            cpSpaceFree(_cpSpace);
        }
        
        void Scene::exec(Context * context) {
            _viewport = context->viewport();
            if(_prevTimeInterval == 0) {
                cpSpaceStep(_cpSpace,0);
            } else {
                cpFloat dt = (cpFloat) (context->current() - _prevTimeInterval) * 0.001f;
                cpSpaceStep(_cpSpace,dt);
            }
            _prevTimeInterval = context->current();
            
            Element::exec(context);
        }
        
        void Scene::changedKey(String &key){
            Element::changedKey(key);
            if(key == "width") {
                size.width = floatValue(get(key.c_str()));
            } else if(key == "height") {
                size.height = floatValue(get(key.c_str()));
            } else if(key == "x") {
                _position.x = floatValue(get(key.c_str()));
            } else if(key == "y") {
                _position.y = floatValue(get(key.c_str()));
            } else if(key == "gravity-x") {
                cpVect p = cpSpaceGetGravity(_cpSpace);
                p.x = floatValue(get(key.c_str()));
                cpSpaceSetGravity(_cpSpace, p);
            } else if(key == "gravity-y") {
                cpVect p = cpSpaceGetGravity(_cpSpace);
                p.y = floatValue(get(key.c_str()));
                cpSpaceSetGravity(_cpSpace, p);
            }
        }
        
        ::cpSpace * Scene::cpSpace() {
            return _cpSpace;
        }
        
        Body * Scene::focus() {
            return _focus.as<Body>();
        }
        
        void Scene::setFocus(Body * body) {
            Body * v = _focus.as<Body>();
            if(v != body) {
                _focus = body;
            }
        }
        
        Point Scene::convert(Point point) {
            Point p = position();
            p.x = - p.x +  point.x * _viewport.width * 0.5f;
            p.y = - p.y + point.y * _viewport.height * 0.5f;
            return p;
        }
        
        Point Scene::position() {
            Point p = _position;
            Body * v = _focus.as<Body>();
            if(v) {
                p = v->position();
            }
            
            if(_viewport.width !=0.0f && _viewport.height !=0.0f) {
                Float r = 0.5f * (size.width - _viewport.width);
                Float b = 0.5f * (size.height - _viewport.height);
                
                if(p.x > r) {
                    p.x = r;
                }
                
                if(p.x < -r) {
                    p.x = -r;
                }
                
                if(p.y > b) {
                    p.y = b;
                }
                
                if(p.y < -b) {
                    p.y = -b;
                }
            }
            
            p.x = - p.x;
            p.y = - p.y;
            return p;
        }
        
        void Scene::setPosition(Point p) {
            _position = p;
        }
        
        Float Scene::zIndex() {
            return 0.0f;
        }
        
        Scene * Scene::scene() {
            return this;
        }
        
        Size & Scene::viewport() {
            return _viewport;
        }
        
        duk_ret_t Scene::duk_focus(duk_context * ctx) {
            kk::script::PushObject(ctx, _focus.get());
            return 1;
        }
        
        duk_ret_t Scene::duk_setFocus(duk_context * ctx) {
            
            int top = duk_get_top(ctx);
            
            if(top > 0 && duk_is_object(ctx, -top)) {
                Body * v = dynamic_cast<Body *>(kk::script::GetObject(ctx, -top));
                setFocus(v);
            }
            
            return 0;
        }
        
        duk_ret_t Scene::duk_convert(duk_context * ctx) {
            
            int top = duk_get_top(ctx);
            
            if(top > 1 && duk_is_number(ctx, -top) && duk_is_number(ctx, -top + 1)) {
                
                Point p(duk_to_number(ctx, -top),duk_to_number(ctx, -top + 1));
                p = convert(p);
                
                duk_push_object(ctx);
                
                duk_push_string(ctx, "x");
                duk_push_number(ctx, p.x);
                duk_put_prop(ctx, -3);
                
                duk_push_string(ctx, "y");
                duk_push_number(ctx, p.y);
                duk_put_prop(ctx, -3);
                
                return 1;
            }
            
            return 0;
        }
        
        struct NearbyBody {
            Body * body;
            Float distance;
        };
        
        bool NearbyBodyLesser(const NearbyBody &a, const NearbyBody &b)
        {
            return a.distance < b.distance;
        }
        
        duk_ret_t Scene::duk_nearby(duk_context * ctx) {
        
            int top = duk_get_top(ctx);
            
            Float x = 0;
            Float y = 0;
            Float r = 0;
            
            if(top > 0 && duk_is_number(ctx, -top )) {
                x = duk_to_number(ctx, -top);
            }
            
            if(top > 1 && duk_is_number(ctx, -top + 1 )) {
                y = duk_to_number(ctx, -top + 1);
            }
            
            if(top > 2 && duk_is_number(ctx, -top + 2)) {
                r = duk_to_number(ctx, -top + 2);
            }
            
//            kk::Log("[SCENE] [NEARBY] %g,%g %g",x,y,r);
            
            duk_push_array(ctx);
            
            if(r > 0) {
                
                std::vector<NearbyBody> nearbyBodys;
                
                NearbyBody nearbyBody = {nullptr,MAXFLOAT};
                
                kk::Element * e = firstChild();
                
                while(e) {
                    
                    {
                        nearbyBody.body = dynamic_cast<Body *>(e);
                        
                        if(nearbyBody.body != nullptr && nearbyBody.body->bodyType == BodyTypeMovable) {
                            
                            kk::GA::Point p = nearbyBody.body->position();
                            
                            Float dx = x - p.x;
                            Float dy = y - p.y;
                            
                            nearbyBody.distance = sqrt(dx * dx + dy * dy) ;
                            
//                            kk::Log("[SCENE] [NEARBY] [BODY] %g,%g %g",p.x,p.y,nearbyBody.distance);
                            
                            if(nearbyBody.distance <= r) {
                                nearbyBodys.push_back(nearbyBody);
                            }
                            
                        }
                    }
                    
                    e = e->nextSibling();
                }
                
                std::sort(nearbyBodys.begin(),nearbyBodys.end(),NearbyBodyLesser);
                
                std::vector<NearbyBody>::iterator i = nearbyBodys.begin();
                
                int idx = 0;
                
                while(i != nearbyBodys.end()) {
                    
                    kk::script::PushObject(ctx, (*i).body);
                    duk_put_prop_index(ctx, -2, idx);
                    
                    i ++;
                    idx ++;
                }
                
            }
            
        
            return 1;
            
        }
        
    }
    
}
