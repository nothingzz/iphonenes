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

#import "EmulationView.h"
#include "InfoNES/InfoNES.h"
#include "InfoNES_iPhone.h"

extern unsigned long dwKeySystem;
extern char emuThread;

@implementation EmulationView
- (id)initWithFrame:(CGRect)frame {
    if ((self == [super initWithFrame:frame])!=nil) {
        _controller = [[ControllerView alloc] initWithFrame: CGRectMake(frame.origin.x, frame.size.height-119, frame.size.width, 119)];

        int width = 256;
        int height = 240;
        int xOffset = floor((frame.size.width-width)/2);
        int yOffset = floor((frame.size.height-119-height)/2);

        _screenView = [[ScreenView alloc] initWithFrame: CGRectMake(xOffset, yOffset, width, height)];

        [self addSubview: _controller];
        [self addSubview: _screenView];
    }
    return self;
}

- (void)dealloc {
	[super dealloc];
}

- (void)startEmulator {
	emuThread = 0;
	pthread_create(&emulation_tid, NULL, emulation_thread, NULL);
}

- (void)stopEmulator {
	dwKeySystem |= 1; /* PAD_SYS_QUIT */
	emuThread = -1;
	pthread_join(emulation_tid, NULL);
	dwKeySystem = 0;
}

- (BOOL)loadROM: (NSString *)path {
	FILE *fp;
	/* Open ROM file */
	fp = fopen([path cStringUsingEncoding: NSASCIIStringEncoding], "rb" );
	if (fp == NULL)
		return NO;

	/* Read ROM Header */
	fread(&NesHeader, sizeof(NesHeader), 1, fp);
	if (memcmp(NesHeader.byID, "NES\x1a", 4 ) != 0 ) {
		/* not .nes file */
		fclose( fp );
		return NO;
	}

	if (NesHeader.byInfo1 & 4) {
		fseek(fp, 528, SEEK_SET);
	}

	ROM = (unsigned char*)malloc(NesHeader.byRomSize * 0x4000);
	fread(ROM, 0x4000, NesHeader.byRomSize, fp);

	if (NesHeader.byVRomSize > 0) {
		VROM = (unsigned char *)malloc(NesHeader.byVRomSize * 0x2000);
		fread(VROM, 0x2000, NesHeader.byVRomSize, fp);
	}

	fclose(fp);
	return YES;
}

- (void)drawRect:(CGRect)rect{
	CGContextRef ctx = UICurrentContext();
	float black[4] = {0, 0, 0, 1};
	CGContextSetFillColor(ctx, black);
	CGContextFillRect(ctx, CGRectMake(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height-119)); 
}
@end
