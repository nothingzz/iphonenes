//
//  MainView.h
//  InfoNes iPhone
//
//  Created by Steve White on 8/04/07.
//  Copyright 2007 Steve White. All rights reserved.
//
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <UIKit/UITransitionView.h>

#import "FileBrowser.h"
#import "EmulationView.h"

@interface MainView : UIView 
{
	UINavigationBar *_navBar;
	UITransitionView *_transitionView;
	FileBrowser *_browser;
	EmulationView *_emuView;
	BOOL _browsing;
}

- (id)initWithFrame:(CGRect)frame;
- (void)dealloc;
@end
