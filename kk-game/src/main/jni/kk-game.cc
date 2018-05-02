#include <jni.h>
#include "kk.h"
#include "duktape.h"
#include "kk-config.h"
#include "kk-script.h"
#include "GLSliceMapElement.h"
#include "GLImageElement.h"
#include "GAScene.h"
#include "GAShape.h"
#include "GABody.h"
#include "GAActionMove.h"
#include "require_js.h"
#include "GLAnimation.h"
#include "GADocument.h"
#include "GLTextElement.h"
#include "GLMinimapElement.h"
#include "GLViewportElement.h"
#include "GATileMap.h"
#include "GLTileMapElement.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>


JNIEXPORT jlong JNICALL
Java_cn_kkmofang_game_Context_alloc(JNIEnv *env, jclass type) {

    kk::script::Context * v = new kk::script::Context();

    v->retain();

    {
        duk_context * ctx = v->jsContext();

        kk::script::SetPrototype(ctx, &kk::ElementEvent::ScriptClass);
        kk::script::SetPrototype(ctx, &kk::GA::Context::ScriptClass);
        kk::script::SetPrototype(ctx, &kk::GA::Scene::ScriptClass);
        kk::script::SetPrototype(ctx, &kk::GA::Shape::ScriptClass);
        kk::script::SetPrototype(ctx, &kk::GA::Body::ScriptClass);
        kk::script::SetPrototype(ctx, &kk::GA::Action::ScriptClass);
        kk::script::SetPrototype(ctx, &kk::GA::ActionMove::ScriptClass);
        kk::script::SetPrototype(ctx, &kk::GA::TileMap::ScriptClass);
        kk::script::SetPrototype(ctx, &kk::GA::Document::ScriptClass);
        kk::script::SetPrototype(ctx, &kk::GL::SliceMapElement::ScriptClass);
        kk::script::SetPrototype(ctx, &kk::GL::ImageElement::ScriptClass);
        kk::script::SetPrototype(ctx, &kk::GL::Animation::ScriptClass);
        kk::script::SetPrototype(ctx, &kk::GL::AnimationItem::ScriptClass);
        kk::script::SetPrototype(ctx, &kk::GL::TextElement::ScriptClass);
        kk::script::SetPrototype(ctx, &kk::GL::MinimapElement::ScriptClass);
        kk::script::SetPrototype(ctx, &kk::GL::ViewportElement::ScriptClass);
        kk::script::SetPrototype(ctx, &kk::GL::TileMapElement::ScriptClass);

    }

    {
        duk_context * ctx = v->jsContext();

        duk_push_global_object(ctx);

        {
            duk_push_string(ctx,"kk");
            duk_push_object(ctx);

            duk_push_string(ctx,"platform");
            duk_push_string(ctx,"android");
            duk_put_prop(ctx,-3);

            duk_put_prop(ctx,-3);
        }


        {
            kk::Strong v = new kk::GL::Context();

            duk_push_string(ctx,"context");
            kk::script::PushObject(ctx,v.get());
            duk_put_prop(ctx,-3);
        }

        {
            kk::Strong v = new kk::GA::Element();
            duk_push_string(ctx,"element");
            kk::script::PushObject(ctx,v.get());
            duk_put_prop(ctx,-3);
        }

        duk_pop(ctx);
    }

    {
        duk_context * ctx = v->jsContext();
        duk_eval_lstring_noresult(ctx, (char *)require_js, sizeof(require_js));
    }

    return (jlong) (long) v->jsContext();
}


JNIEXPORT void JNICALL
Java_cn_kkmofang_game_Context_dealloc(JNIEnv *env, jclass type, jlong ptr) {

    duk_context * ctx = (duk_context *) ptr;

    kk::script::Context * v = kk::script::GetContext(ctx);

    v->release();
}

JNIEXPORT void JNICALL
Java_cn_kkmofang_game_Context_exec(JNIEnv *env, jclass type, jlong ptr) {

    duk_context * ctx = (duk_context *) ptr;

    kk::GL::Context * context = nullptr;
    kk::GA::Element * element = nullptr;

    duk_push_global_object(ctx);

    duk_push_string(ctx,"context");
    duk_get_prop(ctx,-2);

    if(duk_is_object(ctx,-1)) {
        context = dynamic_cast<kk::GL::Context *>( kk::script::GetObject(ctx,-1) );
    }

    duk_pop(ctx);

    duk_push_string(ctx,"element");
    duk_get_prop(ctx,-2);

    if(duk_is_object(ctx,-1)) {
        element = dynamic_cast<kk::GL::Element *>( kk::script::GetObject(ctx,-1) );
    }

    duk_pop(ctx);

    duk_pop(ctx);

    if(context && element) {
        context->tick();
        context->exec(element);
        context->draw(element);
    }

}


JNIEXPORT void JNICALL
Java_cn_kkmofang_game_Context_setViewport(JNIEnv *env, jclass type, jlong ptr, jfloat width,
                                          jfloat height) {

    duk_context * ctx = (duk_context *) ptr;

    kk::GL::Context * context = nullptr;

    duk_push_global_object(ctx);

    duk_push_string(ctx,"context");
    duk_get_prop(ctx,-2);

    if(duk_is_object(ctx,-1)) {
        context = dynamic_cast<kk::GL::Context *>( kk::script::GetObject(ctx,-1) );
    }

    duk_pop(ctx);

    duk_pop(ctx);

    if(context) {

        kk::GL::ContextState & s = context->state();

        context->setViewport( width, height);

        s.projection = glm::scale(kk::GL::mat4(1),kk::GL::vec3(2.0f / (width),-2.0f / (height),-0.0001));

    }

}


namespace kk {

    namespace GL {

        void ContextGetImage(Context * context, Image * image) {

        }

        void ContextGetStringTexture(Context * context,Texture * texture ,kk::CString text, Paint & paint) {

        }

    }
}

