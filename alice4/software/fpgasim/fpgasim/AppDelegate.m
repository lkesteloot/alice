//
//  AppDelegate.m
//  fpgasim
//
//  Created by Lawrence Kesteloot on 5/12/16.
//  Copyright © 2016 Earwig, Inc. All rights reserved.
//

#import "AppDelegate.h"
#import "Server.h"

@interface AppDelegate ()

@property (nonatomic) Server *server;

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    _server = [[Server alloc] init];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
	// Insert code here to tear down your application
}

@end
