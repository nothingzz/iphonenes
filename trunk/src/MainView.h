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

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <UIKit/UITransitionView.h>
#import <UIKit/UIPreferencesTable.h>
#import <UIKit/UISegmentedControl.h>
#import <UIKit/UISwitchControl.h>
#import "FileBrowser.h"
#import "EmulationView.h"

char *fileName;

@interface MainView : UIView 
{
         UINavigationBar  *_navBar;
         UITransitionView *_transitionView;

         FileBrowser      *_browser;
         EmulationView    *_emuView;
         UIPreferencesTable *_pref;

         int       _currentView;
         CGRect    _rect;
         pthread_t emulation_tid;

         UISegmentedControl *frameControl;
         UISwitchControl *debugControl;
         UISwitchControl *delromsControl;
         UISwitchControl *autosaveControl;
         UISegmentedControl *spkControl;
         BOOL allowDeleteROMs;
         UIPreferencesTableCell *cell;
         struct NESApp_Preferences preferences;
}

- (id)initWithFrame:(CGRect)frame;
- (void)dealloc;
- (void)deviceOrientationChanged;
- (void)startEmulator;
- (void)stopEmulator;
- (void)setNavBar;
- (BOOL)isBrowsing;
- (UIPreferencesTable *)createPrefPane;
- (FileBrowser *)createBrowser;
- (EmulationView *)createEmulationView;
- (UINavigationBar *)createNavBar;
- (UITransitionView *)createTransitionView;
- (void)savePreferences;

 - (int)numberOfGroupsInPreferencesTable:(UIPreferencesTable *)aTable;
 - (int)preferencesTable:(UIPreferencesTable *)aTable numberOfRowsInGroup:(int)group;
 - (UIPreferencesTableCell *)preferencesTable:(UIPreferencesTable *)aTable cellForGroup:(int)group;
 - (UIPreferencesTableCell *)preferencesTable:(UIPreferencesTable *)aTable cellForRow:(int)row inGroup:(int)group;
 - (float)preferencesTable:(UIPreferencesTable *)aTable heightForRow:(int)row inGroup:(int)group withProposedHeight:(float)proposed;
 - (BOOL)preferencesTable:(UIPreferencesTable *)aTable isLabelGroup:(int)group;

#define CUR_BROWSER	0x00
#define CUR_PREFERENCES	0x01
#define CUR_EMULATOR	0x02

@end
