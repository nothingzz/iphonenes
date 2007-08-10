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
#import "MainView.h"

extern unsigned long dwKeyPad1;

@implementation ControllerView
- (id)initWithFrame:(CGRect)frame {
	if ((self == [super initWithFrame:frame])!=nil) {

                int screenOrientation = [UIHardware deviceOrientation: YES];

                if (screenOrientation == 3) {
                    _controllerImage = [UIImage applicationImageNamed:@"controller_landscape.png"];
                    B = CGRectMake(83, 401, 59, 59);
                    A = CGRectMake(144, 401, 59, 59);
                    Up = CGRectMake(165, 26, 38, 46);
                    Down = CGRectMake(88, 28, 32, 42);
                    Left = CGRectMake(122, 0, 40, 40);
                    Right = CGRectMake(122, 58, 40, 42);
                    Select = CGRectMake(16, 8, 32, 40);
                    Start = CGRectMake(16, 52, 32, 40);

                    UpLeft = CGRectMake(164, 0, 38, 25); 
                    UpRight = CGRectMake(164, 73, 38, 24);
                    DownLeft = CGRectMake(89, 0, 32, 26);
                    DownRight = CGRectMake(89, 72, 32, 26);
                } else {
                   _controllerImage = [UIImage applicationImageNamed:@"controller_portrait.png"];
                   Up = CGRectMake(34, 0, 39, 33);
                   Down = CGRectMake(34, 65, 39, 33);
                   Left = CGRectMake(0, 31, 43, 38);
                   Right = CGRectMake(63, 31, 43, 38);
                   Select = CGRectMake(110, 60, 36, 20);
                   Start = CGRectMake(155, 60, 36, 20);
                   B = CGRectMake(197, 27, 56, 72);
                   A = CGRectMake(258, 27, 56, 72);

                   UpLeft = CGRectMake(0, 0, 34, 31);
                   UpRight = CGRectMake(73, 0, 34, 31);
                   DownLeft = CGRectMake(0, 69, 34, 31);
                   DownRight = CGRectMake(73, 69, 34, 31);
               }

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
        int screenOrientation = [UIHardware deviceOrientation: YES];

        if (screenOrientation == 3) 
            rect2.size.height = 460;

	[_controllerImage draw1PartImageInRect: rect2];

	
#ifdef DEBUG
	CGContextRef ctx = UICurrentContext();
	float red[4] = {1, 0, 0, 1};
	float green[4] = {0, 1, 0, 1};
	float blue[4] = {0, 0, 1, 1};
	CGContextSetFillColor(ctx, red);
	CGContextFillRect(ctx, UpLeft);
	CGContextFillRect(ctx, UpRight);
	CGContextFillRect(ctx, DownLeft);
	CGContextFillRect(ctx, DownRight);

	CGContextSetFillColor(ctx, blue);
	CGContextFillRect(ctx, Up);
	CGContextFillRect(ctx, Down);
	CGContextFillRect(ctx, Left);
	CGContextFillRect(ctx, Right);

	CGContextSetFillColor(ctx, green);
	CGContextFillRect(ctx, Select);
	CGContextFillRect(ctx, Start);
	CGContextFillRect(ctx, A);
	CGContextFillRect(ctx, B);
#endif

	[self fixRects];
}

- (BOOL)ignoresMouseEvents {

	return NO;
}

- (void)fixRects {
	if (_fixed)
		return;

	/* Because convertPoint:toView: was segfaulting on me... */

	MainView *mainView = [[[self superview] superview] superview];
	Up = [self convertRect: Up toView: mainView];
	Down = [self convertRect: Down toView: mainView];
	Left = [self convertRect: Left toView: mainView];
	Right = [self convertRect: Right toView: mainView];
	Select = [self convertRect: Select toView: mainView];
	Start = [self convertRect: Start toView: mainView];
	B = [self convertRect: B toView: mainView];
	A = [self convertRect: A toView: mainView];

	UpLeft = [self convertRect: UpLeft toView: mainView];
	UpRight = [self convertRect: UpRight toView: mainView];
	DownLeft = [self convertRect: DownLeft toView: mainView];
	DownRight = [self convertRect: DownRight toView: mainView];

	_fixed = YES;
}

- (int)controllerButtonPressed:(GSEvent *)event {
	CGRect rect = GSEventGetLocationInWindow(event);
	CGPoint point = rect.origin;
        int button = 0;

	if (CGRectContainsPoint(A, point)) {
		button |= BIT_00;
	}
	else if (CGRectContainsPoint(B, point)) {
		button |= BIT_01;
	}
	else if (CGRectContainsPoint(Up, point)) {
		button |= BIT_04;
	}
	else if (CGRectContainsPoint(Down, point)) {
		button |= BIT_05;
	}
	else if (CGRectContainsPoint(Left, point)) {
		button |= BIT_06;
	}
	else if (CGRectContainsPoint(Right, point)) {
		button |= BIT_07;
	}
	else if (CGRectContainsPoint(UpLeft, point)) {
		button |= BIT_04;
		button |= BIT_06;
	}
	else if (CGRectContainsPoint(UpRight, point)) {
		button |= BIT_04;
		button |= BIT_07;
	}
	else if (CGRectContainsPoint(DownLeft, point)) {
		button |= BIT_05;
		button |= BIT_06;
	}
	else if (CGRectContainsPoint(DownRight, point)) {
		button |= BIT_05;
		button |= BIT_07;
	}
	else if (CGRectContainsPoint(Select, point)) {
		button |= BIT_02;
	}
	else if (CGRectContainsPoint(Start, point)) {
		button |= BIT_03;
	}
	return button;
}

- (void)mouseDown:(GSEvent *)event {
	int button = [self controllerButtonPressed: event];
        if (button)
            dwKeyPad1 = button;

#ifdef DEBUG
        FILE *f = fopen("/tmp/NES.debug", "a");
        fprintf(f, "ControllerView.mouseDown(%d) dwKeyPad1:%d\n", button, dwKeyPad1);
        fclose(f);
#endif
}

- (void)mouseDragged:(GSEvent *)event {
        int button = [self controllerButtonPressed: event];
        if (!button)
            return;

        if (!(dwKeyPad1 & button)) {
            dwKeyPad1 |= button;
            if (button >= BIT_04 && button <= BIT_07)
            {
                // Reset directional pad
                if (button != BIT_04)
                    dwKeyPad1 &= ~BIT_04;
                if (button != BIT_05)
                    dwKeyPad1 &= ~BIT_05;
                if (button != BIT_06)
                    dwKeyPad1 &= ~BIT_06;
                if (button != BIT_07)
                    dwKeyPad1 &= ~BIT_07;
            } else if (button == BIT_00 || button == BIT_01) 
            {
                if (button != BIT_00)
                   dwKeyPad1 &= ~BIT_00;
                else
                   dwKeyPad1 &= ~BIT_01;
            }
        }

#ifdef DEBUG
        FILE *f = fopen("/tmp/NES.debug", "a");
        fprintf(f, "ControllerView.mouseDragged(%d) dwKeyPad1:%d\n", button, dwKeyPad1);
        fclose(f);
#endif
}
- (void)mouseUp:(GSEvent *)event {
        int button = [self controllerButtonPressed: event];

        if (!button)
            dwKeyPad1 = 0;
        else 
            if (dwKeyPad1 > 16 && dwKeyPad1 % 8 != 0)
                dwKeyPad1 = button;
            else
                dwKeyPad1 &= ~button;

#ifdef DEBUG
        FILE *f = fopen("/tmp/NES.debug", "a");
        fprintf(f, "ControllerView.mouseUp(%d) dwKeyPad1:%d\n", button, dwKeyPad1);
        fclose(f);
#endif
}

@end
