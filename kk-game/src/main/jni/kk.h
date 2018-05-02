//
// Created by zhanghailong on 2016/11/7.
//

#ifndef KK_ANDROID_LUA_KK_H
#define KK_ANDROID_LUA_KK_H

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

extern JavaVM * gJavaVm;

JNIEnv * kk_env(jboolean * isAttated);

void kk_logv(const char * format,va_list va);

void kk_log(const char * format,...);

#ifdef __cplusplus
}
#endif

#endif //KK_ANDROID_LUA_KK_H
