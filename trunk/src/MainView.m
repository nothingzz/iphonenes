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
#import <UIKit/UIKit.h>
#import <UIKit/UISegmentedControl.h>
#import <UIKit/UISwitchControl.h>
#import <UIKit/UIPreferencesTableCell.h>

#include <errno.h>

int __screenOrientation;

extern unsigned long dwKeySystem;
extern char emuThread;
extern AudioDeviceID defaultOutputDevice, basebandDevice;
int audioIsSpeaker;

@implementation MainView 
- (id)initWithFrame:(struct CGRect)rect {
    if ((self == [super initWithFrame: rect]) != nil) {
        float offset = 0.0;
        _rect = rect;
        _currentView = CUR_BROWSER;

        NESApp_LoadPreferences(&preferences);
        FrameSkip = preferences.FrameSkip;
        audioIsSpeaker = preferences.defaultSound;
        IS_DEBUG = preferences.debug;
        if (preferences.canDeleteROMs) {
            [ _browser setAllowDeleteROMs: YES ];
            allowDeleteROMs = YES;
         } else {
            [ _browser setAllowDeleteROMs: NO ];
            allowDeleteROMs = NO;
         }

        __screenOrientation = [UIHardware deviceOrientation: YES];
        [UIHardware setSpeakerPhoneEnabled: YES];

        if (__screenOrientation != 3) {
            _navBar = [ self createNavBar ];
            [ self setNavBar ];
        }

        _transitionView = [ self createTransitionView ];
        _pref    = [ self createPrefPane ];
        _browser = [ self createBrowser ];
        _emuView = [ self createEmulationView ];

        if (__screenOrientation != 3) 
            [self addSubview: _navBar];

        [ self addSubview: _transitionView ];
        [ _transitionView transition:1 toView:_browser ];
    }
    return self;
}

- (void)dealloc {
        int screenOrientation = [UIHardware deviceOrientation: YES];

	[ _browser release ];
        [ _pref release ];
        [ _transitionView release ];
        if (screenOrientation == 3)
            [ _navBar release ];
	[ super dealloc ];
}

- (void)alertSheet:(UIAlertSheet *)sheet buttonClicked:(int)button {
	[ sheet dismiss ];
}

- (void)navigationBar:(UINavigationBar *)navbar buttonClicked:(int)button {
    int screenOrientation = [UIHardware deviceOrientation: YES];

    switch (button) {

        /* Left Navigation Button */

        case 1:
            switch (_currentView) {

                case CUR_PREFERENCES:

                    [ self savePreferences ];
                    _currentView = CUR_BROWSER;
                    [ _transitionView transition:2 toView:_browser ];
                    break;

                case CUR_BROWSER:
                    if ([ _browser getSaved ] == NO) 
                        [ _browser setSaved: YES];
                    else 
                        [_browser setSaved: NO];
                    [ _browser reloadData ];
                    break;

                case CUR_EMULATOR:
                    _currentView = CUR_BROWSER;
                    [ self stopEmulator ];
                    [ _transitionView transition:2 toView:_browser ];
                    break;
            }
            break;

        /* Right Navigation Button */

        case 0:
            switch (_currentView) {

                case CUR_BROWSER:
                    _currentView = CUR_PREFERENCES;
                    [ _transitionView transition:1 toView:_pref ];
                    break;

                case CUR_EMULATOR:
                    if (audioIsSpeaker != -1) 
                        InfoNES_SoundClose();
                    audioIsSpeaker--;
                    if (audioIsSpeaker < -1)
                        audioIsSpeaker = 1;
                    if (audioIsSpeaker != -1)
                        InfoNES_SoundOpen(0, 0);
                    break;
            }
            break;
    }

    [ self setNavBar ];
}

- (void)fileBrowser: (FileBrowser *)browser fileSelected:(NSString *)file {
    if ([_emuView loadROM: file]) {
        int screenOrientation = [UIHardware deviceOrientation: YES];
        fileName = malloc(strlen([file cStringUsingEncoding: 
                                       NSASCIIStringEncoding])+1);
        strcpy(fileName, [file cStringUsingEncoding: NSASCIIStringEncoding]);

        [_transitionView transition:1 toView:_emuView];
        _currentView = CUR_EMULATOR;
        [ self startEmulator ];
        [ self setNavBar ];

    } else {
        UIAlertSheet *sheet = [ [ UIAlertSheet alloc ] initWithFrame: 
            CGRectMake(0, 240, 320, 240) ];
        [ sheet setTitle:@"Invalid ROM Image" ];
        [ sheet setBodyText:[NSString stringWithFormat:@"%@ does not appear to be a valid ROM image.", file] ];
        [ sheet addButtonWithTitle:@"OK" ];
        [ sheet setDelegate: self ];
        [ sheet presentSheetFromAboveView: self ];
    }
}

- (void)deviceOrientationChanged {
    __screenOrientation = [UIHardware deviceOrientation: YES];

    [ _transitionView removeFromSuperview ];
    [ _transitionView release ];

    EmulationView *_newEmuView = [ self createEmulationView ];
    FileBrowser *_newBrowser = [ self createBrowser ];
    UIPreferencesTable *_newPref = [ self createPrefPane ];
    [ _newBrowser reloadData ];

    _transitionView = [ self createTransitionView ];
    [self addSubview: _transitionView];

    switch (_currentView) {
        case (CUR_EMULATOR):
            [_transitionView transition:6 fromView:_emuView toView:_newEmuView];
            break;
        case (CUR_BROWSER):
            [_transitionView transition:1 toView:_newBrowser];
            break;
        case (CUR_PREFERENCES):
            [_transitionView transition:1 toView:_newPref];
            break;
    }

    [_emuView release];
    _emuView = _newEmuView;

    [_browser release];
    _browser = _newBrowser;

    [_pref release];
    _pref = _newPref;

    if (__screenOrientation == 3) {
        [ _navBar removeFromSuperview ];
    } else {
        [ self addSubview: _navBar ];
        [ self setNavBar ];
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
        if (preferences.autoSave)
            SaveState();
}

- (void)setNavBar {
    int screenOrientation = [UIHardware deviceOrientation: YES];

    if (screenOrientation == 3)
        return;
    
    switch (_currentView) {
        case (CUR_PREFERENCES):
            [_navBar showButtonsWithLeftTitle:@"Back"
                 rightTitle:nil leftBack: YES
            ];
            break;

        case (CUR_BROWSER):
            if ([ _browser getSaved ] == NO) {
                [_navBar showButtonsWithLeftTitle:@"Show: Saved Games"
                         rightTitle:@"Preferences" leftBack: NO
                ];
            } else {
                [_navBar showButtonsWithLeftTitle:@"Show: All Games"
                         rightTitle:@"Preferences" leftBack: NO
                ];
            }
            break;

        case (CUR_EMULATOR):
            if (audioIsSpeaker == 0) {
                [_navBar showButtonsWithLeftTitle:@"ROM List"
                         rightTitle:@"Sound: Headset" leftBack: YES];
            } else if (audioIsSpeaker == 1) {
                [_navBar showButtonsWithLeftTitle:@"ROM List"
                         rightTitle:@"Sound: Speaker" leftBack: YES
                ];
            } else {
                [_navBar showButtonsWithLeftTitle:@"ROM List"
                         rightTitle:@"Sound: Off" leftBack: YES
                ];
            }
    }
}

- (FileBrowser *)createBrowser {
    float offset = 0.0;    

    if (__screenOrientation != 3) 
        offset = 48.0;

    FileBrowser *browser = [[FileBrowser alloc] initWithFrame:
        CGRectMake(0, 0, _rect.size.width, _rect.size.height - offset)
    ];

    [ browser setSaved: NO ];
    [ browser setPath:@"/var/root/Media/ROMs/NES/" ];
    [ browser setDelegate: self ];
    [ browser setAllowDeleteROMs: allowDeleteROMs ];

    return browser;
}

- (EmulationView *)createEmulationView {
    float offset = 0.0;
    if (__screenOrientation != 3)
        offset = 48.0;

    EmulationView *emuView = [[EmulationView alloc] initWithFrame:
        CGRectMake(0, 0, _rect.size.width, _rect.size.height - offset)
    ];

    return emuView;
}

- (UINavigationBar *)createNavBar {
    float offset = 48.0;
    UINavigationBar *navBar = [[UINavigationBar alloc] initWithFrame:
        CGRectMake(_rect.origin.x, _rect.origin.y, _rect.size.width, 48.0f)
    ];

    [navBar setDelegate: self];
    [navBar enableAnimation];
    return navBar;
}

- (UITransitionView *)createTransitionView {
    float offset = 0.0;
    if (__screenOrientation != 3)
        offset = 48.0;

    UITransitionView *transitionView = [[UITransitionView alloc] initWithFrame:
            CGRectMake(_rect.origin.x, offset, _rect.size.width,
                       _rect.size.height - offset)
    ];
    return transitionView;
}

- (BOOL)isBrowsing {
    if (_currentView == CUR_EMULATOR)
        return NO;
    return YES;
}

- (void)savePreferences {
    LOGDEBUG("savePreferences: _currentView %d", _currentView);

    if (_currentView != CUR_PREFERENCES)
        return;

    FrameSkip = [ frameControl selectedSegment ];
    preferences.FrameSkip = FrameSkip;

    audioIsSpeaker = [ spkControl selectedSegment ] - 1;
    preferences.defaultSound = audioIsSpeaker;

    long x;
    float y;
    x = [ debugControl value ];
    memcpy(&y, &x, sizeof(float));
    IS_DEBUG = y;
    preferences.debug = IS_DEBUG;

    x = [ delromsControl value ];
    memcpy(&y, &x, sizeof(float));
    allowDeleteROMs = y;
    if (allowDeleteROMs != 0.0) {
        [ _browser setAllowDeleteROMs: YES ];
        allowDeleteROMs = YES;
    } else {
        [ _browser setAllowDeleteROMs: NO ];
        allowDeleteROMs = NO;
    }
    preferences.canDeleteROMs = allowDeleteROMs;
 
    x = [ autosaveControl value ];
    memcpy(&y, &x, sizeof(float));  
    preferences.autoSave = y;

    NESApp_SavePreferences(&preferences);
}

- (UIPreferencesTable *)createPrefPane {
    float offset = 0.0;
    FILE *f;

    if (__screenOrientation != 3)
      offset = 48.0;

    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    float whiteComponents[4] = {1, 1, 1, 1};
    float transparentComponents[4] = {0, 0, 0, 0};

    UIPreferencesTable *pref = [[UIPreferencesTable alloc] initWithFrame:
      CGRectMake(0, 0, _rect.size.width, _rect.size.height - offset)];

    [ pref setDataSource: self ];
    [ pref setDelegate: self ];

    UITextLabel *versionText = [[UITextLabel alloc] initWithFrame:
      CGRectMake(15.0f, 380.0f, 100.0f, 20.0f)];
    [ versionText setText:@VERSION];
    [ versionText setBackgroundColor:
      CGColorCreate(colorSpace, transparentComponents)];
    [ pref addSubview:versionText ];
    [ pref reloadData ];
    return pref;
}

- (int)numberOfGroupsInPreferencesTable:(UIPreferencesTable *)aTable {
         return 2;
}

 - (int)preferencesTable:(UIPreferencesTable *)aTable 
    numberOfRowsInGroup:(int)group 
{
         if (group == 0) return 4;
         else if (group == 1) return 2;
}

- (UIPreferencesTableCell *)preferencesTable:(UIPreferencesTable *)aTable 
    cellForGroup:(int)group 
{
         UIPreferencesTableCell * cell = [[UIPreferencesTableCell alloc] init];
         return [cell autorelease];
} 

- (float)preferencesTable:(UIPreferencesTable *)aTable 
    heightForRow:(int)row 
    inGroup:(int)group 
    withProposedHeight:(float)proposed 
{
    if (group == 0) {
        switch (row) {
            case 0 :
                return 55;
                break;
        }
    } else if (group == 1) {
        switch (row) {
            case 1 :
                return 55;
                break;
        }
    }

    return proposed;
}

- (BOOL)preferencesTable:(UIPreferencesTable *)aTable 
    isLabelGroup:(int)group 
{
    return NO;
}

- (UIPreferencesTableCell *)preferencesTable:(UIPreferencesTable *)aTable 
    cellForRow:(int)row 
    inGroup:(int)group 
{
    UIPreferencesTableCell * cell = [[UIPreferencesTableCell alloc] init];
    [ cell setEnabled: YES ];
    if (group == 0) {
        switch (row) {
            case (0):
                [ cell setTitle:@"Frame Skip" ];
                frameControl = [[UISegmentedControl alloc] 
                  initWithFrame:CGRectMake(170.0f, 5.0f, 120.0f, 50.0f)];
                [ frameControl insertSegment:0 withTitle:@"0" animated: NO ];
                [ frameControl insertSegment:1 withTitle:@"1" animated: NO ];
                [ frameControl insertSegment:2 withTitle:@"2" animated: NO ];
                [ frameControl insertSegment:3 withTitle:@"3" animated: NO ];
                [ frameControl insertSegment:4 withTitle:@"4" animated: NO ];
                [ frameControl insertSegment:5 withTitle:@"5" animated: NO ];
                [ frameControl selectSegment: preferences.FrameSkip ];
                [ cell addSubview:frameControl ];
                break;
            case (1):
                [ cell setTitle:@"Debug" ];
                debugControl = [[UISwitchControl alloc] 
                    initWithFrame:CGRectMake(170.0f, 5.0f, 120.0f, 30.0f)];
                [ debugControl setValue: preferences.debug ];
                [ cell addSubview:debugControl ];
                break;
             case (2):
                [ cell setTitle:@"Auto-Save State" ];
                autosaveControl = [[UISwitchControl alloc]
                initWithFrame:CGRectMake(170.0f, 5.0f, 120.0f, 30.0f)];
                [ autosaveControl setValue: preferences.autoSave ];
                [ cell  addSubview:autosaveControl ];
                break;
            case (3):
                [ cell setTitle:@"Can Delete ROMs" ];
                delromsControl = [[UISwitchControl alloc]
                initWithFrame:CGRectMake(170.0f, 5.0f, 120.0f, 30.0f)];
                [ delromsControl setAlternateColors: YES ];
                [ delromsControl setValue: preferences.canDeleteROMs ];
                [ cell  addSubview:delromsControl ];
                break;
        }
    } else if (group == 1) {
        switch (row) {
            case (0):
                [ cell setTitle:@"Default Sound Output" ];
                [ cell setSeparatorStyle: 0 ];
                break;
            case (1):
                [ cell setSeparatorStyle: 0 ];
                spkControl = [[UISegmentedControl alloc]
                initWithFrame:CGRectMake(50.0f, 5.0f, 240.0f, 55.0f)];
                [ spkControl insertSegment:0 withTitle:@"None" animated:NO ];
                [ spkControl insertSegment:1 withTitle:@"Headset" animated:NO ];
                [ spkControl insertSegment:2 withTitle:@"Speaker" animated:NO ];
                [ spkControl selectSegment: preferences.defaultSound+1 ];
                [ cell addSubview:spkControl ];
                break;
        }
    }

    return [cell autorelease];  
}

@end
