#include <jni.h>
#include "kk-config.h"
#include "kk.h"
#include "duktape.h"
#include "kk-script.h"
#include "GLSliceMapElement.h"
#include "GLImageElement.h"
#include "GAScene.h"
#include "GAShape.h"
#include "GABody.h"
#include "GAActionWalk.h"
#include "require_js.h"
#include "GLAnimation.h"
#include "GADocument.h"
#include "GLTextElement.h"
#include "GLMinimapElement.h"
#include "GLViewportElement.h"
#include "GATileMap.h"
#include "GLTileMapElement.h"
#include "GLSpineElement.h"
#include "GLMetaElement.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include "GLShapeElement.h"


static kk::GL::Context * GetGLContext(duk_context * ctx) {

    kk::GL::Context * v = nullptr;

    duk_get_global_string(ctx,"context");

    if(duk_is_object(ctx,-1)) {
        v = (kk::GL::Context *) kk::script::GetObject(ctx,-1);
    }

    duk_pop(ctx);

    return v;
}

static duk_ret_t KKGLContext_getString(duk_context * ctx) {

    kk::CString path = nullptr;
    kk::GL::Context * GLContext = GetGLContext(ctx);

    int top = duk_get_top(ctx);

    if(top >0  && duk_is_string(ctx, - top)) {
        path = duk_to_string(ctx, -top);
    }

    if(path && GLContext) {

        kk::String code = GLContext->getString(path);

        duk_push_string(ctx,code.c_str());

        return 1;

    }

    return 0;
}

static duk_ret_t KKGLContext_compile(duk_context * ctx) {

    kk::CString path = nullptr;
    kk::CString prefix = nullptr;
    kk::CString suffix = nullptr;
    kk::GL::Context * GLContext = GetGLContext(ctx);

    int top = duk_get_top(ctx);

    if(top > 0  && duk_is_string(ctx, - top)) {
        path = duk_to_string(ctx, -top);
    }

    if(top > 1  && duk_is_string(ctx, - top + 1)) {
        prefix = duk_to_string(ctx, -top + 1);
    }

    if(top > 2  && duk_is_string(ctx, - top + 2)) {
        suffix = duk_to_string(ctx, -top + 2);
    }

    if(path && GLContext) {

        kk::String v = GLContext->getString(path);

        kk::String code;

        if(prefix) {
            code.append(prefix);
        }

        code.append(v);

        if(suffix) {
            code.append(suffix);
        }

        duk_push_string(ctx, path);
        duk_compile_string_filename(ctx, 0, code.c_str());

        return 1;

    }

    return 0;
}

extern "C"
JNIEXPORT jlong JNICALL
Java_cn_kkmofang_game_Context_alloc(JNIEnv *env, jclass type, jstring basePath_) {
    const char *basePath = env->GetStringUTFChars(basePath_, 0);


    kk::GL::Context * GLContext;

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
        kk::script::SetPrototype(ctx, &kk::GA::ActionWalk::ScriptClass);
        kk::script::SetPrototype(ctx, &kk::GA::Document::ScriptClass);
        kk::script::SetPrototype(ctx, &kk::GA::TileMap::ScriptClass);
        kk::script::SetPrototype(ctx, &kk::GL::SliceMapElement::ScriptClass);
        kk::script::SetPrototype(ctx, &kk::GL::ImageElement::ScriptClass);
        kk::script::SetPrototype(ctx, &kk::GL::ShapeElement::ScriptClass);
        kk::script::SetPrototype(ctx, &kk::GL::Animation::ScriptClass);
        kk::script::SetPrototype(ctx, &kk::GL::AnimationItem::ScriptClass);
        kk::script::SetPrototype(ctx, &kk::GL::TextElement::ScriptClass);
        kk::script::SetPrototype(ctx, &kk::GL::MinimapElement::ScriptClass);
        kk::script::SetPrototype(ctx, &kk::GL::ViewportElement::ScriptClass);
        kk::script::SetPrototype(ctx, &kk::GL::TileMapElement::ScriptClass);
        kk::script::SetPrototype(ctx, &kk::GL::SpineElement::ScriptClass);
        kk::script::SetPrototype(ctx, &kk::GL::MetaElement::ScriptClass);


    }

    {
        duk_context *ctx = v->jsContext();

        duk_push_global_object(ctx);


        {
            duk_push_string(ctx, "kk");
            duk_push_object(ctx);

            duk_push_string(ctx, "platform");
            duk_push_string(ctx, "android");
            duk_put_prop(ctx, -3);

            duk_push_string(ctx, "getString");
            duk_push_c_function(ctx, KKGLContext_getString, 1);
            duk_put_prop(ctx, -3);


            duk_push_string(ctx, "compile");
            duk_push_c_function(ctx, KKGLContext_compile, 3);
            duk_put_prop(ctx, -3);

            duk_put_prop(ctx, -3);
        }


        {
            kk::Strong v = new kk::GL::Context();

            GLContext = v.as<kk::GL::Context>();

            GLContext->setBasePath(basePath);

            GLContext->init();

            duk_push_string(ctx, "context");
            kk::script::PushObject(ctx, v.get());
            duk_def_prop(ctx,-3,DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_CLEAR_CONFIGURABLE | DUK_DEFPROP_CLEAR_WRITABLE);

        }

        {
            kk::Strong v = new kk::GA::Element();
            duk_push_string(ctx, "element");
            kk::script::PushObject(ctx, v.get());
            duk_def_prop(ctx,-3,DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_CLEAR_CONFIGURABLE | DUK_DEFPROP_HAVE_WRITABLE);
        }


        duk_pop(ctx);
    }

    {
        duk_context * ctx = v->jsContext();
        duk_eval_lstring_noresult(ctx, (char *)require_js, sizeof(require_js));
    }


    jlong ret = (jlong) (long) v->jsContext();

    env->ReleaseStringUTFChars(basePath_, basePath);

    return ret;
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_game_Context_run(JNIEnv *env, jclass type, jlong ptr) {

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

    if(context ) {

        kk::String code = context->getString("main.js");
        kk::String evelCode ;

        evelCode.append("(function(){");
        evelCode.append(code);
        evelCode.append("})");

        duk_push_string(ctx,"main.js");
        duk_compile_string_filename(ctx,0,evelCode.c_str());

        if(duk_is_function(ctx, -1)) {

            if(DUK_EXEC_SUCCESS != duk_pcall(ctx, 0)) {

                kk::script::Error(ctx, -1);
                duk_pop(ctx);

            } else {

                if(DUK_EXEC_SUCCESS != duk_pcall(ctx, 0)) {
                    kk::script::Error(ctx, -1);
                }

                duk_pop(ctx);
            }
        } else {
            duk_pop(ctx);
        }

    }

}


extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_game_Context_dealloc(JNIEnv *env, jclass type, jlong ptr) {

    duk_context * ctx = (duk_context *) ptr;

    kk::script::Context * v = kk::script::GetContext(ctx);

    v->release();
}

extern "C"
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
        element = dynamic_cast<kk::GA::Element *>( kk::script::GetObject(ctx,-1) );
    }

    duk_pop(ctx);

    duk_pop(ctx);

    if(context && element) {
        context->tick();
        context->exec(element);
        context->draw(element);
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_game_Context_setViewport__JFFF(JNIEnv *env, jclass type, jlong ptr, jfloat width,
                                                jfloat height, jfloat scale) {

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

        context->setViewport( width * scale, height* scale);

        s.projection = glm::scale(kk::GL::mat4(1),kk::GL::vec3(2.0f / (width* scale),-2.0f / (height* scale),-0.0001));

        GLsizei _width = (GLsizei) width;
        GLsizei _height = (GLsizei) height;

        glViewport(0,0,_width,_height);

        glDisable(GL_DEPTH_TEST);

        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE , GL_ONE_MINUS_SRC_ALPHA);

    }

}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_game_Context_emit(JNIEnv *env, jclass type, jlong ptr) {

    duk_context * ctx = (duk_context *) ptr;
    kk::GA::Element * element = nullptr;

    duk_push_global_object(ctx);

    duk_push_string(ctx,"element");
    duk_get_prop(ctx,-2);

    if(duk_is_object(ctx,-1)) {
        element = dynamic_cast<kk::GA::Element *>( kk::script::GetObject(ctx,-1) );
    }

    duk_pop(ctx);

    duk_pop(ctx);

    if(element && duk_is_string(ctx,-2) && duk_is_object(ctx,-1)) {

        kk::CString name = duk_to_string(ctx,-2);

        kk::Strong v = new kk::ElementEvent();
        kk::ElementEvent * ev = v.as<kk::ElementEvent>();
        ev->data = new kk::script::Object(kk::script::GetContext(ctx),-1);
        duk_pop(ctx);

        element->emit(name,ev);
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_game_Context_on(JNIEnv *env, jclass type, jlong ptr) {

    duk_context * ctx = (duk_context *) ptr;
    kk::GA::Element * element = nullptr;

    duk_push_global_object(ctx);

    duk_push_string(ctx,"element");
    duk_get_prop(ctx,-2);

    if(duk_is_object(ctx,-1)) {
        element = dynamic_cast<kk::GA::Element *>( kk::script::GetObject(ctx,-1) );
    }

    duk_pop(ctx);

    duk_pop(ctx);

    if(element && duk_is_string(ctx,-2) && duk_is_function(ctx,-1)) {
        element->duk_on(ctx);
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_game_Context_off(JNIEnv *env, jclass type, jlong ptr) {

    duk_context * ctx = (duk_context *) ptr;
    kk::GA::Element * element = nullptr;

    duk_push_global_object(ctx);

    duk_push_string(ctx,"element");
    duk_get_prop(ctx,-2);

    if(duk_is_object(ctx,-1)) {
        element = dynamic_cast<kk::GA::Element *>( kk::script::GetObject(ctx,-1) );
    }

    duk_pop(ctx);

    duk_pop(ctx);

    if(element && duk_is_string(ctx,-1)) {
        element->duk_off(ctx);
    }

}

extern "C"
JNIEXPORT jlong JNICALL
Java_cn_kkmofang_game_Weak_alloc(JNIEnv *env, jclass type) {
    kk::Weak * v = new kk::Weak();
    return (jlong) (long) v;
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_game_Weak_dealloc(JNIEnv *env, jclass type, jlong ptr) {
    kk::Weak * v = (kk::Weak *) (long) ptr;
    delete v;
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_game_Weak_set__JJ(JNIEnv *env, jclass type, jlong ptr, jlong object) {
    kk::Weak * v = (kk::Weak *) (long) ptr;
    v->set((kk::Object *) (long) object);
}

extern "C"
JNIEXPORT jlong JNICALL
Java_cn_kkmofang_game_Weak_get__J(JNIEnv *env, jclass type, jlong ptr) {
    kk::Weak * v = (kk::Weak *) (long) ptr;
    return (jlong) (long) v->get();
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_game_WeakTexture_setTexture__JII_3B(JNIEnv *env, jclass type, jlong ptr,
                                                               jint width, jint height,
                                                               jbyteArray data_) {
    jbyte *data = env->GetByteArrayElements(data_, NULL);

    kk::Weak * v = (kk::Weak *) (long) ptr;
    kk::GL::Texture * texture = v->as<kk::GL::Texture>();

    if(texture != nullptr) {

        glBindTexture(GL_TEXTURE_2D, texture->texture());

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        texture->setSize(width,height);

    }

    env->ReleaseByteArrayElements(data_, data, 0);
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_game_WeakImage_setStatus__JILjava_lang_String_2(JNIEnv *env, jclass type,
                                                                 jlong ptr, jint status,
                                                                 jstring errmsg_) {
    const char *errmsg = env->GetStringUTFChars(errmsg_, 0);

    kk::Weak * v = (kk::Weak *) (long) ptr;
    kk::GL::Image * image = v->as<kk::GL::Image>();

    if(image != nullptr) {
        image->setStatus((kk::GL::ImageStatus)status);
        image->setError(errmsg);
    }

    env->ReleaseStringUTFChars(errmsg_, errmsg);
}

extern "C"
JNIEXPORT jstring JNICALL
Java_cn_kkmofang_game_WeakImage_url__J(JNIEnv *env, jclass type, jlong ptr) {

    kk::Weak * v = (kk::Weak *) (long) ptr;
    kk::GL::Image * image = v->as<kk::GL::Image>();

    if(image != nullptr) {
        return env->NewStringUTF(image->uri());
    }

    return NULL;
}



namespace kk {

    namespace GL {

        void ContextGetImage(Context * context, Image * image) {

            jboolean isAttach = false;

            JNIEnv * env =  kk_env(&isAttach);

            jclass clazz = env->FindClass("cn/kkmofang/game/Context");

            if(clazz != nullptr) {
                jmethodID method = env->GetStaticMethodID(clazz,"ContextGetImage","(J)V");
                if(method != nullptr) {
                    env->CallStaticVoidMethod(clazz,method,(jlong) (long) image);
                }
                env->DeleteLocalRef(clazz);
            }

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

        }

        static int ContextIntValueWithColor(vec4 color) {
            return ((int)(color.a * 0x0ff) << 24) | ((int)(color.r * 0x0ff) << 16) | ((int)(color.g * 0x0ff) << 8) | ((int)(color.b * 0x0ff));
        }

        static jobject ContextNewTextPaint(JNIEnv * env,Paint & paint) {

            jclass clazz = env->FindClass("cn/kkmofang/game/TextPaint");

            if(clazz != nullptr) {

                jmethodID method = env->GetMethodID(clazz,"<init>","()V");

                if(method != nullptr) {

                    jobject object = env->NewObject(clazz,method);

                    method = env->GetMethodID(clazz,"set","(Ljava/lang/String;FZZIFIF)V");

                    if(method != nullptr) {
                        env->CallVoidMethod(object,method
                                ,env->NewStringUTF(paint.fontFimlay.c_str())
                                ,paint.fontSize
                                            ,paint.fontStyle == FontStyleItalic
                                            ,paint.fontWeight == FontWeightBold
                                            ,ContextIntValueWithColor(paint.textColor)
                                            ,paint.maxWidth
                                            ,ContextIntValueWithColor(paint.textStroke.color)
                                            ,paint.textStroke.size
                                );
                    }

                    return object;
                }

                env->DeleteLocalRef(clazz);
            }

            return nullptr;
        }

        void ContextGetStringTexture(Context * context,Texture * texture ,kk::CString text, Paint & paint) {

            if(text == nullptr) {
                text = "";
            }

            jboolean isAttach = false;

            JNIEnv * env =  kk_env(&isAttach);

            jclass clazz = env->FindClass("cn/kkmofang/game/Context");

            if(clazz != nullptr) {
                jmethodID method = env->GetStaticMethodID(clazz,"ContextGetStringTexture","(JLjava/lang/String;Lcn/kkmofang/game/TextPaint;)V");
                if(method != nullptr) {
                    env->CallStaticVoidMethod(clazz,method,(jlong) (long) texture,env->NewStringUTF(text),ContextNewTextPaint(env,paint));
                }
                env->DeleteLocalRef(clazz);
            }

            if(isAttach) {
                gJavaVm->DetachCurrentThread();
            }

        }

    }
}
