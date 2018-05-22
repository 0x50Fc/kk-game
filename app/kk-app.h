//
//  kk-app.h
//
//  Created by 张海龙 on 2018/02/01
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef _KK_APP_H
#define _KK_APP_H

#include "kk-object.h"
#include "kk-script.h"
#include "kk-object.h"
#include "kk-uv.h"
#include "GAContext.h"

struct uv_loop_s;

namespace kk {
    
    extern kk::Float Kernel;
    
    class Application;
    
    typedef void (*ApplicationExitCB)(Application * app);
    
    class Application : public Object {
    public:
        Application(CString basePath,kk::Uint64 appid,ApplicationExitCB cb);
        virtual ~Application();
        virtual kk::GA::Context * GAContext();
        virtual kk::script::Context * jsContext();
        virtual kk::GA::Element * GAElement();
        virtual duk_context * dukContext();
        virtual void runCommand(kk::CString command);
        virtual void run(uv_loop_t * loop);
        virtual void run();
        virtual void exit();
        virtual kk::Boolean isRunning();
        virtual kk::Uint64 appid();
    protected:
        Strong _jsContext;
        Strong _GAContext;
        Strong _GAElement;
        uv_timer_t _timer;
        kk::Boolean _running;
        kk::Uint64 _appid;
        ApplicationExitCB _cb;
    };
    
    
}


#endif
