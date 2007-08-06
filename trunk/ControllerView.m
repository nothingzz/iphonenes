//
//  ControllerView.m
//  InfoNes iPhone
//
//  Created by Steve White on 8/05/07.
//  Copyright 2007 Steve White. All rights reserved.
//
#import <GraphicsServices/GraphicsServices.h>
#import "ControllerView.h"
#import "ScreenView.h"

extern unsigned long dwKeyPad1;

@implementation ControllerView
- (id)initWithFrame:(CGRect)frame {
	if ((self == [super initWithFrame:frame])!=nil) {
		_controllerImage = [UIImage applicationImageNamed:@"controller.png"];

		Up = CGRectMake(37, 5, 32, 32);
		Down = CGRectMake(37, 65, 32, 32);
		Left = CGRectMake(5, 33, 32, 32);
		Right = CGRectMake(68, 33, 32, 32);
		Select = CGRectMake(110, 60, 36, 20);
		Start = CGRectMake(155, 60, 36, 20);
		B = CGRectMake(207, 30, 44, 44);
		A = CGRectMake(268, 30, 44, 44);
		_fixed = false;
	}
	return self;
}

- (void)dealloc {
	[_controllerImage release];
	[super dealloc];
}

- (void)drawRect:(CGRect)rect{
	CGRect rect2;
	rect2.origin.x = 0;
	rect2.origin.y = 0;
	rect2.size.width = 320;
	rect2.size.height = 119;
	[_controllerImage draw1PartImageInRect: rect2];

	[self fixRects];
}

- (BOOL)ignoresMouseEvents {
	return NO;
}

- (void)fixRects {
	if (_fixed)
		return;

	// Because convertPoint:toView: was segfaulting on me...
	UIView *mainView = [[[self superview] superview] superview];
	Up = [self convertRect: Up toView: mainView];
	Down = [self convertRect: Down toView: mainView];
	Left = [self convertRect: Left toView: mainView];
	Right = [self convertRect: Right toView: mainView];
	Select = [self convertRect: Select toView: mainView];
	Start = [self convertRect: Start toView: mainView];
	B = [self convertRect: B toView: mainView];
	A = [self convertRect: A toView: mainView];

	_fixed = YES;
}

- (int)controllerButtonPressed:(GSEvent *)event {
	CGRect rect = GSEventGetLocationInWindow(event);
	CGPoint point = rect.origin;

	if (CGRectContainsPoint(A, point)) {
		return (1 << 1);
	}
	else if (CGRectContainsPoint(B, point)) {
		return (1 << 0);
	}
	else if (CGRectContainsPoint(Up, point)) {
		return (1 << 4);
	}
	else if (CGRectContainsPoint(Down, point)) {
		return (1 << 5);
	}
	else if (CGRectContainsPoint(Left, point)) {
		return (1 << 6);
	}
	else if (CGRectContainsPoint(Right, point)) {
		return (1 << 7);
	}
	else if (CGRectContainsPoint(Select, point)) {
		return (1 << 2);
	}
	else if (CGRectContainsPoint(Start, point)) {
		return (1 << 3);
	}
	return 0;
}

- (void)mouseDown:(GSEvent *)event {
	int button = [self controllerButtonPressed: event];
	dwKeyPad1 |= button;
}

- (void)mouseDragged:(GSEvent *)event {
	//NSLog(@"mouseDragged:");
}
- (void)mouseUp:(GSEvent *)event {
	int button = [self controllerButtonPressed: event];
	dwKeyPad1 &= ~button;
}
@end
