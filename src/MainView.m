/*

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; version 2
 of the License.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#import "MainView.h"

@implementation MainView 
- (id)initWithFrame:(struct CGRect)rect {
	if ((self == [super initWithFrame: rect]) != nil) {
                float offset = 0.0;
#ifndef LANDSCAPE
                offset = 48.0;
		_navBar = [[UINavigationBar alloc] initWithFrame:
			CGRectMake(rect.origin.x, rect.origin.y, rect.size.width, 48.0f)
		];
		[_navBar setDelegate: self];
		[_navBar showButtonsWithLeftTitle:nil rightTitle:@"Refresh" leftBack: YES];
		[_navBar enableAnimation];
#endif

		_transitionView = [[UITransitionView alloc] initWithFrame: 
			CGRectMake(rect.origin.x, offset, rect.size.width, rect.size.height - offset)
		];

		_browser = [[FileBrowser alloc] initWithFrame:
			CGRectMake(0, 0, rect.size.width, rect.size.height - offset)
		];
		_emuView = [[EmulationView alloc] initWithFrame:
			CGRectMake(0, 0, rect.size.width, rect.size.height - offset)
		];

		[_browser setPath:@"/var/root/Media/ROMs/NES/"];
		[_browser setDelegate: self];

#ifndef LANDSCAPE
		[self addSubview: _navBar];
#endif
		[self addSubview: _transitionView];

		[_transitionView transition:1 toView:_browser];
		_browsing = YES;
	}
	return self;
}
- (void)dealloc {
	[_browser release];
#ifndef LANDSCAPE
	[_navBar release];
#endif
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
#ifndef LANDSCAPE
				[_navBar showButtonsWithLeftTitle:nil rightTitle:@"Refresh" leftBack: YES];
#endif
			}
			break;
	}
}

- (void)fileBrowser: (FileBrowser *)browser fileSelected:(NSString *)file {
	if ([_emuView loadROM: file]) {
		[_transitionView transition:1 toView:_emuView];
#ifndef LANDSCAPE
		[_navBar showButtonsWithLeftTitle:@"ROM List" rightTitle:@"Restart" leftBack: YES];
#endif
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
