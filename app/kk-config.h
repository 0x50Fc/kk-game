//
//  kk-config.h
//
//  Created by 张海龙 on 2018/02/01
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef _KK_CONFIG_H
#define _KK_CONFIG_H

#include <sys/time.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <math.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>

#if defined(__APPLE__)
#include <TargetConditionals.h>

#if TARGET_OS_OSX
#define KK_PLATFORM_OSX 1
#endif

#if TARGET_OS_IOS
#define KK_PLATFORM_IOS 1
#endif

#endif

#if defined(__linux) || defined(__linux__) || defined(linux)
#define KK_PLATFORM_LINUX 1
#endif

#if defined(__ANDROID__)
#define KK_PLATFORM_ANDROID 1
#endif

#ifndef MAXFLOAT
#define MAXFLOAT    0x1.fffffep+127f
#endif

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#endif
