//
//  InfoNES_iPhone.h
//  InfoNes iPhone
//
//  Created by Steve White on 8/05/07.
//  Copyright 2007 Steve White. All rights reserved.
//
int InfoNES_ReadRom( const char *pszFileName );
void InfoNES_ReleaseRom();
void *InfoNES_MemorySet(void *dest, int c, int count);
void InfoNES_LoadFrame();
void InfoNES_PadState( DWORD *pdwPad1, DWORD *pdwPad2, DWORD *pdwSystem );
void InfoNES_SoundInit( void );
int InfoNES_SoundOpen( int samples_per_sync, int sample_rate );
void InfoNES_SoundClose( void );
void InfoNES_SoundOutput( int samples, BYTE *wave1, BYTE *wave2, BYTE *wave3, BYTE *wave4, BYTE *wave5 );
void InfoNES_Wait();
void InfoNES_MessageBox( char *pszMsg, ... );
int InfoNES_Menu();
void *emulation_thread(void *args);
