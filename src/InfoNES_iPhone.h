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

#import "InfoNES/InfoNES_Types.h"

#ifndef _INFONES_IPHONE_H
#define _INFONES_IPHONE_H

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

int LoadSRAM();
int SaveSRAM();

#ifndef DEBUG
#define LOGDEBUG( ... );
#else
void LOGDEBUG (const char *err, ... );
#endif

#endif
