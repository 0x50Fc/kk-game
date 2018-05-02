//
//  GAShape.cpp
//  KKGame
//
//  Created by zhanghailong on 2018/2/1.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include "kk-config.h"
#include "GAShape.h"
#include "GABody.h"
#include "GAScene.h"
#include <chipmunk/chipmunk.h>
#include "kk-string.h"

namespace kk {
    
    namespace GA {

        IMP_SCRIPT_CLASS_BEGIN(&kk::GA::Element::ScriptClass, Shape, GAShape)
        
        IMP_SCRIPT_CLASS_END
        
        Body * Shape::body(){
            return dynamic_cast<Body *> (parent());
        }
        
        Shape::Shape():_cpShape(nullptr),shapeType(ShapeTypeCircle) {
            
        }
        
        Shape::~Shape() {
            if(_cpShape != nullptr) {
                cpShapeFree(_cpShape);
            }
        }
        
        void Shape::changedKey(String& key) {
            Element::changedKey(key);
            
            if(key == "type") {
                kk::String & v = get(key);
                if(v == "poly") {
                    shapeType = ShapeTypePoly;
                } else if(v == "segment") {
                    shapeType = ShapeTypeSegment;
                } else if(v == "box") {
                    shapeType = ShapeTypeBox;
                } else {
                    shapeType = ShapeTypeCircle;
                }
            }
            
            if(_cpShape != nullptr) {
                if(key == "elasticity") {
                    cpShapeSetElasticity(_cpShape, floatValue(get("elasticity")));  //弹性
                } else if(key == "friction") {
                    cpShapeSetFriction(_cpShape, floatValue(get("friction")));      // 摩擦
                } else if(key == "mass") {
                    cpFloat v = floatValue(get("mass"));
                    if(v <= 0) {
                        v = 1;
                    }
                    cpShapeSetMass(_cpShape, v);    // 质量
                } else if(key == "density") {
                    cpFloat v = floatValue(get("density"));
                    if(v <= 0) {
                        v = 1;
                    }
                    cpShapeSetDensity(_cpShape, v);  // 密度
                }
            }
        }
        
        void Shape::exec(Context * context) {
            Element::exec(context);
            
            Body * body = this->body();
            
            if(body && body->cpBody() && _cpShape == nullptr) {
                
                ::cpBody * cpBody = body->cpBody();
                
                switch (shapeType) {
                    case ShapeTypeCircle:
                    {
                        cpVect p = {0,0};
                        p.x = floatValue(get("x"));
                        p.y = floatValue(get("y"));
                        _cpShape = cpCircleShapeNew(cpBody, floatValue(get("radius")), p);
                    }
                        break;
                    case ShapeTypeSegment:
                    {
                        cpVect p = {0,0};
                        p.x = floatValue(get("x"));
                        p.y = floatValue(get("y"));
                        cpVect to = {0,0};
                        to.x = floatValue(get("to-x"));
                        to.y = floatValue(get("to-y"));
                        _cpShape = cpSegmentShapeNew(cpBody, p, to, floatValue(get("radius")));
                    }
                        break;
                    case ShapeTypePoly:
                    {
                        float x,y;
                        std::vector<cpVect> ps;
                        std::vector<kk::String> vs;
                        kk::CStringSplit(get("points").c_str(), " ", vs);
                        
                        std::vector<kk::String>::iterator i = vs.begin();
                        
                        while(i != vs.end()) {
                            x = 0;
                            y = 0;
                            sscanf((*i).c_str(), "%f,%f",&x,&y);
                            ps.push_back({x,y});
                            i ++;
                        }
                        _cpShape = cpPolyShapeNewRaw(cpBody, (int) ps.size(), ps.data(), floatValue(get("radius")));
                    }
                        break;
                    case ShapeTypeBox:
                    {
                        Float x = floatValue(get("x"));
                        Float y = floatValue(get("y"));
                        Float w = floatValue(get("width"));
                        Float h = floatValue(get("height"));
                        Float l = x - w * 0.5f;
                        Float t = y - h * 0.5f;
                        Float r = l + w;
                        Float b = t + h;
                        cpVect p[] = {
                            {l,t},{r,t},{r,b},{l,t}
                        };
                        
                        _cpShape = cpPolyShapeNewRaw(cpBody, 4, p, floatValue(get("radius")));
                        
                    }
                        break;
                    default:
                        break;
                }
                
                if(_cpShape != nullptr) {
                    
                    Scene * scene = body->scene();
                    
                    cpShapeSetUserData(_cpShape, this);
                    
                    {
                        kk::String & v = get("density");
                        if(!v.empty()) {
                            Float vv = floatValue(v);
                            if(vv <= 0) {
                                vv = 1;
                            }
                            cpShapeSetDensity(_cpShape, vv);
                        }
                    }
                    
                    {
                        kk::String & v = get("mass");
                        if(!v.empty()) {
                            Float vv = floatValue(v);
                            if(vv <= 0) {
                                vv = 1;
                            }
                            cpShapeSetMass(_cpShape, vv);
                        }
                    }
                    
                    {
                        kk::String & v = get("elasticity");     //弹性
                        if(&v != &kk::Element::NotFound) {
                            Float vv = floatValue(v);
                            cpShapeSetElasticity(_cpShape, vv);
                        }
                    }
                    
                    {
                        kk::String & v = get("friction");       //摩擦
                        if(&v != &kk::Element::NotFound) {
                            Float vv = floatValue(v);
                            cpShapeSetFriction(_cpShape, vv);
                        } else {
                            cpShapeSetFriction(_cpShape, 1.0f);
                        }
                    }
                    
                    if(scene != nullptr) {
                        cpSpaceAddShape(scene->cpSpace(), _cpShape);
                    }
                }
                
            }
        }
        
        ::cpShape * Shape::cpShape() {
            return _cpShape;
        }
        
        void Shape::onWillRemoveFromParent(kk::Element * element){
            
            Body * v = body();
            
            if(v != nullptr && _cpShape != nullptr) {
                
                Scene * scene = v->scene();
                
                if(scene != nullptr) {
                    cpSpaceRemoveShape(scene->cpSpace(), _cpShape);
                }
                
            }
            
            Element::onWillRemoveFromParent(element);
        }
        
    
    }
    
}
