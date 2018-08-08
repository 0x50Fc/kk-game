//
//  GLTileMapElement.c
//  KKGame
//
//  Created by zhanghailong on 2018/4/25.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include "kk-config.h"
#include "GLTileMapElement.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

namespace kk {
    
    namespace GL
    {
        
        class TileMapTileLayerDrawItem : public kk::Object {
        public:
            kk::Strong image;
            kk::Strong data;
            std::vector<TextureVertex> vertexts;
        };
        
        class TileMapTileLayerDraw : public kk::Object {
        public:
            TileMapTileLayerDraw(Context * context,TileMapElement * element,kk::GA::TileMap* tileMap, kk::GA::TileMapTileLayer* layer);
            virtual void draw(Context * context);
        protected:
            std::vector<kk::Strong> items;
        };
        
        TileMapTileLayerDraw::TileMapTileLayerDraw(Context * context,TileMapElement * element,kk::GA::TileMap* tileMap,kk::GA::TileMapTileLayer* layer) {
            
            std::map<kk::GA::TileSet*,TileMapTileLayerDrawItem *> itemMap;
            
            for(kk::Int y = 0;y < tileMap->height;y++) {
                
                for(kk::Int x = 0;x < tileMap->width;x++) {
                    
                    int gid = layer->get(x, y);
                    
                    kk::GA::Tile * tile = tileMap->tile(gid);
                    
                    if(tile != nullptr ) {
                        
                        kk::GA::TileSet * tileSet = tile->tileSet.as<kk::GA::TileSet>();
                        
                        if(tileSet) {
                            
                            TileMapTileLayerDrawItem * item = nullptr;
                            
                            std::map<kk::GA::TileSet*,TileMapTileLayerDrawItem *>::iterator i = itemMap.find(tileSet);
                            
                            if(i != itemMap.end()) {
                                item = i->second;
                            } else {
                                item = new TileMapTileLayerDrawItem();
                                kk::Strong vv = element->image(context, tileSet->image.c_str());
                                item->image = vv.get();
                                items.push_back(item);
                                itemMap[tileSet] = item;
                            }
                            
                            kk::GA::Point p = tileMap->global({x,y});
                            
                            Float l = p.x;
                            Float t = p.y;
                            
                            Float sl = (Float) tile->x * tileSet->sw;
                            Float st = (Float) tile->y * tileSet->sh;
                            Float sr = sl + tileSet->sw;
                            Float sb = st + tileSet->sh;
                            
                            Float r = l + tileMap->tileWidth;
                            Float b = t + tileMap->tileHeight;
                            
                            l -= tileSet->tileX;
                            t -= tileSet->tileY;
                            
                    
                            item->vertexts.push_back({
                                {l,t,0},{sl,st}
                            });
                            
                            item->vertexts.push_back({
                                {r,t,0},{sr,st}
                            });
                            
                            item->vertexts.push_back({
                                {l,b,0},{sl,sb}
                            });
                            
                            item->vertexts.push_back({
                                {r,b,0},{sr,sb}
                            });
                            
                            item->vertexts.push_back({
                                {r,t,0},{sr,st}
                            });
                            
                            item->vertexts.push_back({
                                {l,b,0},{sl,sb}
                            });
                        }
                        
                    }
                    
                }
                
            }
            
            {
                std::vector<kk::Strong>::iterator i = items.begin();
                while(i != items.end()) {
                    TileMapTileLayerDrawItem * item = (*i).as<TileMapTileLayerDrawItem>();
                    Buffer * data = new Buffer(item->vertexts.size() * sizeof(TextureVertex),item->vertexts.data(),GL_STATIC_DRAW);
                    item->data = data;
                    item->vertexts.clear();
                    i ++;
                }
            }
            
        }

        
        void TileMapTileLayerDraw::draw(Context * context) {
            
            std::vector<kk::Strong>::iterator i = items.begin();
            
            while(i != items.end()) {
                
                TileMapTileLayerDrawItem * v = (*i).as<TileMapTileLayerDrawItem>();
                
                if(v) {
                    Image * image = v->image.as<Image>();
                    if(image && image->status() == ImageStatusLoaded) {
                        Buffer * data = v->data.as<Buffer>();
                        if(data) {
                            context->drawTexture(image, GL_TRIANGLES, data);
                        }
                        
                    }
                }
                
                i ++;
            }
            
        }
        
        IMP_SCRIPT_CLASS_BEGIN_NOALLOC(&Element::ScriptClass, TileMapElement, GLTileMapElement)
        
        IMP_SCRIPT_CLASS_END
        
        KK_IMP_ELEMENT_CREATE(TileMapElement)

        TileMapElement::TileMapElement(kk::Document * document,kk::CString name, kk::ElementKey elementId):Element(document,name,elementId) {
            
        }
        
        kk::GA::TileMap * TileMapElement::tileMap() {
            return dynamic_cast<kk::GA::TileMap *>(parent());
        }
        
        void TileMapElement::onDraw(Context * context) {
            Element::onDraw(context);
            
            kk::GA::TileMap * map = tileMap();
            
            if(map && map->isLoaded()) {
                
#ifdef KK_PLATFORM_OSX
                glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA);
#else
                glBlendFunc(GL_ONE , GL_ONE_MINUS_SRC_ALPHA);
#endif
                
                std::vector<kk::Strong>::iterator i = map->layers.begin();
                
                while(i != map->layers.end()) {
                    
                    {
                        kk::GA::TileMapTileLayer * v = (*i).as<kk::GA::TileMapTileLayer>();
                        
                        if(v) {
                            
                            TileMapTileLayerDraw * draw = v->draw.as<TileMapTileLayerDraw>();
                            
                            if(draw == nullptr) {
                                draw = new TileMapTileLayerDraw(context,this,map,v);
                                v->draw = draw;
                            }
                            
                            if(draw) {
                                draw->draw(context);
                            }
                        }
                    }
                    
                    i ++;
                }
            }
            
        }
        
        
        Image * TileMapElement::image(Context * context,kk::CString path) {
            
            if(path == nullptr) {
                return nullptr;
            }
            
            std::map<kk::String,kk::Strong>::iterator i = _images.find(path);
            
            if(i != _images.end()) {
                return i->second.as<Image>();
            }
            
            kk::Strong v = context->image(path);
            
            Image * image = v.as<Image>();
            
            if(image){
                _images[path] = image;
            }
            
            return image;
        }
    }
    
}

