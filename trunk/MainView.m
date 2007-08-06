//
//  MainView.m
//  InfoNes iPhone
//
//  Created by Steve White on 8/04/07.
//  Copyright 2007 Steve White. All rights reserved.
//
#import "MainView.h"

@implementation MainView 
- (id)initWithFrame:(struct CGRect)rect {
	if ((self == [super initWithFrame: rect]) != nil) {
		_navBar = [[UINavigationBar alloc] initWithFrame:
			CGRectMake(rect.origin.x, rect.origin.y, rect.size.width, 48.0f)
		];
		[_navBar setDelegate: self];
		[_navBar showButtonsWithLeftTitle:nil rightTitle:@"Refresh" leftBack: YES];
		[_navBar enableAnimation];

		_transitionView = [[UITransitionView alloc] initWithFrame: 
			CGRectMake(rect.origin.x, 48.0f, rect.size.width, rect.size.height - 48.0f)
		];

		_browser = [[FileBrowser alloc] initWithFrame:
			CGRectMake(0, 0, rect.size.width, rect.size.height - 48.0f)
		];
		_emuView = [[EmulationView alloc] initWithFrame:
			CGRectMake(0, 0, rect.size.width, rect.size.height - 48.0f)
		];

		[_browser setPath:@"/var/root/Media/ROMs/NES/"];
		[_browser setDelegate: self];

		[self addSubview: _navBar];
		[self addSubview: _transitionView];

		[_transitionView transition:1 toView:_browser];
		_browsing = YES;
	}
	return self;
}
- (void)dealloc {
	[_browser release];
	[_navBar release];
	[super dealloc];
}

- (void)alertSheet:(UIAlertSheet *)sheet buttonClicked:(int)button {
	[sheet dismiss];
}

- (void)navigationBar:(UINavigationBar *)navbar buttonClicked:(int)button {
	switch (button) {
		case 0:		// right
			if (_browsing) {	// Reload
				[_browser reloadData];
			} else {		// Restart Game
				[_emuView stopEmulator];
				[_emuView startEmulator];
			}
			break;
		case 1:		// left
			if (!_browsing) {	// ROM List
				[_emuView stopEmulator];
				[_transitionView transition:2 toView:_browser];
				[_navBar showButtonsWithLeftTitle:nil rightTitle:@"Refresh" leftBack: YES];
			}
			break;
	}
}

- (void)fileBrowser: (FileBrowser *)browser fileSelected:(NSString *)file {
	if ([_emuView loadROM: file]) {
		[_transitionView transition:1 toView:_emuView];
		[_navBar showButtonsWithLeftTitle:@"ROM List" rightTitle:@"Restart" leftBack: YES];
		_browsing = NO;
		[_emuView startEmulator];
		// [[file lastPathComponent] stringByDeletingPathExtension];
	} else {
		UIAlertSheet *sheet = [[UIAlertSheet alloc] initWithFrame: CGRectMake(0, 240, 320, 240)];
		[sheet setTitle:@"Invalid ROM Image"];
		[sheet setBodyText:[NSString stringWithFormat:@"%@ does not appear to be a valid ROM image.", file]];
		[sheet addButtonWithTitle:@"OK"];
		[sheet setDelegate: self];
		[sheet presentSheetFromAboveView: self];
	}
}
@end
