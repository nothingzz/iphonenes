//
//  NESApp.h
//  InfoNes iPhone
//
//  Created by Steve White on 8/04/07.
//  Copyright 2007 Steve White. All rights reserved.
//
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

@interface NESApp : UIApplication
{
    UIWindow *_window;
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification;
- (void)applicationWillTerminate;

@end
