//
//  NESApp.m
//  InfoNes iPhone
//
//  Created by Steve White on 8/04/07.
//  Copyright 2007 Steve White. All rights reserved.
//
#import "NESApp.h"
#import "MainView.h"

@implementation NESApp
- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
	_window = [[UIWindow alloc] initWithContentRect:
		 [UIHardware fullScreenApplicationContentRect]
	];

	struct CGRect rect = [UIHardware fullScreenApplicationContentRect];
	rect.origin.x = rect.origin.y = 0.0f;
	MainView *mainView = [[MainView alloc] initWithFrame: rect];
	[_window setContentView: mainView]; 

	[_window orderFront: self];
	[_window makeKey: self];
	[_window _setHidden: NO];
}

- (void)applicationWillTerminate {
	[_window release];
}
@end
