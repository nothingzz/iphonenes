//
//  InfoNES_iPhone.c
//  InfoNes iPhone
//
//  Created by Steve White on 8/05/07.
//  Copyright 2007 Steve White. All rights reserved.
//
#include "InfoNES/InfoNES.h"
#include <string.h>
#include <stdio.h>

// From ScreenView.h
extern void updateScreen();
extern unsigned char *screen;

unsigned long dwKeyPad1;
unsigned long dwKeyPad2;
unsigned long dwKeySystem;

WORD NesPalette[ 64 ] =
{
  0x39ce, 0x1071, 0x0015, 0x2013, 0x440e, 0x5402, 0x5000, 0x3c20,
  0x20a0, 0x0100, 0x0140, 0x00e2, 0x0ceb, 0x0000, 0x0000, 0x0000,
  0x5ef7, 0x01dd, 0x10fd, 0x401e, 0x5c17, 0x700b, 0x6ca0, 0x6521,
  0x45c0, 0x0240, 0x02a0, 0x0247, 0x0211, 0x0000, 0x0000, 0x0000,
  0x7fff, 0x1eff, 0x2e5f, 0x223f, 0x79ff, 0x7dd6, 0x7dcc, 0x7e67,
  0x7ae7, 0x4342, 0x2769, 0x2ff3, 0x03bb, 0x0000, 0x0000, 0x0000,
  0x7fff, 0x579f, 0x635f, 0x6b3f, 0x7f1f, 0x7f1b, 0x7ef6, 0x7f75,
  0x7f94, 0x73f4, 0x57d7, 0x5bf9, 0x4ffe, 0x0000, 0x0000, 0x0000
};

char emuThread;

int InfoNES_ReadRom( const char *pszFileName ) {
//printf("InfoNES_ReadROM\n");
	return 0;
}

void InfoNES_ReleaseRom() {
//printf("InfoNES_ReleaseRom\n");
}

void *InfoNES_MemoryCopy( void *dest, const void *src, int count ) {
//printf("InfoNES_MemoryCopy\n");
	memcpy( dest, src, count );
	return dest;
}

void *InfoNES_MemorySet(void *dest, int c, int count) {
//printf("InfoNES_MemorySet\n");
	memset(dest, c, count);
	return dest;
}

void InfoNES_LoadFrame() {
//printf("InfoNES_LoadFrame\n");
	unsigned char *c;
	c = screen;
	int x, y,i;
	for (y=240; y!=-1; y--) {
		i = 256 * y;
		for (x=0; x<256; x++) {
			*c++ = (unsigned char)WorkFrame[i+x];
		}
	}
	updateScreen();
}

void InfoNES_PadState( DWORD *pdwPad1, DWORD *pdwPad2, DWORD *pdwSystem ) {
//printf("InfoNES_PadState dwKeyPad1=%i, dwKeySystem=%i\n", dwKeyPad1, dwKeySystem);
	*pdwPad1 = dwKeyPad1;
	*pdwPad2 = 0;
	*pdwSystem = dwKeySystem;
}

void InfoNES_SoundInit( void ) {
//printf("InfoNES_SoundInit\n");
}

int InfoNES_SoundOpen( int samples_per_sync, int sample_rate ) {
//printf("InfoNES_SoundOpen\n");
	return 1;
}

void InfoNES_SoundClose( void ) {
//printf("InfoNES_SoundClose\n");
}

void InfoNES_SoundOutput( int samples, BYTE *wave1, BYTE *wave2, BYTE *wave3, BYTE *wave4, BYTE *wave5 ) {
//printf("InfoNES_SoundOuput\n");

}

void InfoNES_Wait() {
//printf("InfoNES_Wait\n");
}

void InfoNES_MessageBox( char *pszMsg, ... ) {
//printf("InfoNES_MessageBox\n");

}

int InfoNES_Menu() {
printf("InfoNES_Menu returning %i\n",emuThread);
	return emuThread;
}

void *emulation_thread(void *args) {
	dwKeyPad1 = 0;
	dwKeySystem = 0;
	InfoNES_Reset();
	InfoNES_Main();
	printf("emulation_thread exiting..\n");
}
