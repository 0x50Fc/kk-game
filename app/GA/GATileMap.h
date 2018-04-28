//
//  GATileMap.h
//  KKGame
//
//  Created by zhanghailong on 2018/4/27.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef GATileMap_h
#define GATileMap_h

#include "GAContext.h"
#include "GABody.h"
#include "kk-yaml.h"

namespace kk {
    
    namespace GA {
        
        enum TileMapOrientation {
            TileMapOrientationOrthogonal,
            TileMapOrientationIsometric,
            TileMapOrientationStaggered,
            TileMapOrientationHexagonal
        };
        
        class Tile : public kk::Object {
        public:
            kk::Weak tileSet;
            std::map<kk::String,kk::String> propertys;
        };
        
        class TileSet : public kk::Object , public YObject {
        public:
            TileSet():width(0),height(0),tileWidth(0),tileHeight(0),firstgid(0){}
            virtual void set(kk::CString key,yaml_document_t * document,yaml_node_t * node) ;
            kk::String image;
            kk::Uint width;
            kk::Uint height;
            kk::Uint tileWidth;
            kk::Uint tileHeight;
            kk::Int firstgid;
            std::map<kk::String,kk::String> propertys;
            std::vector<kk::Strong> tiles;
        protected:
            virtual void setContent(yaml_document_t * document,kk::CString path) ;
        };
        
        class TileMapLayer : public kk::Object ,public YObject{
        public:
            TileMapLayer():width(0),height(0),x(0),y(0),opacity(1){}
            virtual void set(kk::CString key,yaml_document_t * document,yaml_node_t * node) ;
            kk::Uint x;
            kk::Uint y;
            kk::Uint width;
            kk::Uint height;
            Float opacity;
            std::map<kk::String,kk::String> propertys;
        };
        
        class TileMapTileLayer : public TileMapLayer {
        public:
            TileMapTileLayer();
            virtual ~TileMapTileLayer();
            virtual void set(kk::CString key,yaml_document_t * document,yaml_node_t * node) ;
            virtual int get(kk::Int x,kk::Int y);
        protected:
            kk::CString _compression;
            kk::CString _encoding;
            yaml_node_t * _data;
            kk::Int * _dataBytes;
        };
        
        class TileMapGroupLayer : public TileMapLayer {
        public:
            virtual void set(kk::CString key,yaml_document_t * document,yaml_node_t * node) ;
            std::vector<kk::Strong> layers;
        };
        
        class TileMapImageLayer : public TileMapLayer {
        public:
            virtual void set(kk::CString key,yaml_document_t * document,yaml_node_t * node) ;
            kk::Strong image;
        };
        
        class TileMapPolygonObject : public kk::Object {
        public:
            virtual void set(kk::CString key,yaml_document_t * document,yaml_node_t * node) ;
            std::vector<Point> data;
        };
        
        class TileMapCircleObject : public kk::Object {
        public:
            TileMapCircleObject():x(0),y(0),radius(0){}
            virtual void set(kk::CString key,yaml_document_t * document,yaml_node_t * node) ;
            Float x;
            Float y;
            Float radius;
        };
        
        class TileMapObjectLayer : public TileMapLayer {
        public:
            virtual void set(kk::CString key,yaml_document_t * document,yaml_node_t * node) ;
            std::vector<kk::Strong> objects;
        };
        
        class TileMap : public Body,public YObject  {
            
        public:
            TileMap();
            virtual kk::Boolean isLoaded();
            virtual void exec(Context * context);
            virtual void set(kk::CString key,yaml_document_t * document,yaml_node_t * node) ;
            
            DEF_SCRIPT_CLASS
            
            kk::Uint height;
            kk::Uint width;
            kk::Uint tileHeight;
            kk::Uint tileWidth;
            TileMapOrientation orientation;
            std::vector<kk::Strong> tileSets;
            std::map<kk::String,kk::String> propertys;
            std::vector<kk::Strong> layers;
        protected:
            kk::Boolean _loaded;
            
        };
        
    }
    
}


#endif /* GATileMap_hpp */
