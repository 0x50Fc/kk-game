
cmake_minimum_required(VERSION 3.4.1)

add_library(duktape
            SHARED
            IMPORTED)

set_target_properties(duktape
            PROPERTIES IMPORTED_LOCATION
            ${CMAKE_SOURCE_DIR}/lib/${ANDROID_ABI}/libduktape.so)

add_library(event
            SHARED
            IMPORTED)

set_target_properties(event
            PROPERTIES IMPORTED_LOCATION
            ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libevent.so)

add_library(kk-object
            SHARED
            IMPORTED)

set_target_properties(kk-object
            PROPERTIES IMPORTED_LOCATION
            ${CMAKE_SOURCE_DIR}/lib/${ANDROID_ABI}/libkk-object.so)

add_library(kk-element
            SHARED
            IMPORTED)

set_target_properties(kk-element
            PROPERTIES IMPORTED_LOCATION
            ${CMAKE_SOURCE_DIR}/lib/${ANDROID_ABI}/libkk-element.so)

add_library(chipmunk
        STATIC
        src/main/jni/Chipmunk2D/src/chipmunk.c
        src/main/jni/Chipmunk2D/src/cpArbiter.c
        src/main/jni/Chipmunk2D/src/cpArray.c
        src/main/jni/Chipmunk2D/src/cpBBTree.c
        src/main/jni/Chipmunk2D/src/cpBody.c
        src/main/jni/Chipmunk2D/src/cpCollision.c
        src/main/jni/Chipmunk2D/src/cpConstraint.c
        src/main/jni/Chipmunk2D/src/cpDampedRotarySpring.c
        src/main/jni/Chipmunk2D/src/cpDampedSpring.c
        src/main/jni/Chipmunk2D/src/cpGearJoint.c
        src/main/jni/Chipmunk2D/src/cpGrooveJoint.c
        src/main/jni/Chipmunk2D/src/cpHashSet.c
        src/main/jni/Chipmunk2D/src/cpHastySpace.c
        src/main/jni/Chipmunk2D/src/cpMarch.c
        src/main/jni/Chipmunk2D/src/cpPinJoint.c
        src/main/jni/Chipmunk2D/src/cpPivotJoint.c
        src/main/jni/Chipmunk2D/src/cpPolyShape.c
        src/main/jni/Chipmunk2D/src/cpPolyline.c
        src/main/jni/Chipmunk2D/src/cpRatchetJoint.c
        src/main/jni/Chipmunk2D/src/cpRobust.c
        src/main/jni/Chipmunk2D/src/cpRotaryLimitJoint.c
        src/main/jni/Chipmunk2D/src/cpShape.c
        src/main/jni/Chipmunk2D/src/cpSimpleMotor.c
        src/main/jni/Chipmunk2D/src/cpSlideJoint.c
        src/main/jni/Chipmunk2D/src/cpSpace.c
        src/main/jni/Chipmunk2D/src/cpSpaceComponent.c
        src/main/jni/Chipmunk2D/src/cpSpaceDebug.c
        src/main/jni/Chipmunk2D/src/cpSpaceHash.c
        src/main/jni/Chipmunk2D/src/cpSpaceQuery.c
        src/main/jni/Chipmunk2D/src/cpSpaceStep.c
        src/main/jni/Chipmunk2D/src/cpSpatialIndex.c
        src/main/jni/Chipmunk2D/src/cpSweep1D.c
        )



add_library(spine
        STATIC
        src/main/jni/spine/src/spine/Animation.c
        src/main/jni/spine/src/spine/AnimationState.c
        src/main/jni/spine/src/spine/AnimationStateData.c
        src/main/jni/spine/src/spine/Array.c
        src/main/jni/spine/src/spine/Atlas.c
        src/main/jni/spine/src/spine/AtlasAttachmentLoader.c
        src/main/jni/spine/src/spine/Attachment.c
        src/main/jni/spine/src/spine/AttachmentLoader.c
        src/main/jni/spine/src/spine/Bone.c
        src/main/jni/spine/src/spine/BoneData.c
        src/main/jni/spine/src/spine/BoundingBoxAttachment.c
        src/main/jni/spine/src/spine/ClippingAttachment.c
        src/main/jni/spine/src/spine/Color.c
        src/main/jni/spine/src/spine/Event.c
        src/main/jni/spine/src/spine/EventData.c
        src/main/jni/spine/src/spine/IkConstraint.c
        src/main/jni/spine/src/spine/IkConstraintData.c
        src/main/jni/spine/src/spine/MeshAttachment.c
        src/main/jni/spine/src/spine/PathAttachment.c
        src/main/jni/spine/src/spine/PathConstraint.c
        src/main/jni/spine/src/spine/PathConstraintData.c
        src/main/jni/spine/src/spine/PointAttachment.c
        src/main/jni/spine/src/spine/RegionAttachment.c
        src/main/jni/spine/src/spine/Skeleton.c
        src/main/jni/spine/src/spine/SkeletonBinary.c
        src/main/jni/spine/src/spine/SkeletonBounds.c
        src/main/jni/spine/src/spine/SkeletonClipping.c
        src/main/jni/spine/src/spine/SkeletonData.c
        src/main/jni/spine/src/spine/Skin.c
        src/main/jni/spine/src/spine/Slot.c
        src/main/jni/spine/src/spine/SlotData.c
        src/main/jni/spine/src/spine/TransformConstraint.c
        src/main/jni/spine/src/spine/TransformConstraintData.c
        src/main/jni/spine/src/spine/Triangulator.c
        src/main/jni/spine/src/spine/VertexAttachment.c
        src/main/jni/spine/src/spine/VertexEffect.c
        src/main/jni/spine/src/spine/extension.c
        )



add_library(kk-game
            SHARED
            src/main/jni/kk-game.cc
            src/main/jni/kk.c
            src/main/jni/app/kk-y.cc
            src/main/jni/app/kk-app.cc
            src/main/jni/app/GA/GAContext.cc
            src/main/jni/app/GA/GABody.cc
            src/main/jni/app/GA/GAScene.cc
            src/main/jni/app/GA/GAShape.cc
            src/main/jni/app/GA/GADocument.cc
            src/main/jni/app/GA/GAAction.cc
            src/main/jni/app/GA/GAActionWalk.cc
            src/main/jni/app/GA/GATileMap.cc
            src/main/jni/app/GL/GLContext.cc
            src/main/jni/app/GL/GLImageElement.cc
            src/main/jni/app/GL/GLMinimapElement.cc
            src/main/jni/app/GL/GLTextElement.cc
            src/main/jni/app/GL/GLSliceMapElement.cc
            src/main/jni/app/GL/GLViewportElement.cc
            src/main/jni/app/GL/GLAnimation.cc
            src/main/jni/app/GL/GLTileMapElement.cc
            src/main/jni/app/GL/GLShapeElement.cc
            src/main/jni/app/GL/GLSpineElement.cc
            src/main/jni/app/GL/GLMetaElement.cc
            src/main/jni/glsl/GLSLTexture.cc
            src/main/jni/glsl/GLSLFillColor.cc
            )

include_directories(
            src/main/jni/duktape
            src/main/jni/md5
            src/main/jni/app
            src/main/jni/app/GA
            src/main/jni/app/GL
            src/main/jni/js
            src/main/jni/glsl
            src/main/jni/Chipmunk2D/include
            src/main/jni/spine/include
            src/main/jni/
            src/main/jni/libevent/include
            src/main/jni/kk-object/include
            src/main/jni/kk-element/include
            )

set_target_properties(kk-game PROPERTIES
    CXX_STANDARD 11
)

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

target_link_libraries(
                    kk-game
                    kk-element
                    kk-object
                    duktape
                    event
                    chipmunk
                    spine
                    android
                    z
                    GLESv2
                    EGL
                    log
                    )
