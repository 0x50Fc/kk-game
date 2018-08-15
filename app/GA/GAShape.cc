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

#if defined(KK_PLATFORM_IOS)

#include <KKObject/KKObject.h>

#else

#include "kk-string.h"

#endif

namespace kk {
    
    namespace GA {

        IMP_SCRIPT_CLASS_BEGIN_NOALLOC(&kk::GA::Element::ScriptClass, Shape, GAShape)
        
        static kk::script::Property propertys[] = {
            {"position",(kk::script::Function) &Shape::duk_position,(kk::script::Function)  &Shape::duk_setPosition},
        };
        
        kk::script::SetProperty(ctx, -1, propertys, sizeof(propertys) / sizeof(kk::script::Property));
        
        
        IMP_SCRIPT_CLASS_END
        
        Body * Shape::body(){
            return dynamic_cast<Body *> (parent());
        }
        
        KK_IMP_ELEMENT_CREATE(Shape)
        
        Shape::Shape(kk::Document * document,kk::CString name, kk::ElementKey elementId):kk::GA::Element(document,name,elementId),_cpShape(nullptr),shapeType(ShapeTypeCircle) {
            
        }
        
        Shape::~Shape() {
            if(_cpShape != nullptr) {
                cpShapeFree(_cpShape);
            }
        }
        
        Point Shape::position() {
            return _position;
        }

        void Shape::setPosition(Point value){
            _position = value;
        }
        
        void Shape::changedKey(String& key) {
            Element::changedKey(key);
            
            if(key == "type") {
                kk::CString v = get(key.c_str());
                if(CStringEqual(v , "poly") ){
                    shapeType = ShapeTypePoly;
                } else if(CStringEqual(v , "segment")) {
                    shapeType = ShapeTypeSegment;
                } else if(CStringEqual(v , "box")) {
                    shapeType = ShapeTypeBox;
                } else {
                    shapeType = ShapeTypeCircle;
                }
            } else if(key == "x") {
                _position.x = floatValue(get(key.c_str()));
            } else if(key == "y") {
                _position.y = floatValue(get(key.c_str()));
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
                        kk::CStringSplit(get("points"), " ", vs);
                        
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
                        kk::CString v = get("density");
                        if(v != nullptr) {
                            Float vv = floatValue(v);
                            if(vv <= 0) {
                                vv = 1;
                            }
                            cpShapeSetDensity(_cpShape, vv);
                        }
                    }
                    
                    {
                        kk::CString v = get("mass");
                        if(v != nullptr) {
                            Float vv = floatValue(v);
                            if(vv <= 0) {
                                vv = 1;
                            }
                            cpShapeSetMass(_cpShape, vv);
                        }
                    }
                    
                    {
                        kk::CString v = get("elasticity");     //弹性
                        if(v != nullptr) {
                            Float vv = floatValue(v);
                            cpShapeSetElasticity(_cpShape, vv);
                        }
                    }
                    
                    {
                        kk::CString v = get("friction");       //摩擦
                        if(v != nullptr) {
                            Float vv = floatValue(v);
                            cpShapeSetFriction(_cpShape, vv);
                        } else {
                            cpShapeSetFriction(_cpShape, 1.0f);
                        }
                    }
                    
                    {
                        kk::CString v = get("group");       //组
                        if(v != nullptr) {
                            Int vv = intValue(v);
                            cpShapeSetFilter(_cpShape, cpShapeFilterNew(vv, CP_ALL_CATEGORIES, CP_ALL_CATEGORIES));
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
        
        duk_ret_t Shape::duk_position(duk_context * ctx) {
            
            duk_push_object(ctx);
            
            duk_push_string(ctx, "x");
            duk_push_number(ctx, _position.x);
            duk_put_prop(ctx, -3);
            
            duk_push_string(ctx, "y");
            duk_push_number(ctx, _position.y);
            duk_put_prop(ctx, -3);
            
            return 1;
        }
        
        duk_ret_t Shape::duk_setPosition(duk_context * ctx) {
            
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
            }
            
            return 0;
        }
        
    
    }
    
}
