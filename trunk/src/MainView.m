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
#import "CoreAudio.h"
#include "InfoNES/InfoNES.h"
#include "InfoNES_iPhone.h"

int __screenOrientation;

extern unsigned long dwKeySystem;
extern char emuThread;
extern AudioDeviceID defaultOutputDevice, basebandDevice;
int audioIsSpeaker;

@implementation MainView 
- (id)initWithFrame:(struct CGRect)rect {
    if ((self == [super initWithFrame: rect]) != nil) {
        float offset = 0.0;
        int screenOrientation = [UIHardware deviceOrientation: YES];
        audioIsSpeaker = 1;

        [UIHardware setSpeakerPhoneEnabled: YES];
          
        if (screenOrientation != 3) {
            offset = 48.0;
            _navBar = [[UINavigationBar alloc] initWithFrame:
                CGRectMake(rect.origin.x, rect.origin.y, rect.size.width, 48.0f)
            ];

            [_navBar setDelegate: self];
            [_navBar showButtonsWithLeftTitle:nil rightTitle:
                @"All Games" leftBack: YES];
            [_navBar enableAnimation];
        }

        _transitionView = [[UITransitionView alloc] initWithFrame: 
            CGRectMake(rect.origin.x, offset, rect.size.width, 
                       rect.size.height - offset)
        ];

        _browser = [[FileBrowser alloc] initWithFrame:
            CGRectMake(0, 0, rect.size.width, rect.size.height - offset)
        ];
        [ _browser setSaved: NO];
        [_browser setPath:@"/var/root/Media/ROMs/NES/"];
        [_browser setDelegate: self];

        _emuView = [[EmulationView alloc] initWithFrame:
            CGRectMake(0, 0, rect.size.width, rect.size.height - offset)
	];

        if (screenOrientation != 3) 

        [self addSubview: _navBar];
        [self addSubview: _transitionView];
        [_transitionView transition:1 toView:_browser];
        _browsing = YES;
        _rect = rect;
    }
    return self;
}

- (void)dealloc {
        int screenOrientation = [UIHardware deviceOrientation: YES];

	[_browser release];
        if (screenOrientation == 3)
            [_navBar release];
	[super dealloc];
}

- (void)alertSheet:(UIAlertSheet *)sheet buttonClicked:(int)button {
	[sheet dismiss];
}

- (void)navigationBar:(UINavigationBar *)navbar buttonClicked:(int)button {
    int screenOrientation = [UIHardware deviceOrientation: YES];

    switch (button) {

        /* Right Nav Button */

        case 0: 
        if (_browsing) {
            if ([ _browser getSaved ] == NO) {
                [ _browser setSaved: YES];
                [_navBar showButtonsWithLeftTitle:nil 
                         rightTitle:@"Saved Games" leftBack: YES
                ];
            } else {
                [_browser setSaved: NO];
                [_navBar showButtonsWithLeftTitle:nil 
                         rightTitle:@"All Games" leftBack: YES
                ];
            }
            [_browser reloadData];

        } else {
            InfoNES_SoundClose();
            if (!audioIsSpeaker) {
                [UIHardware setSpeakerPhoneEnabled: YES];
                audioIsSpeaker = 1;
                if (screenOrientation != 3)
                    [_navBar showButtonsWithLeftTitle:@"ROM List" 
                             rightTitle:@"Speaker" leftBack: YES];
            } else {
                [UIHardware setSpeakerPhoneEnabled: NO];
                audioIsSpeaker = 0;
                if (screenOrientation != 3)
                    [_navBar showButtonsWithLeftTitle:@"ROM List" 
                             rightTitle:@"Headset" leftBack: YES
                    ];
            }
            InfoNES_SoundOpen(0, 0);
        }
        break;

        /* Left Nav Button */

        case 1:	
        if (!_browsing) {
            _browsing = YES;
            [self stopEmulator];
            [_transitionView transition:2 toView:_browser];
            if (screenOrientation != 3) {
                if ([ _browser getSaved ] == NO) {
                    [_navBar showButtonsWithLeftTitle:nil 
                             rightTitle:@"All Games" leftBack: YES
                    ];
                } else {
                    [_navBar showButtonsWithLeftTitle:nil 
                             rightTitle:@"Saved Games" leftBack: YES
                    ];
                }

            }
        }
        break;

    }
}

- (void)fileBrowser: (FileBrowser *)browser fileSelected:(NSString *)file {
    if ([_emuView loadROM: file]) {
        int screenOrientation = [UIHardware deviceOrientation: YES];
        fileName = malloc(strlen([file cStringUsingEncoding: 
                                       NSASCIIStringEncoding])+1);
        strcpy(fileName, [file cStringUsingEncoding: NSASCIIStringEncoding]);

        [_transitionView transition:1 toView:_emuView];
        if (screenOrientation != 3) {
            if (!audioIsSpeaker) {
                [_navBar showButtonsWithLeftTitle:@"ROM List" 
                         rightTitle:@"Headset" leftBack: YES];
            } else {
                [_navBar showButtonsWithLeftTitle:@"ROM List" 
                         rightTitle:@"Speaker" leftBack: YES];
            }
        }
        _browsing = NO;
        [self startEmulator];

    } else {
        UIAlertSheet *sheet = [[UIAlertSheet alloc] initWithFrame: CGRectMake(0, 240, 320, 240)];
        [sheet setTitle:@"Invalid ROM Image"];
        [sheet setBodyText:[NSString stringWithFormat:@"%@ does not appear to be a valid ROM image.", file]];
        [sheet addButtonWithTitle:@"OK"];
        [sheet setDelegate: self];
        [sheet presentSheetFromAboveView: self];
    }
}

- (void)deviceOrientationChanged {
    float offset = 0.0;
    int screenOrientation = [UIHardware deviceOrientation: YES];

    __screenOrientation = screenOrientation;
    if (screenOrientation != 3) 
        offset = 48.0;

    [_transitionView removeFromSuperview];
    [_transitionView release];

    EmulationView *_newEmuView = [[EmulationView alloc] initWithFrame:
            CGRectMake(0, 0, _rect.size.width, _rect.size.height - offset)
    ];

    FileBrowser *_newBrowser = [[FileBrowser alloc] initWithFrame:
            CGRectMake(0, 0, _rect.size.width, _rect.size.height - offset)
    ];
    [_newBrowser setPath:@"/var/root/Media/ROMs/NES/"];
    [_newBrowser setDelegate: self];

    _transitionView = [[UITransitionView alloc] initWithFrame:
            CGRectMake(_rect.origin.x, offset, 
                       _rect.size.width, _rect.size.height - offset)
    ];

    [self addSubview: _transitionView];
    if (!_browsing) {
        [_transitionView transition:6 fromView: _emuView toView:_newEmuView];
    } else {
        [_transitionView transition:1 toView:_newBrowser];
    }

    [_emuView release];
    _emuView = _newEmuView;

    [_browser release];
    _browser = _newBrowser;

    if (screenOrientation == 3) {
        [ _navBar removeFromSuperview ];
    } else {
        [ self addSubview: _navBar ];

        if (!_browsing) {
            if (!audioIsSpeaker) {
                [_navBar showButtonsWithLeftTitle:@"ROM List" 
                         rightTitle:@"Headset" leftBack: YES];
            } else {
                [_navBar showButtonsWithLeftTitle:@"ROM List" 
                         rightTitle:@"Speaker" leftBack: YES];
            }
        } else {
            if ([ _browser getSaved ] == NO) {
                [_navBar showButtonsWithLeftTitle:nil 
                         rightTitle:@"All Games" leftBack: YES];
            } else {
                [_navBar showButtonsWithLeftTitle:nil 
                         rightTitle:@"Saved Games" leftBack: YES];
            }
        }
    }
}

- (void)startEmulator {
        emuThread = 0;
        pthread_create(&emulation_tid, NULL, emulation_thread, NULL);
}

- (void)stopEmulator {
        dwKeySystem |= 1;
        emuThread = -1;
        pthread_join(emulation_tid, NULL);
        dwKeySystem = 0;
        SaveSRAM();
        SaveState();
}

- (int)isBrowsing {
	return _browsing;
}

@end
