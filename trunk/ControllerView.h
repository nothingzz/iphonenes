//
//  ControllerView.h
//  InfoNes iPhone
//
//  Created by Steve White on 8/05/07.
//  Copyright 2007 Steve White. All rights reserved.
//
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

@interface ControllerView : UIView 
{
	UIImage *_controllerImage;
	CGRect Up;
	CGRect Left;
	CGRect Down;
	CGRect Right;
	CGRect Select;
	CGRect Start;
	CGRect B;
	CGRect A;

	BOOL _fixed;
}

- (id)initWithFrame:(CGRect)frame;
- (void)dealloc;
- (void)drawRect:(CGRect)frame;
- (BOOL)ignoresMouseEvents;
- (void)fixRects;
- (int)controllerButtonPressed:(struct __GSEvent *)event;
- (void)mouseDown:(struct __GSEvent *)event;
- (void)mouseDragged:(struct __GSEvent *)event;
- (void)mouseUp:(struct __GSEvent *)event;

@end
