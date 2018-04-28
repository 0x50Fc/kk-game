//
//  AppDelegate.m
//  demo
//
//  Created by hailong11 on 2018/2/5.
//  Copyright © 2018年 kkmofang.cn. All rights reserved.
//

#import "AppDelegate.h"
#import <KKApplication/KKApplication.h>
#import <KKGame/KKGame.h>

@interface AppDelegate ()<KKShellDelegate>

@end

@implementation AppDelegate

+(void) initialize {
    [super initialize];
    
    [KKGLViewElement class];
    [KKGamepadElement class];
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Override point for customization after application launch.
    
    [self.window makeKeyAndVisible];
    
    NSString * path = [[[NSBundle mainBundle] bundlePath] stringByAppendingPathComponent:@"main"];
    
    [[KKShell main] setDelegate:self];
    [[KKShell main] open:[NSURL fileURLWithPath:path]];
    
    return YES;
}

-(BOOL) KKShell:(KKShell *) shell openApplication:(KKApplication *) application {
    
    [application.observer set:@[@"auth"] value:@{@"uid":@"5870450982",@"gsid":@"_2A253y3NZDeRhGeNG7FIV9S7Fwz6IHXVVNB0RrDV_PUJbm9AKLWHQkW1NSz-eYoyuUV9IHezNPRKunAY8-wBjo4Wb"}];
    
    NSString * proxy = [[application.observer get:@[@"info",@"http",@"proxy"] defaultValue:nil] kk_stringValue];
    
    if(proxy != nil) {
        
        NSArray * vs = [proxy componentsSeparatedByString:@":"];
        
        NSURLSessionConfiguration * config = [NSURLSessionConfiguration defaultSessionConfiguration];
        
        config.connectionProxyDictionary =
        @{(id)kCFNetworkProxiesHTTPEnable:@YES,
          (id)kCFNetworkProxiesHTTPProxy:vs[0],
          (id)kCFNetworkProxiesHTTPPort:@([vs count] >1 ? [vs[1] intValue]:80)};
        
        application.http = [[KKHttp alloc] initWithConfiguration:config];
        
    }
    
    return NO;
}


- (void)applicationWillResignActive:(UIApplication *)application {
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and invalidate graphics rendering callbacks. Games should use this method to pause the game.
}


- (void)applicationDidEnterBackground:(UIApplication *)application {
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}


- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Called as part of the transition from the background to the active state; here you can undo many of the changes made on entering the background.
}


- (void)applicationDidBecomeActive:(UIApplication *)application {
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}


- (void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}


@end
