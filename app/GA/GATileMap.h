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
#include "kk-y.h"

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
            kk::Int x;
            kk::Int y;
            std::map<kk::String,kk::String> propertys;
        };
        
        class TileSet : public kk::Object , public YObject {
        public:
            TileSet():width(0),height(0),tileWidth(0),tileHeight(0),firstgid(0),sw(0),sh(0),tileX(0),tileY(0){}
            virtual void set(kk::CString key,duk_context * ctx,duk_idx_t idx) ;
            kk::String image;
            kk::Int width;
            kk::Int height;
            kk::Int tileWidth;
            kk::Int tileHeight;
            kk::Int tileX;
            kk::Int tileY;
            kk::Float sw;
            kk::Float sh;
            kk::Int firstgid;
            std::map<kk::String,kk::String> propertys;
            std::vector<kk::Strong> tiles;
        protected:
            virtual void setContent(duk_context * ctx,kk::CString path) ;
        };
        
        class TileMapLayer : public kk::Object ,public YObject{
        public:
            TileMapLayer():width(0),height(0),x(0),y(0),opacity(1){}
            virtual void set(kk::CString key,duk_context * ctx,duk_idx_t idx) ;
            kk::Int x;
            kk::Int y;
            kk::Int width;
            kk::Int height;
            Float opacity;
            std::map<kk::String,kk::String> propertys;
            kk::Strong draw;
        };
        
        class TileMapTileLayer : public TileMapLayer {
        public:
            TileMapTileLayer();
            virtual ~TileMapTileLayer();
            virtual void set(kk::CString key,duk_context * ctx,duk_idx_t idx) ;
            virtual kk::Int get(kk::Int x,kk::Int y);
        protected:
            kk::Int * _dataBytes;
        };
        
        class TileMapGroupLayer : public TileMapLayer {
        public:
            virtual void set(kk::CString key,duk_context * ctx,duk_idx_t idx) ;
            std::vector<kk::Strong> layers;
        };
        
        class TileMapImageLayer : public TileMapLayer {
        public:
            virtual void set(kk::CString key,duk_context * ctx,duk_idx_t idx) ;
            kk::Strong image;
        };
        
        class TileMapPolygonObject : public kk::Object ,public kk::YObject  {
        public:
            virtual void set(kk::CString key,duk_context * ctx,duk_idx_t idx) ;
            std::vector<Point> data;
        };
        
        class TileMapCircleObject : public kk::Object ,public kk::YObject {
        public:
            TileMapCircleObject():x(0),y(0),width(0),height(0),radius(0){}
            virtual void set(kk::CString key,duk_context * ctx,duk_idx_t idx) ;
            Float x;
            Float y;
            Float width;
            Float height;
            Float radius;
        };
        
        class TileMapObjectLayer : public TileMapLayer {
        public:
            virtual void set(kk::CString key,duk_context * ctx,duk_idx_t idx) ;
            std::vector<kk::Strong> objects;
        };
        
        struct TileLocal {
            kk::Int x;
            kk::Int y;
        };
        
        class TileMap : public Body,public YObject  {
            
        public:

            virtual kk::Boolean isLoaded();
            virtual void exec(Context * context);
            virtual void set(kk::CString key,duk_context * ctx,duk_idx_t idx) ;
            virtual Tile * tile(kk::Int gid);
            virtual TileLocal local(Point global);
            virtual Point global(TileLocal local);
            
            DEF_SCRIPT_CLASS_NOALLOC
            
            KK_DEF_ELEMENT_CREATE(TileMap)
            
            kk::Int height;
            kk::Int width;
            kk::Int tileHeight;
            kk::Int tileWidth;
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
