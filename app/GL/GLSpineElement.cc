//
//  GLSpineElement.cc
//  KKGame
//
//  Created by zhanghailong on 2018/5/2.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include "kk-config.h"
#include "GLSpineElement.h"
#include "kk-string.h"
#import <spine/spine.h>
#import <spine/extension.h>

void _spAtlasPage_createTexture (spAtlasPage* page, const char* path) {
    kk::GL::SpineElement * e = (kk::GL::SpineElement *) page->atlas->rendererObject;
    page->rendererObject = e->image(path);
}

void _spAtlasPage_disposeTexture (spAtlasPage* page) {
    page->rendererObject = nullptr;
}

char* _spUtil_readFile (const char* path, int* length) {
    
    struct stat st;
    
    if( stat(path, &st) == -1) {
        kk::Log("Not Open %s",path);
        * length = 0;
        return nullptr;
    }
    
    FILE * fd = fopen(path, "r");
    
    if(fd == nullptr ){
        kk::Log("Not Open %s",path);
        * length = 0;
        return nullptr;
    }
    
    char * data = MALLOC(char, (size_t) st.st_size  +1);
    
    size_t n = fread(data, 1, (size_t) st.st_size, fd);
    
    data[n]  = 0;
    
    *length = (int) n;
    
    fclose(fd);
    

    return data;
    
}

namespace kk {
    
    namespace GL {
    
        IMP_SCRIPT_CLASS_BEGIN(&Element::ScriptClass, SpineElement, GLSpineElement)
        
        IMP_SCRIPT_CLASS_END
        
        
        SpineElement::SpineElement()
            :_loaded(false),_spAtlas(nullptr),_spSkeletonData(nullptr)
            ,_spSkeleton(nullptr),_spAnimationStateData(nullptr)
            ,_spAnimationState(nullptr),_context(nullptr),_prevTimeInterval(0)
            ,_updatting(false),_spTrackEntry(nullptr),_spClipping(nullptr) {

        }
        
        SpineElement::~SpineElement() {
            
            if(_spAnimationState) {
                spAnimationState_dispose(_spAnimationState);
            }
            
            if(_spAnimationStateData) {
                spAnimationStateData_dispose(_spAnimationStateData);
            }
            
            if(_spSkeleton) {
                spSkeleton_dispose(_spSkeleton);
            }
            
            if(_spSkeletonData) {
                spSkeletonData_dispose(_spSkeletonData);
            }
            
            if(_spAtlas) {
                spAtlas_dispose(_spAtlas);
            }

            if(_spClipping) {
                spSkeletonClipping_dispose(_spClipping);
            }
            
        }
        
        void SpineElement::changedKey(String& key) {
            Element::changedKey(key);
            
            _updatting = true;
        }
        
        #define MAX_VERTICES_PER_ATTACHMENT 2048
        static float worldVerticesPositions[MAX_VERTICES_PER_ATTACHMENT];
        static TextureVertex vertices[MAX_VERTICES_PER_ATTACHMENT];
        
        static void addVertex(float x, float y, float u, float v, int* index) {
            TextureVertex* vertex = &vertices[*index];
            vertex->position.x = x;
            vertex->position.y = - y;
            vertex->position.z = 0;
            vertex->texCoord.x = u;
            vertex->texCoord.y = v;
            *index += 1;
        }

        
        void SpineElement::onDraw(Context * context) {
            Element::onDraw(context);
            
            _context = context;
            
            if(!_loaded) {
                
                kk::String path = get("path");
                
                if(!path.empty()) {
                    
                    path = context->absolutePath(path.c_str());
                    
                    kk::String basePath =  kk::CStringPathDeleteExtension(path.c_str());
                    kk::String atlas = basePath + ".atlas";
                    
                    _spAtlas = spAtlas_createFromFile(atlas.c_str(), this);
                    
                    if(_spAtlas == nullptr) {
                        kk::Log("Not Open %s",atlas.c_str());
                        _loaded = true;
                        return;
                    }
                    
                    if(kk::CStringHasSuffix(path.c_str(), ".json")) {
                        
                        spSkeletonJson* json = spSkeletonJson_create(_spAtlas);
                        
                        json->scale = 1.0f;
                        
                        _spSkeletonData = spSkeletonJson_readSkeletonDataFile(json, path.c_str());
                    
                        if (_spSkeletonData == nullptr) {
                            kk::Log("Not Open %s",path.c_str());
                            _loaded = true;
                            spSkeletonJson_dispose(json);
                            return;
                        }
                    
                        spSkeletonJson_dispose(json);
                        
                    } else {
                        
                        spSkeletonBinary* binary = spSkeletonBinary_create(_spAtlas);
                        
                        binary->scale = 1.0f;
                
                        _spSkeletonData = spSkeletonBinary_readSkeletonDataFile(binary, path.c_str());
                        
                        if (_spSkeletonData == nullptr) {
                            kk::Log("Not Open %s",path.c_str());
                            _loaded = true;
                            spSkeletonBinary_dispose(binary);
                            return;
                        }
                        
                        spSkeletonBinary_dispose(binary);
                    }
                    
                    _spClipping = spSkeletonClipping_create();
                    
                    _spAnimationStateData = spAnimationStateData_create(_spSkeletonData);
                    
                    _spAnimationState = spAnimationState_create(_spAnimationStateData);
                    
                    _spSkeleton = spSkeleton_create(_spSkeletonData);
                    
                    _updatting = true;
                    _loaded = true;
                }
                
            }
            
            if(_spSkeleton) {
                
                float dt = 0;
                
                if(_prevTimeInterval != 0) {
                    dt = (float) (context->current() - _prevTimeInterval) * 0.001f;
                }
                
                _prevTimeInterval = context->current();
                
                if(_updatting) {
                    
                    kk::String name = get("animation");
                    
                    if(!name.empty()) {
                        
                        spAnimation * anim = spSkeletonData_findAnimation(_spSkeletonData, name.c_str());
                        
                        if(anim) {
                            if(_spTrackEntry == nullptr || _spTrackEntry->animation != anim) {
                                _spTrackEntry = spAnimationState_setAnimation(_spAnimationState, 0, anim, kk::GA::booleanValue(get("loop")) ? 1 : 0);
                            }
                        
                        } else {
                            kk::Log("Spine Not Found Animation %s",name.c_str());
                        }
                    }
                    
                    _updatting = false;
                }
                
                spAnimationState_update(_spAnimationState, dt);
                
                spAnimationState_apply(_spAnimationState, _spSkeleton);
                
                spSkeleton_updateWorldTransform(_spSkeleton);
                
                

                for (int i = 0; i < _spSkeleton->slotsCount; ++i) {
                    
                    spSlot* slot = _spSkeleton->drawOrder[i];
                    spAttachment* attachment = slot->attachment;
                    
                    if (!attachment) continue;
                    
                    switch (slot->data->blendMode) {
                        case SP_BLEND_MODE_NORMAL:
                            glBlendFunc(GL_ONE , GL_ONE_MINUS_SRC_ALPHA);
                            break;
                        case SP_BLEND_MODE_ADDITIVE:
                            glBlendFunc(GL_ONE , GL_ONE);
                            break;
                        case SP_BLEND_MODE_MULTIPLY:
                            glBlendFunc(GL_DST_COLOR , GL_ONE_MINUS_SRC_ALPHA);
                            break;
                        case SP_BLEND_MODE_SCREEN:
                            glBlendFunc(GL_ONE , GL_ONE_MINUS_SRC_COLOR);
                            break;
                        default:
                            glBlendFunc(GL_ONE , GL_ONE_MINUS_SRC_ALPHA);
                    }

                    float tintR = _spSkeleton->color.r * slot->color.r;
                    float tintG = _spSkeleton->color.g * slot->color.g;
                    float tintB = _spSkeleton->color.b * slot->color.b;
                    float tintA = _spSkeleton->color.a * slot->color.a;
                    
                    Image* texture = 0;
                    int vertexIndex = 0;
                    if (attachment->type == SP_ATTACHMENT_REGION) {
                        
                        
                        spRegionAttachment* regionAttachment = (spRegionAttachment*)attachment;
                        
                        texture = (Image*)((spAtlasRegion*)regionAttachment->rendererObject)->page->rendererObject;
                        
                        spRegionAttachment_computeWorldVertices(regionAttachment, slot->bone, worldVerticesPositions, 0, 2);
                        
                        addVertex(worldVerticesPositions[0], worldVerticesPositions[1],
                                  regionAttachment->uvs[0], regionAttachment->uvs[1],
                                   &vertexIndex);
                        
                        addVertex(worldVerticesPositions[2], worldVerticesPositions[3],
                                  regionAttachment->uvs[2], regionAttachment->uvs[3],
                                   &vertexIndex);
                        
                        addVertex(worldVerticesPositions[4], worldVerticesPositions[5],
                                  regionAttachment->uvs[4], regionAttachment->uvs[5],
                                   &vertexIndex);
                        
                        addVertex(worldVerticesPositions[4], worldVerticesPositions[5],
                                  regionAttachment->uvs[4], regionAttachment->uvs[5],
                                   &vertexIndex);
                        
                        addVertex(worldVerticesPositions[6], worldVerticesPositions[7],
                                  regionAttachment->uvs[6], regionAttachment->uvs[7],
                                   &vertexIndex);
                        
                        addVertex(worldVerticesPositions[0], worldVerticesPositions[1],
                                  regionAttachment->uvs[0], regionAttachment->uvs[1],
                                   &vertexIndex);
                    } else if (attachment->type == SP_ATTACHMENT_MESH) {
                        
                        spMeshAttachment* mesh = (spMeshAttachment*)attachment;
                        
                       
                        if (mesh->super.worldVerticesLength > MAX_VERTICES_PER_ATTACHMENT) continue;
                        
                        
                        texture = (Image*)((spAtlasRegion*)mesh->rendererObject)->page->rendererObject;
                        
                       
                        spVertexAttachment_computeWorldVertices(SUPER(mesh), slot, 0, mesh->super.worldVerticesLength, worldVerticesPositions, 0, 2);
                        
                       
                        for (int i = 0; i < mesh->trianglesCount; ++i) {
                            int index = mesh->triangles[i] << 1;
                            addVertex(worldVerticesPositions[index], worldVerticesPositions[index + 1],
                                      mesh->uvs[index], mesh->uvs[index + 1],
                                       &vertexIndex);
                        }
                        
                    }
                    
                    context->store();
                    
                    ContextState & state = context->state();
                    
                    state.opacity = tintR * tintG * tintB * tintA;
                    
                    context->drawTexture(texture, GL_TRIANGLES, vertices, vertexIndex);
                    
                    context->restore();
                }
                    
            }
            
            _context = nullptr;
        }
        
        Image * SpineElement::image(kk::CString path) {
            
            if(path == nullptr) {
                return nullptr;
            }
            
            std::map<kk::String,kk::Strong>::iterator i = _images.find(path);
            
            if(i != _images.end()) {
                return i->second.as<Image>();
            }
            
            if(_context != nullptr) {
                
                kk::Strong v = _context->image(path);
                
                Image * image = v.as<Image>();
                
                if(image){
                    _images[path] = image;
                }
                
                return image;
            }
            
            return nullptr;
        }
        
    }
    
}
