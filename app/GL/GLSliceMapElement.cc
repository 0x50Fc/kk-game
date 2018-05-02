//
//  GLSliceMapElement.cpp
//  KKGame
//
//  Created by zhanghailong on 2018/2/24.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//


#include "kk-config.h"
#include "GLSliceMapElement.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

namespace kk {
    
    namespace GL {
        
        IMP_SCRIPT_CLASS_BEGIN(&Element::ScriptClass, SliceMapElement, GLSliceMapElement)
        
        IMP_SCRIPT_CLASS_END
        
        SliceMapElement::SliceMapElement():size(0),slice(0) {
            
        }
        
        kk::CString SliceMapElement::path() {
            return _path.c_str();
        }
        
        void SliceMapElement::setPath(kk::CString path) {
            _path = path;
            _images.clear();
        }
        
        void SliceMapElement::changedKey(String& key) {
            Element::changedKey(key);
            
            if(key == "path") {
                setPath(get(key).c_str());
            } else if(key == "width") {
                size.x = kk::GA::floatValue(get(key));
            } else if(key == "height") {
                size.y = kk::GA::floatValue(get(key));
            } else if(key == "slice-width") {
                slice.x = kk::GA::floatValue(get(key));
            } else if(key == "slice-height") {
                slice.y = kk::GA::floatValue(get(key));
            }
        }
        
        
        void SliceMapElement::onDraw(Context * context) {
            Element::onDraw(context);
            
            if(slice.x <= 0 || slice.y <=0 || size.x <= 0 || size.y <=0 || _path.empty()) {
                return;
            }
            
            vec4 p[] = {{0,0,0,1},{0,0,0,1}};
            kk::Int ix = 0,iy = 0, iw = (kk::Int) (size.x / slice.x);
            char path[PATH_MAX];
            Float minY = - 0.5f * size.y;
            Float maxY = size.y * 0.5;
            Float minX = - 0.5f * size.x;
            Float maxX = size.x * 0.5;
            
            for(Float y = minY; y < maxY; y += slice.y) {
                
                ix = 0;
                
                for(Float x = minX; x < maxX; x += slice.x) {
                
                    p[0].x = x ;
                    p[0].y = y;
                    p[1].x = x + slice.x;
                    p[1].y = y + slice.y;
                    
                    if(context->isVisible(p, 2,vec4(slice.x,slice.y,slice.x,slice.y))) {
                        
                        snprintf(path, sizeof(path), _path.c_str(),iy * iw + ix + 1);
                        
                        Image * image = nullptr;
                        
                        std::map<String,Strong>::iterator i = _images.find(path);
                        
                        if(i != _images.end()) {
                            image = i->second.as<Image>();
                        } else {
                            Strong v = context->image(path);
                            image = v.as<Image>();
                            _images[path] = image;
                        }
                        
                        if(image != nullptr && image->status() == ImageStatusLoaded) {
                            
                            vec4 dest(x,y,slice.x,slice.y);
                            vec4 src(0,0,image->width(),image->height());
                            
                            context->drawTexture(image, dest, src,0);
                        }
                        
                    }
                    
                    ix ++;
                }
                
                iy ++;
            }
            
        }
        
        Float SliceMapElement::loadingProgress() {
            kk::Int n = 0, c = 0;
            std::map<String,Strong>::iterator i = _images.begin();
            while(i != _images.end()) {
                
                Image * v = i->second.as<Image>();
                
                if(v == nullptr || (v->status() == ImageStatusLoaded || v->status() == ImageStatusFail)) {
                    c ++;
                }
                
                n ++;
                i ++;
            }
            if(n == 0) {
                return 1.0f;
            } else {
                return (Float) c / (Float) n;
            }
        }
        
        
    }
}
