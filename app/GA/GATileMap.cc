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
#include "yaml.h"
#include "base64.h"
#include <zlib.h>
#include <chipmunk/chipmunk.h>

namespace kk {
    
    namespace GA {
        
        IMP_SCRIPT_CLASS_BEGIN(&kk::GA::Element::ScriptClass, TileMap, GATileMap)
        
        IMP_SCRIPT_CLASS_END
        
        struct TileMapDocument : yaml_document_t {
        public:
            TileMap * map;
            Context * context;
        };
        
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
        
        void TileSet::setContent(yaml_document_t * doc,kk::CString path)  {
            
            TileMapDocument * mDocument = (TileMapDocument *)(doc);
            
            kk::String basePath = mDocument->map->get("path");
            
            basePath = kk::CStringPathDeleteLast(basePath.c_str());
            
            kk::String v = mDocument->context->absolutePath(kk::CStringPathAppend(basePath.c_str(), path).c_str());
            
            FILE * fd = fopen(v.c_str(), "r");
            
            if(fd != nullptr) {
                
                yaml_parser_t ps;
                
                yaml_parser_initialize(&ps);
                
                yaml_parser_set_input_file(&ps, fd);
                
                yaml_document_t document;
                
                if(yaml_parser_load(&ps, &document)) {
                    
                    yaml_node_t * node = yaml_document_get_root_node(&document);
                    
                    if(node && node->type == YAML_MAPPING_NODE) {
                        
                        kk::Uint tileCount = 0;
                        std::map<kk::String,yaml_node_t *> tilepropertys;
                        
                        yaml_node_pair_t *  p = node->data.mapping.pairs.start;
                        while(p != node->data.mapping.pairs.top) {
                            yaml_node_t * key = yaml_document_get_node(&document, p->key);
                            yaml_node_t * value = yaml_document_get_node(&document, p->value);
                            if(key->type == YAML_SCALAR_NODE) {
                                if(Y_isString(key,"imagewidth")) {
                                    width = Y_toInt(value);
                                } else if(Y_isString(key,"imageheight")) {
                                    height = Y_toInt(value);
                                } else if(Y_isString(key,"tilewidth")) {
                                    tileWidth = Y_toInt(value);
                                } else if(Y_isString(key,"tileheight")) {
                                    tileHeight = Y_toInt(value);
                                } else if(Y_isString(key, "properties")) {
                                    YStringMapSet(propertys, &document, value);
                                } else if(Y_isString(key, "tileproperties")) {
                                    YMapSet(tilepropertys, &document, value);
                                } else if(Y_isString(key, "tilecount")) {
                                    tileCount = Y_toInt(value);
                                } else if(Y_isString(key, "image")) {
                                    image = kk::CStringPathAppend(basePath.c_str(), Y_toString(value, ""));
                                } else if(Y_isString(key, "tileoffset")) {
                                    {
                                        YMap m;
                                        YMapSet(m, &document, value);
                                        YMap::iterator i = m.find("x");
                                        if(i != m.end()) {
                                            tileX = Y_toFloat(i->second);
                                        }
                                        i = m.find("y");
                                        if(i != m.end()) {
                                            tileY = Y_toFloat(i->second);
                                        }
                                    }
                                }
                            }
                            p ++;
                        }
                        
                        char sKey[128];
                        
                        sw = (Float) tileWidth / (Float) width;
                        sh = (Float) tileHeight / (Float) height;
                        
                        kk::Uint column = width / tileWidth;
                        
                        for(kk::Uint i = 0;i< tileCount;i++) {
                            
                            snprintf(sKey, sizeof(sKey), "%u",i);
                            
                            Tile * tile = new Tile();
                            
                            tile->tileSet = this;
                            tile->x = i % column;
                            tile->y = i / column;
                            
                            std::map<kk::String,yaml_node_t *>::iterator ii = tilepropertys.find(sKey);
                            
                            if(ii != tilepropertys.end()) {
                                yaml_node_t * node = ii->second;
                                YStringMapSet(tile->propertys, &document, node);
                            }
                            
                            tiles.push_back(tile);
                            
                        }
                        
                    }
                    
                    yaml_document_delete(&document);
                } else {
                    kk::Log("(%d,%d) %s",ps.problem_mark.line,ps.problem_mark.column,ps.problem);
                }
                
                yaml_parser_delete(&ps);
                
                fclose(fd);
            } else {
                kk::Log("Not Open %s",v.c_str());
            }
            
        }
        
        void TileSet::set(kk::CString key,yaml_document_t * document,yaml_node_t * node)  {
            
            if(kk::CStringEqual(key, "firstgid")) {
                firstgid = Y_toInt(node);
            } else if(kk::CStringEqual(key, "source")) {
                setContent(document, Y_toString(node, ""));
            }
            
        }
        
        static kk::Strong TileMapTileSetAlloc(yaml_document_t * document,yaml_node_t * node) {
         
            TileSet * v = new TileSet();
            
            YObjectSet(v, document, node);
            
            return v;
        }
        
        void TileMapLayer::set(kk::CString key,yaml_document_t * document,yaml_node_t * node)  {
            
            if(kk::CStringEqual(key, "x")) {
                x = Y_toInt(node);
            } else if(kk::CStringEqual(key, "y")) {
                y = Y_toInt(node);
            } else if(kk::CStringEqual(key, "width")) {
                width = Y_toInt(node);
            } else if(kk::CStringEqual(key, "height")) {
                height = Y_toInt(node);
            } else if(kk::CStringEqual(key, "opacity")) {
                opacity = Y_toInt(node);
            } else if(kk::CStringEqual(key, "properties")) {
                YStringMapSet(propertys, document, node);
            }
        }
        
        TileMapTileLayer::TileMapTileLayer():_compression(0),_encoding(0),_data(0),_dataBytes(nullptr){
            
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
        
        void TileMapTileLayer::set(kk::CString key,yaml_document_t * document,yaml_node_t * node)  {
            TileMapLayer::set(key, document, node);
            
            if(kk::CStringEqual(key, "compression")) {
                _compression = Y_toString(node, nullptr);
            } else if(kk::CStringEqual(key, "encoding")) {
                _encoding = Y_toString(node, nullptr);
            } else if(kk::CStringEqual(key, "data")) {
                _data = node;
            }
            
            if(key == nullptr) {
                
                if(_encoding == nullptr || _compression == nullptr) {
                    if(_data && _data->type == YAML_SEQUENCE_NODE) {
                        yaml_node_item_t * p = _data->data.sequence.items.start;
                        int i = 0;
                        int n = width * height;
                        if(_dataBytes) {
                            delete [] _dataBytes;
                        }
                        if(_dataBytes == nullptr) {
                            _dataBytes = new kk::Int[n];
                        }
                        while(p != _data->data.sequence.items.top && i < n) {
                            yaml_node_t * v = yaml_document_get_node(document, *p);
                            _dataBytes[i] = Y_toInt(v);
                            i++;
                            p ++;
                        }
                    }
                } else if(kk::CStringEqual(_encoding, "base64") && _data) {
                    kk::CString p = Y_toString(_data, nullptr);
                    
                    if(p) {
                        
                        size_t n = strlen(p);
                        size_t size = base64_declen(n);
                        uint8_t * enc = new uint8_t[size];
                        
                        n = base64_decode(p, enc, size);
                        
                        size = width * height;
                        
                        if(_dataBytes) {
                            delete [] _dataBytes;
                        }
                        
                        if(_dataBytes == nullptr) {
                            _dataBytes = new kk::Int[size];
                        }
                        
                        memset(_dataBytes, 0,size * sizeof(kk::Int));
                        
                        {
                            z_stream strm;
                            strm.next_in = enc;
                            strm.avail_in = (uInt) n;
                            strm.avail_out = (uInt) size * sizeof(kk::Int);
                            strm.total_out = 0;
                            strm.zalloc = Z_NULL;
                            strm.zfree = Z_NULL;
                            strm.next_out = (Bytef *) _dataBytes;
                            
                            int r = Z_OK;
                            
                            if(kk::CStringEqual(_compression, "zlib")) {
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
                        
                        delete [] enc;
                        
                    }
                }
                
            }
        }
        
        void TileMapImageLayer::set(kk::CString key,yaml_document_t * document,yaml_node_t * node) {
            TileMapLayer::set(key, document, node);
            
            
        }
        
        void TileMapCircleObject::set(kk::CString key,yaml_document_t * document,yaml_node_t * node) {
            if(kk::CStringEqual(key, "x")) {
                x = Y_toFloat(node);
            } else if(kk::CStringEqual(key, "y")) {
                y = Y_toFloat(node);
            } else if(kk::CStringEqual(key, "width")) {
                width = Y_toFloat(node);
            } else if(kk::CStringEqual(key, "height")) {
                height = Y_toFloat(node);
            } else if(key == nullptr) {
                radius = MIN(width, height) * 0.5f;
            }
        }
        
        void TileMapPolygonObject::set(kk::CString key,yaml_document_t * document,yaml_node_t * node) {
            if(kk::CStringEqual(key, "polygon") || kk::CStringEqual(key, "polyline") ) {
                
                if(node->type == YAML_SEQUENCE_NODE) {
                    
                    yaml_node_item_t * p = node->data.sequence.items.start;
                    
                    while(p != node->data.sequence.items.top) {
                        
                        yaml_node_t * item = yaml_document_get_node(document, *p);
                        
                        if(item->type == YAML_MAPPING_NODE) {
                            Point v = {0,0};
                            
                            yaml_node_pair_t * n = item->data.mapping.pairs.start;
                            
                            while(n != item->data.mapping.pairs.top) {
                                
                                yaml_node_t * key = yaml_document_get_node(document, n->key);
                                
                                if(Y_isString(key, "x")) {
                                    v.x = Y_toFloat(yaml_document_get_node(document, n->value));
                                } else if(Y_isString(key, "y")) {
                                    v.y = Y_toFloat(yaml_document_get_node(document, n->value));
                                }
                                
                                n ++;
                            }
                            
                            data.push_back(v);
                        }
                        
                        
                        
                        p ++;
                    }
                }
            }
        }
        
        static kk::Strong TileMapLayerObjectAlloc(yaml_document_t * document,yaml_node_t * node) {
            
            if(node->type == YAML_MAPPING_NODE) {
                
                yaml_node_pair_t * p = node->data.mapping.pairs.start;
                
                while(p != node->data.mapping.pairs.end) {
                    
                    yaml_node_t * key = yaml_document_get_node(document, p->key);
                    
                    if(Y_isString(key, "ellipse")) {
                        yaml_node_t * value = yaml_document_get_node(document, p->value);
                        if(Y_isString(value, "true")) {
                            TileMapCircleObject * v = new TileMapCircleObject();
                            YObjectSet(v, document, node);
                            return v;
                        }
                    } else if(Y_isString(key, "polygon")) {
                        TileMapPolygonObject * v = new TileMapPolygonObject();
                        YObjectSet(v, document, node);
                        return v;
                    } else if(Y_isString(key, "polyline")) {
                        TileMapPolygonObject * v = new TileMapPolygonObject();
                        YObjectSet(v, document, node);
                        return v;
                    }
                    
                    p ++;
                }
                
            }
            
            return nullptr;
        }
        
        void TileMapObjectLayer::set(kk::CString key,yaml_document_t * document,yaml_node_t * node) {
            TileMapLayer::set(key, document, node);
            
            if(kk::CStringEqual(key, "objects")) {
                
                YObjectArraySet(objects, TileMapLayerObjectAlloc, document, node);
                
            }
        }
        
        static kk::Strong TileMapLayerAlloc(yaml_document_t * document,yaml_node_t * node);
        
        void TileMapGroupLayer::set(kk::CString key,yaml_document_t * document,yaml_node_t * node) {
            TileMapLayer::set(key, document, node);
            
            if(kk::CStringEqual(key, "layers")) {
                YObjectArraySet(layers, TileMapLayerAlloc, document, node);
            }
        }
        
        static kk::Strong TileMapLayerAlloc(yaml_document_t * document,yaml_node_t * node) {
            
            TileMapLayer * v = nullptr;
            
            if(node->type == YAML_MAPPING_NODE) {
                
                yaml_node_t * type = nullptr;
                
                yaml_node_pair_t *  p = node->data.mapping.pairs.start;
                
                while(p != node->data.mapping.pairs.top) {
                    
                    yaml_node_t * key = yaml_document_get_node(document, p->key);
                    
                    if(Y_isString(key, "type")) {
                        type = yaml_document_get_node(document, p->value);
                        break;
                    }
                    p ++;
                }
                
                if(type) {
                    if(Y_isString(type, "tilelayer")) {
                        v = new TileMapTileLayer();
                        YObjectSet(v, document, node);
                    } else if(Y_isString(type, "imagelayer")) {
                        v = new TileMapImageLayer();
                        YObjectSet(v, document, node);
                    } else if(Y_isString(type, "objectgroup")) {
                        v = new TileMapObjectLayer();
                        YObjectSet(v, document, node);
                    } else if(Y_isString(type, "group")) {
                        v = new TileMapGroupLayer();
                        YObjectSet(v, document, node);
                    }
                }
                
            }
            
            return v;
        }
        
        void TileMap::set(kk::CString key,yaml_document_t * document,yaml_node_t * node) {
            if(kk::CStringEqual(key, "width")) {
                width = Y_toInt(node);
            } else if(kk::CStringEqual(key, "height")) {
                height = Y_toInt(node);
            } else if(kk::CStringEqual(key, "tilewidth")) {
                tileWidth = Y_toInt(node);
            } else if(kk::CStringEqual(key, "tileheight")) {
                tileHeight = Y_toInt(node);
            } else if(kk::CStringEqual(key, "orientation")) {
                if(Y_isString(node, "isometric")) {
                    orientation = TileMapOrientationIsometric;
                } else if(Y_isString(node, "staggered")) {
                    orientation = TileMapOrientationStaggered;
                } else if(Y_isString(node, "hexagonal")) {
                    orientation = TileMapOrientationHexagonal;
                } else {
                    orientation = TileMapOrientationOrthogonal;
                }
            } else if(kk::CStringEqual(key, "properties")) {
                YStringMapSet(propertys, document, node);
            } else if(kk::CStringEqual(key, "tilesets")) {
                YObjectArraySet(tileSets, TileMapTileSetAlloc, document, node);
            } else if(kk::CStringEqual(key, "layers")) {
                YObjectArraySet(layers, TileMapLayerAlloc, document, node);
            }
        }
        
        void TileMap::exec(Context * context) {
            Body::exec(context);
            
            if(!_loaded) {
                
                String& v = get("path");
                
                if(!v.empty()) {
                    
                    kk::String path = context->absolutePath(v.c_str());
                    
                    FILE * fd = fopen(path.c_str(), "r");
                    
                    if(fd == nullptr) {
                        kk::Log("Not Open %s",path.c_str());
                        return;
                    }
                    
                    yaml_parser_t ps;
                    
                    yaml_parser_initialize(&ps);
                    
                    yaml_parser_set_input_file(&ps, fd);
                    
                    TileMapDocument document;
                    
                    document.context = context;
                    document.map = this;
                    
                    if(yaml_parser_load(&ps, &document)) {
                        
                        yaml_node_t * node = yaml_document_get_root_node(&document);
                        
                        if(node && node->type == YAML_MAPPING_NODE) {
                            YObjectSet(this, &document, node);
                        }
                        
                        yaml_document_delete(&document);
                    } else {
                        kk::Log("(%d,%d) %s",ps.problem_mark.line,ps.problem_mark.column,ps.problem);
                    }
                    
                    yaml_parser_delete(&ps);
                    
                    fclose(fd);
                    
                    _loaded = true;
                    
                }
            }
        }
       
    }
    
}

