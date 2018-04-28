//
//  kk-app.h
//
//  Created by 张海龙 on 2018/02/01
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#ifndef _KK_APP_H
#define _KK_APP_H

#include "duktape.h"

#ifdef __cplusplus
extern "C" {
#endif

#define KKApplicationKernel 1.0
    
    struct KKApplication;
    
	struct KKApplication {
		duk_context * jsContext;
        void * object;
	};

	struct KKApplication * KKApplicationCreate(void);

	void KKApplicationRun(struct KKApplication * app,const char * basePath);

	void KKApplicationExit(struct KKApplication * app);

	void KKApplicationExec(struct KKApplication * app);
    
    struct KKApplication * KKApplicationGet(duk_context * jsContext);
    
    void duk_push_string_ptr(duk_context * ctx, void * ptr);
    
    void KKApplicationError(struct KKApplication * app, duk_idx_t idx);
    
#ifdef __cplusplus
}
#endif

#endif
