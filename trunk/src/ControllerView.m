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

#import <GraphicsServices/GraphicsServices.h>
#import <UIKit/UIView-Geometry.h>
#import "ControllerView.h"
#import "ScreenView.h"

extern unsigned long dwKeyPad1;

@implementation ControllerView
- (id)initWithFrame:(CGRect)frame {
	if ((self == [super initWithFrame:frame])!=nil) {
		_controllerImage = [UIImage applicationImageNamed:@"controller.png"];
		Up = CGRectMake(37, 0, 36, 36);
		Down = CGRectMake(37, 60, 36, 36);
		Left = CGRectMake(5, 33, 31, 36);
		Right = CGRectMake(68, 33, 31, 36);
		Select = CGRectMake(110, 60, 36, 20);
		Start = CGRectMake(155, 60, 36, 20);
		B = CGRectMake(198, 21, 54, 54);
		A = CGRectMake(260, 21, 54, 54);
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

	/* Because convertPoint:toView: was segfaulting on me... */

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
        int button = 0;

	if (CGRectContainsPoint(A, point)) {
		button |= BIT_00;
	}
	if (CGRectContainsPoint(B, point)) {
		button |= BIT_01;
	}
	if (CGRectContainsPoint(Up, point)) {
		button |= BIT_04;
	}
	if (CGRectContainsPoint(Down, point)) {
		button |= BIT_05;
	}
	if (CGRectContainsPoint(Left, point)) {
		button |= BIT_06;
	}
	if (CGRectContainsPoint(Right, point)) {
		button |= BIT_07;
	}
	if (CGRectContainsPoint(Select, point)) {
		button |= BIT_02;
	}
	if (CGRectContainsPoint(Start, point)) {
		button |= BIT_03;
	}
	return button;
}

- (void)mouseDown:(GSEvent *)event {
	int button = [self controllerButtonPressed: event];
	dwKeyPad1 = button;
}

- (void)mouseDragged:(GSEvent *)event {
        int button = [self controllerButtonPressed: event];
        dwKeyPad1 = button;
}
- (void)mouseUp:(GSEvent *)event {
        int button = [self controllerButtonPressed: event];
        dwKeyPad1 &= ~button;
}

- (void)mouseMoved:(GSEvent *)event {
        int button = [self controllerButtonPressed: event];
        dwKeyPad1 = button;
}

@end
