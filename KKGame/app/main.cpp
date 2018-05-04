//
//  main.cpp
//  app
//
//  Created by hailong11 on 2018/5/4.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#include "kk-config.h"
#include "kk-app.h"

int main(int argc, const char * argv[]) {
    
    kk::Strong v = new kk::Application("/Users/hailong11/Documents/src/git.sc.weibo.com/kk/fruit/srv/");
    
    kk::Application * app = v.as<kk::Application>();
    
    app->run();
    
    return 0;
}
