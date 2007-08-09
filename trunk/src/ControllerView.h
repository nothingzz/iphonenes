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

#define BIT_00  0x01
#define BIT_01  0x02
#define BIT_02  0x04
#define BIT_03  0x08
#define BIT_04  0x10
#define BIT_05  0x20
#define BIT_06  0x40
#define BIT_07  0x80

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

	CGRect UpLeft;
	CGRect UpRight;
	CGRect DownLeft;
	CGRect DownRight;
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
