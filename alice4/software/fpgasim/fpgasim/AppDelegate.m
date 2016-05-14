//
//  AppDelegate.m
//  fpgasim
//
//  Created by Lawrence Kesteloot on 5/12/16.
//  Copyright © 2016 Earwig, Inc. All rights reserved.
//

#import "AppDelegate.h"

@interface AppDelegate ()

@property (nonatomic) Server *server;

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    _server = [[Server alloc] initWithDelegate:self];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
	// Insert code here to tear down your application
}

// For ServerDelegate:
- (void)setWindowTitle:(NSString *)title {
    // This isn't right, we don't want all windows, but mainWindow is nil here.
    for (NSWindow *window in [NSApplication sharedApplication].windows) {
	window.title = title;
    }
}

@end
