#include <jni.h>
#include "kk-config.h"
#include "kk.h"
#include "duktape.h"
#include "GLContext.h"
#include "kk-script.h"
#include "kk-app.h"
#include "kk-dispatch.h"
#include "kk-ev.h"
#include "kk-ws.h"
#include "kk-wk.h"
#include "kk-http.h"
#include <event.h>
#include <evdns.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

static kk::Application * getApp(duk_context * ctx) {

    kk::Application * v = nullptr;

    duk_get_global_string(ctx,"__app");

    if(duk_is_pointer(ctx,-1)) {
        v = (kk::Application *) duk_to_pointer(ctx,-1);
    }

    duk_pop(ctx);

    return v;
}

static kk::DispatchQueue * getQueue(duk_context * ctx) {

    kk::DispatchQueue * v = nullptr;

    duk_get_global_string(ctx,"__queue");

    if(duk_is_pointer(ctx,-1)) {
        v = (kk::DispatchQueue *) duk_to_pointer(ctx,-1);
    }

    duk_pop(ctx);

    return v;

}

static void KKGLViewElement_EventOnCreateContext (duk_context * ctx,kk::DispatchQueue * queue, duk_context * newContext) {

    kk::Application * app = nullptr;

    duk_get_global_string(ctx, "__app");

    if(duk_is_pointer(ctx, -1)) {
        app = (kk::Application *) duk_to_pointer(ctx, -1);
    }

    duk_pop(ctx);

    if(app != nullptr) {
        app->installContext(newContext);
        duk_push_pointer(newContext, app);
        duk_put_global_string(newContext, "__app");
    }

    event_base * base = kk::ev_base(newContext);
    evdns_base * dns = kk::ev_dns(newContext);

    {

        kk::script::SetPrototype(newContext, &kk::WebSocket::ScriptClass);
        kk::script::SetPrototype(newContext, &kk::Http::ScriptClass);

        kk::Strong v = new kk::Http(base,dns);

        kk::script::PushObject(newContext, v.get());
        duk_put_global_string(newContext, "http");

    }
}

extern "C"
JNIEXPORT jlong JNICALL
Java_cn_kkmofang_game_Context_alloc(JNIEnv *env, jclass type, jstring basePath_) {
    const char *basePath = env->GetStringUTFChars(basePath_, 0);

    kk::script::Context * v = new kk::script::Context();

    v->retain();

    {
        duk_context *ctx = v->jsContext();

        event_base *base = event_base_new();
        evdns_base *dns = evdns_base_new(base, EVDNS_BASE_INITIALIZE_NAMESERVERS);

        kk::DispatchQueue *queue = new kk::DispatchQueue("main",base);

        queue->retain();

        kk::ev_openlibs(ctx, base, dns);
        kk::wk_openlibs(ctx, queue, KKGLViewElement_EventOnCreateContext);

        {
            kk::script::SetPrototype(ctx, &kk::WebSocket::ScriptClass);
            kk::script::SetPrototype(ctx, &kk::Http::ScriptClass);

            kk::Strong v = new kk::Http(base, dns);

            kk::script::PushObject(ctx, v.get());
            duk_put_global_string(ctx, "http");
        }

        kk::Application * app = new kk::Application(basePath,0,v);

        app->retain();

        duk_push_pointer(ctx, app);
        duk_put_global_string(ctx, "__app");

        kk::GL::Context * GAContext = app->GAContext();

        GAContext->init();

    }

    jlong ret = (jlong) (long) v->jsContext();

    env->ReleaseStringUTFChars(basePath_, basePath);

    return ret;
}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_game_Context_run(JNIEnv *env, jclass type, jlong ptr) {

    duk_context * ctx = (duk_context *) ptr;

    kk::Application * app = getApp(ctx);

    if(app != nullptr && !app->isRunning()) {
        app->run();
    }

}


extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_game_Context_dealloc(JNIEnv *env, jclass type, jlong ptr) {

    duk_context * ctx = (duk_context *) ptr;

    kk::Application * app = getApp(ctx);
    kk::DispatchQueue * queue = getQueue(ctx);
    kk::script::Context * jsContext = kk::script::GetContext(ctx);
    event_base * base = kk::ev_base(ctx);
    evdns_base * dns = kk::ev_dns(ctx);

    if(queue) {
        queue->join();
    }

    if(jsContext) {
        jsContext->release();
    }

    if(app) {
        app->document()->off("", nullptr);
        app->release();
    }

    if(queue) {
        queue->release();
    }

    if(dns) {
        evdns_base_free(dns,0);
    }

    if(base) {
        event_base_free(base);
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_game_Context_exec(JNIEnv *env, jclass type, jlong ptr) {

    duk_context * ctx = (duk_context *) ptr;

    event_base * base = kk::ev_base(ctx);

    kk::Application * app = getApp(ctx);

    if(base) {
        event_base_loop(base,EVLOOP_ONCE | EVLOOP_NONBLOCK);
    }

    if(app) {
        app->exec();
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_game_Context_setViewport__JFFF(JNIEnv *env, jclass type, jlong ptr, jfloat width,
                                                jfloat height, jfloat scale) {

    duk_context * ctx = (duk_context *) ptr;

    kk::Application * app = getApp(ctx);

    if(app) {

        kk::GL::Context * context = app->GAContext();

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

    kk::Application * app = getApp(ctx);

    if(app && duk_is_string(ctx,-2) && duk_is_object(ctx,-1)) {

        kk::CString name = duk_to_string(ctx,-2);

        kk::Strong v = new kk::ElementEvent();
        kk::ElementEvent * ev = v.as<kk::ElementEvent>();
        ev->data = new kk::script::Object(kk::script::GetContext(ctx),-1);
        duk_pop(ctx);

        app->document()->emit(name,ev);
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_game_Context_on(JNIEnv *env, jclass type, jlong ptr) {

    duk_context * ctx = (duk_context *) ptr;

    kk::Application * app = getApp(ctx);

    if(app && duk_is_string(ctx,-2) && duk_is_function(ctx,-1)) {
        app->document()->duk_on(ctx);
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_cn_kkmofang_game_Context_off(JNIEnv *env, jclass type, jlong ptr) {

    duk_context * ctx = (duk_context *) ptr;

    kk::Application * app = getApp(ctx);

    if(app && duk_is_string(ctx,-1)) {
        app->document()->duk_off(ctx);
    }

}

extern "C"
JNIEXPORT jfloat JNICALL
Java_cn_kkmofang_game_Context_loadingProgress(JNIEnv *env, jclass type, jlong ptr) {

    duk_context * ctx = (duk_context *) ptr;

    kk::Application * app = getApp(ctx);

    if(app) {
        kk::Document * doc = app->document();
        if(doc) {
            kk::Element *element = doc->rootElement();
            if (element) {
                app->GAContext()->loadingProgress(element);
            } else {
                return 1;
            }
        }
    }

    return 0;

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
Java_cn_kkmofang_game_WeakTexture_setTexture__JII_3BF(JNIEnv *env, jclass type, jlong ptr,
                                                                jint width, jint height,
                                                                jbyteArray data_, jfloat scale) {
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

        texture->setSize((GLsizei) (width * scale),(GLsizei) (height * scale));

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
