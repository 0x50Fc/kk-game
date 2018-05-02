//
// Created by zhanghailong on 2016/11/7.
//

#include <jni.h>
#include <android/log.h>
#include "kk.h"


JavaVM * gJavaVm = 0;

void kk_logv(const char * format , va_list va){
    __android_log_vprint(ANDROID_LOG_DEBUG,"kk",format,va);
}

void kk_log(const char * format,...) {
    va_list  va;
    va_start(va,format);
    kk_logv(format,va);
    va_end(va);
}

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved){

    gJavaVm = vm;

    kk_log("JNI_OnLoad");

    return JNI_VERSION_1_4;
}

JNIEXPORT void JNI_OnUnload(JavaVM* vm, void* reserved){

    if(vm == gJavaVm) {
        gJavaVm = 0;
    }

    kk_log("JNI_OnUnload");
}


JNIEnv * kk_env(jboolean * isAttated) {

    int status;
    JNIEnv *envnow = 0;

    if(gJavaVm){

        status = (*gJavaVm)->GetEnv(gJavaVm,(void **) &envnow, JNI_VERSION_1_4);

        if(status < 0)
        {
            status = (*gJavaVm)->AttachCurrentThread(gJavaVm,&envnow, 0);

            if(status < 0)
            {
                return 0;
            }

            * isAttated = 1;
        }
    }
    return envnow;
}