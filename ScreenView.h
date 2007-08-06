//
//  ScreenView.h
//  InfoNes iPhone
//
//  Created by Steve White on 8/05/07.
//  Copyright 2007 Steve White. All rights reserved.
//
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

unsigned char	*screen;
@interface ScreenView : UIView 
{
	NSLock		*mutex;
	CGDataProviderRef	provider;
	CGColorSpaceRef colorSpace;
	CGColorSpaceRef		colors;
	int currentColor;
}

- (id)initWithFrame:(CGRect)frame;
- (void)dealloc;
- (void)drawRect:(CGRect)frame;

- (void)updateScreen;

- (void)lock;
- (void)unlock;
@end
