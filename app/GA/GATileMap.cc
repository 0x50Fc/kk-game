//
//  GATileMap.c
//  KKGame
//
//  Created by zhanghailong on 2018/4/27.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include "kk-config.h"
#include "GATileMap.h"
#include "kk-string.h"
#include <zlib.h>
#include <chipmunk/chipmunk.h>

namespace kk {
    
    namespace GA {
        
        IMP_SCRIPT_CLASS_BEGIN(&kk::GA::Element::ScriptClass, TileMap, GATileMap)
        
        IMP_SCRIPT_CLASS_END

        TileMap::TileMap()
            :height(0),width(0),
                tileHeight(0),tileWidth(0),orientation(TileMapOrientationOrthogonal),_loaded(false) {
            
        }
        
        kk::Boolean TileMap::isLoaded() {
            return _loaded;
        }
        
        TileLocal TileMap::local(Point global) {
            TileLocal p = {0,0};
            
            kk::Int CHIP_W = tileWidth >> 1;
            kk::Int CHIP_H = tileHeight >> 1;
            
            switch (orientation) {
                case TileMapOrientationIsometric:
                    global.y += height * CHIP_H;
                    p.x = (kk::Int) (global.y / CHIP_H + global.x / CHIP_W) >> 1;
                    p.y = (kk::Int) (global.y / CHIP_H - global.x / CHIP_W) >> 1;
                    break;
                case TileMapOrientationStaggered:
                case TileMapOrientationHexagonal:
                    global.x += width * CHIP_W;
                    global.y += (height * CHIP_H) >> 1;
                    p.y = (kk::Int) (global.y + CHIP_H) / CHIP_H;
                    if(p.y % 2 == 0) {
                        p.x = (kk::Int) (global.x + CHIP_W) / tileWidth;
                    } else {
                        p.x = (kk::Int) (global.x) / tileWidth;
                    }
                    break;
                default:
                    p.x = (global.x + 0.5f * width * tileWidth) / tileWidth;
                    p.x = (global.y + 0.5f * height * tileHeight) / tileHeight;
                    break;
            }
            
            return p;
        }
        
        Point TileMap::global(TileLocal local) {
            
            Point p;
            
            kk::Int CHIP_W = tileWidth >> 1;
            kk::Int CHIP_H = tileHeight >> 1;
            
            switch (orientation) {
                case TileMapOrientationIsometric:
                    p.x = (local.x - local.y) * CHIP_W;
                    p.y = (local.x + local.y) * CHIP_H - height * CHIP_H;
                    break;
                case TileMapOrientationStaggered:
                case TileMapOrientationHexagonal:
                    if(local.y % 2 == 0) {
                        p.x = local.x * tileWidth - CHIP_W;
                        p.y = local.y * CHIP_H - CHIP_H;
                    } else {
                        p.x = local.x * tileWidth;
                        p.y = local.y * CHIP_H - CHIP_H;
                    }
                    p.x -= width * CHIP_W;
                    p.y -= (height * CHIP_H) >> 1;
                    break;
                default:
                    p.x = local.x * tileWidth - width * CHIP_W;
                    p.y = local.y * tileHeight - height * CHIP_H;
                    break;
            }
            
            return p;
        }
        
        Tile * TileMap::tile(kk::Int gid) {
            TileSet * prev = nullptr;
            std::vector<kk::Strong>::iterator i = tileSets.begin();
            while(i != tileSets.end()) {
                TileSet * v = (*i).as<TileSet>();
                if(v) {
                    if(v->firstgid > gid) {
                        break;
                    } else {
                        prev = v;
                    }
                }
                i ++;
            }
            if(prev != nullptr) {
                kk::Int i = gid - prev->firstgid;
                if(i >=0 && i < prev->tiles.size()) {
                    return prev->tiles[i].as<Tile>();
                }
            }
            return nullptr;
        }
        
        void TileSet::setContent(duk_context * ctx,kk::CString path)  {
            
            int top = duk_get_top(ctx);
            
            TileMap * map = dynamic_cast<TileMap *>( kk::script::GetObject(ctx, - top) );
            Context * context = dynamic_cast<Context *>( kk::script::GetObject(ctx, - top + 1) );
            
            kk::String basePath = map->get("path");
            
            basePath = kk::CStringPathDeleteLast(basePath.c_str());
            
            kk::String v = context->getString(kk::CStringPathAppend(basePath.c_str(), path).c_str());
            
            if(kk::script::decodeJSON(ctx, v.c_str(), v.size())) {
                kk::script::Error(ctx, -1);
            }
            
            width = Y_toInt(ctx, -1, "imagewidth");
            height = Y_toInt(ctx, -1, "imageheight");
            tileWidth = Y_toInt(ctx, -1, "tilewidth");
            tileHeight = Y_toInt(ctx, -1, "tileheight");
            image = kk::CStringPathAppend(basePath.c_str(),Y_toString(ctx, -1, "image",""));
            
            {
                duk_get_prop_string(ctx, -1, "properties");
                YStringMapSet(propertys, ctx, -1);
                duk_pop(ctx);
            }
            
            {
                duk_get_prop_string(ctx, -1, "tileoffset");
                tileX = Y_toInt(ctx, -1,"x");
                tileY = Y_toInt(ctx, -1,"y");
                duk_pop(ctx);
            }
            
            {
                kk::Int tilecount = Y_toInt(ctx, -1,"tilecount");
                void * props = Y_toHeapptr(ctx, -1, "tileproperties");
                
                sw = (Float) tileWidth / (Float) width;
                sh = (Float) tileHeight / (Float) height;
                
                kk::Uint column = width / tileWidth;
                
                char sKey[255];
                
                for(kk::Uint i = 0;i< tilecount;i++) {
                    
                    snprintf(sKey, sizeof(sKey), "%u",i);
                    
                    Tile * tile = new Tile();
                    
                    tile->tileSet = this;
                    tile->x = i % column;
                    tile->y = i / column;
                    
                    void * prop = Y_toHeapptr(ctx, props, sKey);
                    
                    if(prop) {
                        duk_push_heapptr(ctx, prop);
                        YStringMapSet(tile->propertys, ctx, -1);
                        duk_pop(ctx);
                    }
 
                    tiles.push_back(tile);
                    
                }
                
            }
            
            duk_pop(ctx);
            
        }
        
        void TileSet::set(kk::CString key,duk_context * ctx,duk_idx_t idx)  {
            
            if(kk::CStringEqual(key, "firstgid")) {
                firstgid = Y_toInt(ctx,idx);
            } else if(kk::CStringEqual(key, "source")) {
                setContent(ctx, Y_toString(ctx, idx,""));
            }
            
        }
        
        static kk::Strong TileMapTileSetAlloc(duk_context * ctx,duk_idx_t idx) {
         
            TileSet * v = new TileSet();
            
            YObjectSet(v, ctx, idx);
            
            return v;
        }
        
        void TileMapLayer::set(kk::CString key,duk_context * ctx,duk_idx_t idx)  {
            
            if(kk::CStringEqual(key, "x")) {
                x = Y_toInt(ctx,idx);
            } else if(kk::CStringEqual(key, "y")) {
                y = Y_toInt(ctx,idx);
            } else if(kk::CStringEqual(key, "width")) {
                width = Y_toInt(ctx,idx);
            } else if(kk::CStringEqual(key, "height")) {
                height = Y_toInt(ctx,idx);
            } else if(kk::CStringEqual(key, "opacity")) {
                opacity = Y_toInt(ctx,idx);
            } else if(kk::CStringEqual(key, "properties")) {
                YStringMapSet(propertys, ctx,idx);
            }
        }
        
        TileMapTileLayer::TileMapTileLayer():_dataBytes(nullptr){
            
        }
        
        TileMapTileLayer::~TileMapTileLayer() {
            if(_dataBytes) {
                delete [] _dataBytes;
            }
        }
        
        kk::Int TileMapTileLayer::get(kk::Int x,kk::Int y) {
            if(_dataBytes != nullptr &&
               x >= this->x && y >= this->y &&
               x < this->x + this->width && y < this->y + this->height) {
                return _dataBytes[(y - this->y) * this->width + (x - this->x)];
            }
            return 0;
        }
        
        void TileMapTileLayer::set(kk::CString key,duk_context * ctx,duk_idx_t idx)  {
            TileMapLayer::set(key, ctx, idx);
            
            if(key == nullptr) {
                
                kk::CString encoding = Y_toString(ctx, idx, "encoding",nullptr);
                kk::CString compression = Y_toString(ctx, idx, "compression",nullptr);
            
                if(encoding == nullptr || encoding == nullptr) {
                    
                    void * data = Y_toHeapptr(ctx, idx, "data");
                    
                    if(data) {
                        
                        int n = width * height;
                        
                        if(_dataBytes) {
                            delete [] _dataBytes;
                            _dataBytes = new kk::Int[n];
                        } else {
                            _dataBytes = new kk::Int[n];
                        }
                        
                        memset(_dataBytes, 0, n * sizeof(kk::Int));
                        
                        duk_push_heapptr(ctx, data);
                        
                        if(duk_is_array(ctx, -1)) {
                            
                            size_t nn = duk_get_length(ctx, -1);
                            
                            for(int i=0;i<nn && i < n;i++ ){
                                duk_get_prop_index(ctx, -1, i);
                                _dataBytes[i] = Y_toInt(ctx, -1);
                                duk_pop(ctx);
                            }
                        }
                        
                    }
                    
                } else if(kk::CStringEqual(encoding, "base64")) {
                    
                    duk_get_prop_string(ctx, idx, "data");
                    
                    if(duk_is_string(ctx, -1)) {
                        duk_base64_decode(ctx, -1);
                        if(duk_is_buffer(ctx, -1)) {
                            
                            size_t nn = 0;
                            void * p = duk_get_buffer_data(ctx, -1, &nn);
                            
                            int n = width * height;
                            
                            if(_dataBytes) {
                                delete [] _dataBytes;
                                _dataBytes = new kk::Int[n];
                            } else {
                                _dataBytes = new kk::Int[n];
                            }
                            
                            memset(_dataBytes, 0, n * sizeof(kk::Int));
                            
                            {
                                z_stream strm;
                                strm.next_in = (Bytef *) p;
                                strm.avail_in = (uInt) nn;
                                strm.avail_out = (uInt) (n * sizeof(kk::Int));
                                strm.total_out = 0;
                                strm.zalloc = Z_NULL;
                                strm.zfree = Z_NULL;
                                strm.next_out = (Bytef *) _dataBytes;
                                
                                int r = Z_OK;
                                
                                if(kk::CStringEqual(compression, "zlib")) {
                                    r = inflateInit(&strm);
                                } else {
                                    r = inflateInit2(&strm,(15+32));
                                }
                                
                                if(r == Z_OK){
                                    
                                    r = inflate(&strm,Z_SYNC_FLUSH);
                                    
                                    if(r == Z_STREAM_END){
                                        
                                    }
                                    else if(r != Z_OK){
                                        kk::Log("TileMap zlib decode error (%d) ",r);
                                    }
                                    
                                    inflateEnd(&strm);
                                }
                            }
                            
                            duk_pop(ctx);
                            
                        } else {
                            duk_pop(ctx);
                        }
                    } else {
                        duk_pop(ctx);
                    }
                    
                }
                
            }
        }
        
        void TileMapImageLayer::set(kk::CString key,duk_context * ctx,duk_idx_t idx) {
            TileMapLayer::set(key, ctx, idx);
  
        }
        
        void TileMapCircleObject::set(kk::CString key,duk_context * ctx,duk_idx_t idx) {
            if(kk::CStringEqual(key, "x")) {
                x = Y_toFloat(ctx,idx);
            } else if(kk::CStringEqual(key, "y")) {
                y = Y_toFloat(ctx,idx);
            } else if(kk::CStringEqual(key, "width")) {
                width = Y_toFloat(ctx,idx);
            } else if(kk::CStringEqual(key, "height")) {
                height = Y_toFloat(ctx,idx);
            } else if(key == nullptr) {
                radius = MIN(width, height) * 0.5f;
            }
        }
        
        void TileMapPolygonObject::set(kk::CString key,duk_context * ctx,duk_idx_t idx) {
            if(kk::CStringEqual(key, "polygon") || kk::CStringEqual(key, "polyline") ) {
                
                if(duk_is_array(ctx, idx)) {
                    size_t n = duk_get_length(ctx, idx);
                    for(int i=0 ;i < n;i++ ){
                        duk_get_prop_index(ctx, idx, i);
                        
                        Point v = {0,0};
                        
                        v.x = Y_toFloat(ctx, -1,"x");
                        v.y = Y_toFloat(ctx, -1,"y");
                        
                        data.push_back(v);
                        
                        duk_pop(ctx);
                    }
                }
                
            }
        }
        
        static kk::Strong TileMapLayerObjectAlloc(duk_context * ctx,duk_idx_t idx) {
            
            if(duk_is_object(ctx, idx)) {
                
                kk::CString v = Y_toString(ctx, idx, "ellipse",nullptr);
                
                if(kk::CStringEqual(v, "true")) {
                    TileMapCircleObject * v = new TileMapCircleObject();
                    YObjectSet(v, ctx, idx);
                    return v;
                }
                
                {
                
                    TileMapPolygonObject * v = nullptr;
                    
                    duk_get_prop_string(ctx, idx, "polygon");
                    
                    if(duk_is_array(ctx, -1)) {
                        v = new TileMapPolygonObject();
                    }
                    
                    duk_pop(ctx);
                    
                    if(v == nullptr) {
                        
                        duk_get_prop_string(ctx, idx, "polyline");
                        
                        if(duk_is_array(ctx, -1)) {
                            v = new TileMapPolygonObject();
                        }
                        
                        duk_pop(ctx);
                    }
                    
                    if(v != nullptr) {
                        YObjectSet(v, ctx, idx);
                        return v;
                    }
                    
                    
                }
            }
            
            return nullptr;
        }
        
        void TileMapObjectLayer::set(kk::CString key,duk_context * ctx,duk_idx_t idx) {
            TileMapLayer::set(key, ctx, idx);
            
            if(kk::CStringEqual(key, "objects")) {
                
                YObjectArraySet(objects, TileMapLayerObjectAlloc, ctx, idx);
                
            }
        }
        
        static kk::Strong TileMapLayerAlloc(duk_context * ctx,duk_idx_t idx);
        
        void TileMapGroupLayer::set(kk::CString key,duk_context * ctx,duk_idx_t idx) {
            TileMapLayer::set(key, ctx, idx);
            
            if(kk::CStringEqual(key, "layers")) {
                YObjectArraySet(layers, TileMapLayerAlloc, ctx, idx);
            }
        }
        
        static kk::Strong TileMapLayerAlloc(duk_context * ctx,duk_idx_t idx) {
            
            TileMapLayer * v = nullptr;
            
            kk::CString type = Y_toString(ctx, idx, "type",nullptr);
            
            if(kk::CStringEqual(type, "tilelayer")) {
                v = new TileMapTileLayer();
                YObjectSet(v, ctx, idx);
            } else if(kk::CStringEqual(type, "imagelayer")) {
                v = new TileMapImageLayer();
                YObjectSet(v, ctx, idx);
            } else if(kk::CStringEqual(type, "objectgroup")) {
                v = new TileMapObjectLayer();
                YObjectSet(v, ctx, idx);
            } else if(kk::CStringEqual(type, "group")) {
                v = new TileMapGroupLayer();
                YObjectSet(v, ctx, idx);
            }
            
            return v;
        }
        
        void TileMap::set(kk::CString key,duk_context * ctx,duk_idx_t idx) {
            if(kk::CStringEqual(key, "width")) {
                width = Y_toInt(ctx,idx);
            } else if(kk::CStringEqual(key, "height")) {
                height = Y_toInt(ctx,idx);
            } else if(kk::CStringEqual(key, "tilewidth")) {
                tileWidth = Y_toInt(ctx,idx);
            } else if(kk::CStringEqual(key, "tileheight")) {
                tileHeight = Y_toInt(ctx,idx);
            } else if(kk::CStringEqual(key, "orientation")) {
                if(Y_isString(ctx,idx, "isometric")) {
                    orientation = TileMapOrientationIsometric;
                } else if(Y_isString(ctx,idx, "staggered")) {
                    orientation = TileMapOrientationStaggered;
                } else if(Y_isString(ctx,idx, "hexagonal")) {
                    orientation = TileMapOrientationHexagonal;
                } else {
                    orientation = TileMapOrientationOrthogonal;
                }
            } else if(kk::CStringEqual(key, "properties")) {
                YStringMapSet(propertys, ctx,idx);
            } else if(kk::CStringEqual(key, "tilesets")) {
                YObjectArraySet(tileSets, TileMapTileSetAlloc, ctx,idx);
            } else if(kk::CStringEqual(key, "layers")) {
                YObjectArraySet(layers, TileMapLayerAlloc, ctx,idx);
            }
        }
        
        void TileMap::exec(Context * context) {
            Body::exec(context);
            
            if(!_loaded) {
                
                String& v = get("path");
                
                if(!v.empty()) {
                    
                    kk::String vv = context->getString(v.c_str());
                    
                    if(!vv.empty()) {
                        
                        kk::Strong jsContext = new kk::script::Context();
                        
                        duk_context * ctx = jsContext.as<kk::script::Context>()->jsContext();
                        
                        kk::script::PushObject(ctx, this);
                        kk::script::PushObject(ctx, context);
                        
                        if(kk::script::decodeJSON(ctx, vv.c_str(), vv.size()) != DUK_EXEC_SUCCESS) {
                            kk::script::Error(ctx, -1);
                        }
                        
                        YObjectSet(this, ctx, -1);
                        
                        duk_pop_n(ctx,3);
                        
                    }
                    
                    _loaded = true;
                    
                }
            }
            
            
        }
       
    }
    
}

