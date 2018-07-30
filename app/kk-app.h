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
#include "GAContext.h"

namespace kk {
    
    extern kk::Float Kernel;
    
    class Application;
    
    class Application : public Object {
    public:
        Application(CString basePath,kk::Uint64 appid,kk::script::Context * jsContext);
        virtual ~Application();
        virtual kk::GA::Context * GAContext();
        virtual kk::script::Context * jsContext();
        virtual kk::GA::Element * GAElement();
        virtual duk_context * dukContext();
        virtual void exec();
        virtual void run();
        virtual void runCommand(kk::CString command);
        virtual kk::Uint64 appid();
        virtual void installContext(duk_context * ctx);
    protected:
        Strong _jsContext;
        Strong _GAContext;
        Strong _GAElement;
        kk::Uint64 _appid;
    };
    
    
}


#endif
