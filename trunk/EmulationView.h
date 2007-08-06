//
//  EmulationView.h
//  InfoNes iPhone
//
//  Created by Steve White on 8/04/07.
//  Copyright 2007 Steve White. All rights reserved.
//
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <pthread.h>

#import "ControllerView.h"
#import "ScreenView.h"

@interface EmulationView : UIView 
{
	ScreenView *_screenView;
	ControllerView *_controller;
	pthread_t emulation_tid;
}

- (id)initWithFrame:(CGRect)frame;
- (void)startEmulator;
- (void)stopEmulator;
- (BOOL)loadROM: (NSString *)path;
- (void)dealloc;
- (void)drawRect:(CGRect)frame;
@end
