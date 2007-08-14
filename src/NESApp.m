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

#import "NESApp.h"
#import "MainView.h"

@implementation NESApp
- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
        _mainView = NULL;
        _screenOrientation = 0;
	_window = [[UIWindow alloc] initWithContentRect:
		 [UIHardware fullScreenApplicationContentRect]
	];

	struct CGRect rect = [UIHardware fullScreenApplicationContentRect];
	rect.origin.x = rect.origin.y = 0.0f;

	_mainView = [[MainView alloc] initWithFrame: rect];

	[_window setContentView: _mainView]; 
	[_window orderFront: self];
	[_window makeKey: self];
	[_window _setHidden: NO];
}

- (void)applicationWillTerminate {
        [ _mainView savePreferences ];
        [ _mainView stopEmulator ];
	[_window release];
}

- (void)deviceOrientationChanged:(GSEvent *)event {
    int screenOrientation = [UIHardware deviceOrientation: YES];

    if ( (screenOrientation == 1 || screenOrientation == 3) &&
         _screenOrientation != screenOrientation) 
    {
        _screenOrientation = screenOrientation; 

        [ self setStatusBarMode: ((screenOrientation == 3 && _mainView && [ _mainView isBrowsing ] != YES) ? 1 : 0) duration: 0 ];

        if (screenOrientation)
             [_mainView deviceOrientationChanged];
    }
} 
@end
