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

#import <CoreSurface/CoreSurface.h>
#import "ScreenView.h"

static ScreenView *sharedInstance = nil;
void updateScreen() {

	[sharedInstance performSelectorOnMainThread:@selector(updateScreen) withObject:nil waitUntilDone: NO];
}

@implementation ScreenView 
- (id)initWithFrame:(CGRect)frame {
	if ((self == [super initWithFrame:frame])!=nil) {
            sharedInstance = self;
            initGraphics = 0;
	}
	return self;
}

- (void)updateScreen {

	[sharedInstance setNeedsDisplay];
}

- (void)dealloc {
        [ screenLayer release ];
        pthread_mutex_destroy(&screenUpdateMutex);
        pthread_cond_destroy(&screenUpdateLock);
	[super dealloc];
}

- (void)drawRect:(CGRect)rect{

    int screenOrientation = [UIHardware deviceOrientation: YES];

    if (initGraphics == 0) {
        int i;
        CFMutableDictionaryRef dict;
        int w = 256;
        int h = 240;

        if (screenOrientation == 3) {
            w = 240;
            h = 256;
        }

        int pitch = w * 2, allocSize = 2 * w * h;
        char *pixelFormat = "565L";
        unsigned short *screen;

        initGraphics = 1;
        pthread_cond_init(&screenUpdateLock, NULL);
        pthread_mutex_init(&screenUpdateMutex, NULL);

        dict = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
            &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
        CFDictionarySetValue(dict, kCoreSurfaceBufferGlobal, kCFBooleanTrue);
        CFDictionarySetValue(dict, kCoreSurfaceBufferMemoryRegion,
            CFSTR("PurpleGFXMem"));
        CFDictionarySetValue(dict, kCoreSurfaceBufferPitch,
            CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &pitch));
        CFDictionarySetValue(dict, kCoreSurfaceBufferWidth,
            CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &w));
        CFDictionarySetValue(dict, kCoreSurfaceBufferHeight,
            CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &h));
        CFDictionarySetValue(dict, kCoreSurfaceBufferPixelFormat,
            CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, pixelFormat));
        CFDictionarySetValue(dict, kCoreSurfaceBufferAllocSize,
            CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &allocSize));

        screenSurface = CoreSurfaceBufferCreate(dict);
        CoreSurfaceBufferLock(screenSurface, 3);

        screenLayer = [[LKLayer layer] retain];
        if (screenOrientation == 3) 
            [screenLayer setFrame: CGRectMake(0.0f, 0.0f, 240.0f, 255.0f)];
        else
            [screenLayer setFrame: CGRectMake(0.0f, 0.0f, 256.0f, 239.0f)];

        [screenLayer setContents: screenSurface];
        [screenLayer setOpaque: YES];
//        [screenLayer setDelegate: self];
        [[self _layer] addSublayer: screenLayer];

        CoreSurfaceBufferUnlock(screenSurface);
    }

}

@end
