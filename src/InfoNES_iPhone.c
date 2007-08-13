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
#include "InfoNES/InfoNES_Types.h"
#include "InfoNES/InfoNES_Mapper.h"
#include "InfoNES/InfoNES.h"
#include "CoreAudio.h"
#include "InfoNES_iPhone.h"
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/select.h>

extern void updateScreen();
extern CoreSurfaceBufferRef screenSurface;
extern int __screenOrientation;
extern int audioIsSpeaker;
extern BYTE ROM_Mirroring;

extern BYTE PPU_MirrorTable[6][ 4 ];

extern pthread_cond_t screenUpdateLock;
extern pthread_mutex_t screenUpdateMutex;
extern char *fileName;

AudioDeviceID defaultInputDevice, defaultOutputDevice, basebandDevice;
AudioStreamBasicDescription deviceFormat;

unsigned long dwKeyPad1;
unsigned long dwKeyPad2;
unsigned long dwKeySystem;

#define WAVE_BUFFER_SIZE 20480
float waveBuffer[WAVE_BUFFER_SIZE];
int writePtr;
int playPtr;

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

/* BEGIN :Externs for Save/Load State */

int nStep;
extern WORD PC;
extern BYTE SP;
extern BYTE F;
extern BYTE A;
extern BYTE X;
extern BYTE Y;
extern BYTE IRQ_State;
extern BYTE NMI_State;
extern WORD g_wPassedClocks;
extern BYTE g_byTestTable[ 256 ];
extern BYTE PPU_R0;
extern BYTE PPU_R1;
extern BYTE PPU_R2;
extern BYTE PPU_R3;
extern BYTE PPU_R7;
extern WORD PPU_Addr;
extern WORD PPU_Temp;
extern WORD PPU_Increment;
extern WORD PPU_Scanline;
extern BYTE PPU_ScanTable[ 263 ];
extern BYTE PPU_NameTableBank;
extern BYTE ChrBuf[ 256 * 2 * 8 * 8 ];
extern BYTE ChrBufUpdate;
extern WORD PalTable[ 32 ];

struct value_table_tag
{
  BYTE byValue;
  BYTE byFlag;
};

extern struct value_table_tag g_ASLTable[ 256 ];
extern struct value_table_tag g_LSRTable[ 256 ];
extern struct value_table_tag g_ROLTable[ 2 ][ 256 ];
extern struct value_table_tag g_RORTable[ 2 ][ 256 ];
extern BYTE ApuCtrl;
extern BYTE ApuCtrlNew;
extern WORD PPU_SP_Height;
extern int SpriteJustHit;
extern BYTE byVramWriteEnable;
extern BYTE PPU_Latch_Flag;
extern BYTE PPU_UpDown_Clip;
extern BYTE FrameIRQ_Enable;
extern WORD FrameStep;
extern BYTE APU_Reg[ 0x18 ];
extern BYTE *pbyPrevBank[ 8 ];
extern int pbyPrevBanks[ 8 ];


/* Mapper 1 Globals */
extern BYTE  Map1_Regs[ 4 ];
extern DWORD Map1_Cnt;
extern BYTE  Map1_Latch;
extern WORD  Map1_Last_Write_Addr;
extern BYTE Map1_Size;
extern DWORD Map1_256K_base;
extern DWORD Map1_swap;
extern DWORD Map1_bank1;
extern DWORD Map1_bank2;
extern DWORD Map1_bank3;
extern DWORD Map1_bank4;
extern DWORD Map1_HI1;
extern DWORD Map1_HI2;

/* Mapper 4 Globals */
extern BYTE  Map4_Regs[ 8 ];
extern DWORD Map4_Rom_Bank;
extern DWORD Map4_Prg0, Map4_Prg1;
extern DWORD Map4_Chr01, Map4_Chr23;
extern DWORD Map4_Chr4, Map4_Chr5, Map4_Chr6, Map4_Chr7;
extern BYTE Map4_IRQ_Enable;
extern BYTE Map4_IRQ_Cnt;
extern BYTE Map4_IRQ_Latch;
extern BYTE Map4_IRQ_Request;
extern BYTE Map4_IRQ_Present;
extern BYTE Map4_IRQ_Present_Vbl;

/* Mapper 5 Globals */
extern BYTE Map5_Wram[ 0x2000 * 8 ];
extern BYTE Map5_Ex_Ram[ 0x400 ];
extern BYTE Map5_Ex_Vram[ 0x400 ];
extern BYTE Map5_Ex_Nam[ 0x400 ];
extern BYTE Map5_Prg_Reg[ 8 ];
extern BYTE Map5_Wram_Reg[ 8 ];
extern BYTE Map5_Chr_Reg[ 8 ][ 2 ];
extern BYTE Map5_IRQ_Enable;
extern BYTE Map5_IRQ_Status;
extern BYTE Map5_IRQ_Line;
extern DWORD Map5_Value0;
extern DWORD Map5_Value1;
extern BYTE Map5_Wram_Protect0;
extern BYTE Map5_Wram_Protect1;
extern BYTE Map5_Prg_Size;
extern BYTE Map5_Chr_Size;
extern BYTE Map5_Gfx_Mode;

/* Mapper 6 Globals */
extern BYTE Map6_IRQ_Enable;
extern DWORD Map6_IRQ_Cnt;
extern BYTE Map6_Chr_Ram[ 0x2000 * 4 ];

/* Mapper 9 Globals */
struct Map9_Latch
{
  BYTE lo_bank;
  BYTE hi_bank;
  BYTE state;
};
extern struct Map9_Latch latch1;
extern struct Map9_Latch latch2;

/* Mapper 10 Globals */
struct Map10_Latch
{
  BYTE lo_bank;
  BYTE hi_bank;
  BYTE state;
};
extern struct Map10_Latch latch3; 
extern struct Map10_Latch latch4; 

/* Mapper 16 Globals */
extern BYTE  Map16_Regs[3];
extern BYTE  Map16_IRQ_Enable;
extern DWORD Map16_IRQ_Cnt;
extern DWORD Map16_IRQ_Latch;

/* Mapper 17 Globals */
extern BYTE  Map17_IRQ_Enable;
extern DWORD Map17_IRQ_Cnt;
extern DWORD Map17_IRQ_Latch;

/* END: Exnerns for Save/Load State */

int InfoNES_ReadRom( const char *pszFileName ) {

	return 0;
}

void InfoNES_ReleaseRom() {

}

void *InfoNES_MemoryCopy( void *dest, const void *src, int count ) {

	memcpy( dest, src, count );
	return dest;
}

void *InfoNES_MemorySet(void *dest, int c, int count) {

	memset(dest, c, count);
	return dest;
}

void InfoNES_LoadFrame() {
    WORD *c;
    int x, y, i = 0;

    c = CoreSurfaceBufferGetBaseAddress(screenSurface);
    register WORD wColor;

    pthread_mutex_lock(&screenUpdateMutex);

    if (__screenOrientation == 3) {

        for (y=0; y < 240; y++)
        {
            for (x=0; x<256; x++) {
                wColor = WorkFrame[ ( y << 8 ) + x ];
                wColor = ((wColor & 0x7fe0)<<1) | (wColor&0x001f);
                c[((x+1) * 240) - (y+1)] = wColor;
            }
        }
    } else {

        for (y=0; y < 240; y++)
        {
            for (x=0; x<256; x++) {
                wColor = WorkFrame[ ( y << 8 ) + x ];
                wColor = ((wColor & 0x7fe0)<<1) | (wColor&0x001f);
                c[i++] = wColor;
            }
        }
    }
    pthread_mutex_unlock(&screenUpdateMutex);
    updateScreen();
}

void InfoNES_PadState( DWORD *pdwPad1, DWORD *pdwPad2, DWORD *pdwSystem ) {
	*pdwPad1 = dwKeyPad1;
	*pdwPad2 = 0;
	*pdwSystem = dwKeySystem;
}

static OSStatus AudioOutputProc(
    AudioDeviceID inDevice,
    const AudioTimeStamp* inNow,
    const AudioBufferList* inInputData,
    const AudioTimeStamp* inInputTime,
    AudioBufferList* outOutputData,
    const AudioTimeStamp* inOutputTime,
    void* inClientData)
{
    int i;
    float volume = 127.0;
    AudioBuffer *outputBuffer = &outOutputData->mBuffers[0];
    unsigned long frameCount = outputBuffer->mDataByteSize
                            / (outputBuffer->mNumberChannels * sizeof(short));
    short *coreAudioBuffer = (short *) outputBuffer->mData;

    if (!audioIsSpeaker)
        volume = 255;

    if (writePtr > playPtr+frameCount || writePtr < playPtr) {
        short sample;
        for(i=0;i<frameCount*2;i+=2) {
            sample = (int)(waveBuffer[playPtr] * volume);
            coreAudioBuffer[i] =   sample;
            coreAudioBuffer[i+1] = sample;
            playPtr++;
        }
        if (playPtr == WAVE_BUFFER_SIZE) 
            playPtr = 0;
    }
    return noErr;
}

AudioDeviceID getBasebandDevice() {
    UInt32 propsize = 0;
    int i = 0; 
    char null[4096];
    char *outName;

    AudioDeviceID mydevice;

    AudioHardwareGetPropertyInfo(kAudioHardwarePropertyDevices, &propsize, 0);
    int numDevices = propsize / sizeof(AudioDeviceID);
    AudioDeviceID *deviceIDs = (AudioDeviceID*) malloc(sizeof(AudioDeviceID) * numDevices);
    AudioHardwareGetProperty(kAudioHardwarePropertyDevices, &propsize, deviceIDs);

    for(i=0;i<numDevices;i++) {
        AudioDeviceGetPropertyInfo(deviceIDs[i], 0, 0, kAudioDevicePropertyDeviceName, &propsize, null);
        outName = (char*)malloc(propsize);
        AudioDeviceGetProperty(deviceIDs[i], 0, 0, kAudioDevicePropertyDeviceName, &propsize, outName);
        if (!strcmp(outName, "Baseband Output")) {
              
             LOGDEBUG("InfoNES_SoundInit.getBasebandDevice: Found baseband output");
             mydevice = deviceIDs[i];
        }
        free(outName);
    }
  
    return mydevice;
} 

void InfoNES_SoundInit( void ) {
    UInt32 propsize = 0;
    int isInput = 0, count;
    double sampleRate = 44100.0, actualSampleRate;
    writePtr = 0;
    playPtr = 0;

    audioIsSpeaker = 1;

    propsize = sizeof(AudioDeviceID);
    AudioHardwareGetProperty(kAudioHardwarePropertyDefaultInputDevice,
        &propsize, &defaultInputDevice);
    AudioHardwareGetProperty(kAudioHardwarePropertyDefaultOutputDevice,
        &propsize, &defaultOutputDevice);

    basebandDevice = getBasebandDevice();

/*
    AudioDeviceSetProperty(defaultOutputDevice, 0, 0, isInput,
        kAudioDevicePropertyNominalSampleRate, propsize, &sampleRate);
    AudioDeviceSetProperty(basebandDevice, 0, 0, isInput,
        kAudioDevicePropertyNominalSampleRate, propsize, &sampleRate);
*/

}


int InfoNES_SoundOpen(int samples_per_sync, int sample_rate) {
    OSErr err;

    err = AudioDeviceAddIOProc((audioIsSpeaker) ? basebandDevice : defaultOutputDevice, AudioOutputProc, 0);
    LOGDEBUG("InfoNES_SoundOpen.AudioDeviceAddIOProc(%d) returned %d", audioIsSpeaker, err);
    if (err) {
      return 1;
    } 

    err = AudioDeviceStart((audioIsSpeaker) ? basebandDevice : defaultOutputDevice, AudioOutputProc);
    LOGDEBUG("InfoNES_SoundOpen.AudioDeviceStart(%d) returned %d", audioIsSpeaker, err);
    if (err) {
      return 1;
    }

    return 0;
}

void InfoNES_SoundClose(void) {

    AudioDeviceStop((audioIsSpeaker) ? basebandDevice : defaultOutputDevice, AudioOutputProc);
    AudioDeviceRemoveIOProc((audioIsSpeaker) ? basebandDevice : defaultOutputDevice, AudioOutputProc);
}

void InfoNES_SoundOutput(
  int samples,
  BYTE *wave1,
  BYTE *wave2,
  BYTE *wave3,
  BYTE *wave4,
  BYTE *wave5 ) 
{
  int i;
  for (i = 0; i < samples; i++)
  {
    waveBuffer[writePtr] = 
        (wave1[i] + wave2[i] + wave3[i] + wave4[i] + wave5[i]) / 5; 
    writePtr++;
    if (writePtr == WAVE_BUFFER_SIZE) 
        writePtr = 0;
  }
}

void InfoNES_Wait() {

}

void InfoNES_MessageBox( char *pszMsg, ... ) {

}

int InfoNES_Menu() {

	return emuThread;
}

void *emulation_thread(void *args) {
	dwKeyPad1 = 0;
	dwKeySystem = 0;
        InfoNES_Reset();
        int nBank, nIdx;
        LoadSRAM();
        if (!strcmp(fileName + (strlen(fileName)-4), ".sav"))
            LoadState(); 
	InfoNES_Main();
}

int SaveSRAM()
{
  FILE *fp;
  char *fn;
  
  if (!ROM_SRAM) return -1;
  fn = strdup(fileName);
  strcpy(fn + (strlen(fn)-3), "srm");
  LOGDEBUG("SaveSRAM: Saving SRAM to %s (ROM_SRAM %d)", fn, ROM_SRAM);

  fp = fopen(fn, "wb");
  if ( fp == NULL ) {
    free(fn);
    return -1;
  }

  fwrite(SRAM, SRAM_SIZE, 1, fp);

  fclose(fp);
  free(fn);
  return 0;
}

int LoadSRAM()
{
  FILE *fp;
  char *fn;

  if (!ROM_SRAM) return -1;
  fn = strdup(fileName);
  strcpy(fn + (strlen(fn)-3), "srm");
  LOGDEBUG("LoadSRAM: Loading SRAM from %s (ROM_SRAM %d)", fn, ROM_SRAM);

  fp = fopen(fn, "rb");
  if (fp == NULL) {
    free(fn);
    return -1;
  }

  fread(SRAM, SRAM_SIZE, 1, fp);

  fclose(fp);
  free(fn);
  return 0;
}

int SaveState()
{
  FILE *fp;
  char *fn = strdup(fileName);
  
  strcpy(fn + (strlen(fn)-3), "sav");
  LOGDEBUG("SaveState: Saving State to %s", fn);

  fp = fopen(fn, "wb");
  if (fp == NULL) {
    free(fn);
    return -1;
  }

  /* RAM State */

  fwrite(RAM, RAM_SIZE, 1, fp);
//  fwrite(DRAM, DRAM_SIZE, 1, fp);

  /* 65C02 State */

  fwrite(&PC, sizeof(PC), 1, fp);
  fwrite(&SP, sizeof(SP), 1, fp);
  fwrite(&A, sizeof(A), 1, fp);
  fwrite(&X, sizeof(X), 1, fp);
  fwrite(&Y, sizeof(Y), 1, fp);
  fwrite(&F, sizeof(F), 1, fp);

  fwrite(&NMI_State, sizeof(NMI_State), 1, fp);
  fwrite(&IRQ_State, sizeof(IRQ_State), 1, fp);
  fwrite(&g_wPassedClocks, sizeof(g_wPassedClocks), 1, fp);
  fwrite(g_byTestTable, sizeof(g_byTestTable), 1, fp);
  fwrite(g_ASLTable, sizeof(g_ASLTable), 1, fp);
  fwrite(g_LSRTable, sizeof(g_LSRTable), 1, fp);
  fwrite(g_ROLTable, sizeof(g_ROLTable), 1, fp);
  fwrite(g_RORTable, sizeof(g_RORTable), 1, fp);

  /* PPU State */

  fwrite(PPUBANK, sizeof(BYTE *) * 16, 1, fp);
  fwrite(PPURAM, PPURAM_SIZE, 1, fp);
  fwrite(SPRRAM, SPRRAM_SIZE, 1, fp);

  fwrite(&PPU_R0, sizeof(PPU_R0), 1, fp);
  fwrite(&PPU_R1, sizeof(PPU_R1), 1, fp);
  fwrite(&PPU_R2, sizeof(PPU_R2), 1, fp);
  fwrite(&PPU_R3, sizeof(PPU_R3), 1, fp);
  fwrite(&PPU_R7, sizeof(PPU_R7), 1, fp);

  fwrite(&PPU_Scr_V, sizeof(PPU_Scr_V), 1, fp);
  fwrite(&PPU_Scr_V_Next, sizeof(PPU_Scr_V_Next), 1, fp);
  fwrite(&PPU_Scr_V_Byte, sizeof(PPU_Scr_V_Byte), 1, fp);
  fwrite(&PPU_Scr_V_Byte_Next, sizeof(PPU_Scr_V_Byte_Next), 1, fp);
  fwrite(&PPU_Scr_V_Bit, sizeof(PPU_Scr_V_Bit), 1, fp);
  fwrite(&PPU_Scr_V_Bit_Next, sizeof(PPU_Scr_V_Bit_Next), 1, fp);
  fwrite(&PPU_Scr_H, sizeof(PPU_Scr_H), 1, fp);
  fwrite(&PPU_Scr_H_Next, sizeof(PPU_Scr_H_Next), 1, fp);
  fwrite(&PPU_Scr_H_Byte, sizeof(PPU_Scr_H_Byte), 1, fp);
  fwrite(&PPU_Scr_H_Byte_Next, sizeof(PPU_Scr_H_Byte_Next), 1, fp);
  fwrite(&PPU_Scr_H_Bit, sizeof(PPU_Scr_H_Bit), 1, fp);
  fwrite(&PPU_Scr_H_Bit_Next, sizeof(PPU_Scr_H_Bit_Next), 1, fp);

  fwrite(&PPU_Addr, sizeof(PPU_Addr), 1, fp);
  fwrite(&PPU_Temp, sizeof(PPU_Temp), 1, fp);
  fwrite(&PPU_Increment, sizeof(PPU_Increment), 1, fp);
  fwrite(&PPU_Scanline, sizeof(PPU_Scanline), 1, fp);
  fwrite(PPU_ScanTable, sizeof(PPU_ScanTable), 1, fp); 
  fwrite(&PPU_NameTableBank, sizeof(PPU_NameTableBank), 1, fp);
  fwrite(&PPU_SP_Height, sizeof(PPU_SP_Height),1, fp);
  fwrite(&SpriteJustHit, sizeof(SpriteJustHit), 1, fp);
  fwrite(&byVramWriteEnable, sizeof(byVramWriteEnable), 1, fp);
  fwrite(&PPU_Latch_Flag, sizeof(PPU_Latch_Flag), 1, fp);
  fwrite(&PPU_UpDown_Clip, sizeof(PPU_UpDown_Clip), 1, fp);
  fwrite(&FrameIRQ_Enable, sizeof(FrameIRQ_Enable), 1, fp);
  fwrite(&FrameStep, sizeof(FrameStep), 1, fp);
  fwrite(APU_Reg, sizeof(APU_Reg), 1, fp);
  fwrite(&ApuCtrl, sizeof(ApuCtrl), 1, fp);
  fwrite(&ApuCtrlNew, sizeof(ApuCtrlNew), 1, fp);

  /* Display State */
  fwrite(&FrameCnt, sizeof(FrameCnt), 1, fp);
  fwrite(ChrBuf, sizeof(ChrBuf), 1, fp);
  fwrite(&ChrBufUpdate, sizeof(ChrBufUpdate), 1, fp);
  fwrite(PalTable, sizeof(PalTable), 1, fp);
  fwrite(pbyPrevBanks, sizeof(pbyPrevBanks), 1, fp);

  /* Mapper States */
  if (MapperNo == 1) {
      fwrite(Map1_Regs, sizeof(Map1_Regs), 1, fp);
      fwrite(&Map1_Cnt, sizeof(Map1_Cnt), 1, fp);
      fwrite(&Map1_Latch, sizeof(Map1_Latch), 1, fp);
      fwrite(&Map1_Last_Write_Addr, sizeof(Map1_Last_Write_Addr), 1, fp);
      fwrite(&Map1_Size, sizeof(Map1_Size), 1, fp);
      fwrite(&Map1_256K_base, sizeof(Map1_256K_base), 1, fp);
      fwrite(&Map1_swap, sizeof(Map1_swap), 1, fp);
      fwrite(&Map1_bank1, sizeof(Map1_bank1), 1, fp);
      fwrite(&Map1_bank2, sizeof(Map1_bank2), 1, fp);
      fwrite(&Map1_bank3, sizeof(Map1_bank3), 1, fp);
      fwrite(&Map1_bank4, sizeof(Map1_bank4), 1, fp);
      fwrite(&Map1_HI1, sizeof(Map1_HI1), 1, fp);
      fwrite(&Map1_HI2, sizeof(Map1_HI2), 1, fp);
  } else if (MapperNo == 4) {
      fwrite(Map4_Regs, sizeof(Map4_Regs), 1, fp);
      fwrite(&Map4_Rom_Bank, sizeof(Map4_Rom_Bank), 1, fp);
      fwrite(&Map4_Prg0, sizeof(Map4_Prg0), 1, fp);
      fwrite(&Map4_Prg1, sizeof(Map4_Prg1), 1, fp);
      fwrite(&Map4_Chr01, sizeof(Map4_Chr01), 1, fp);
      fwrite(&Map4_Chr23, sizeof(Map4_Chr23), 1, fp);
      fwrite(&Map4_Chr4, sizeof(Map4_Chr4), 1, fp);
      fwrite(&Map4_Chr5, sizeof(Map4_Chr5), 1, fp);
      fwrite(&Map4_Chr6, sizeof(Map4_Chr6), 1, fp);
      fwrite(&Map4_Chr7, sizeof(Map4_Chr7), 1, fp);
      fwrite(&Map4_IRQ_Enable, sizeof(Map4_IRQ_Enable), 1, fp);
      fwrite(&Map4_IRQ_Cnt, sizeof(Map4_IRQ_Cnt), 1, fp);
      fwrite(&Map4_IRQ_Latch, sizeof(Map4_IRQ_Latch), 1, fp);
      fwrite(&Map4_IRQ_Request, sizeof(Map4_IRQ_Request), 1, fp);
      fwrite(&Map4_IRQ_Present, sizeof(Map4_IRQ_Present), 1, fp);
      fwrite(&Map4_IRQ_Present_Vbl, sizeof(Map4_IRQ_Present_Vbl), 1, fp);
  } else if (MapperNo == 5) {
      fwrite(Map5_Wram, sizeof(Map5_Wram), 1, fp); 
      fwrite(Map5_Ex_Ram, sizeof(Map5_Ex_Ram), 1, fp);
      fwrite(Map5_Ex_Vram, sizeof(Map5_Ex_Vram), 1, fp);
      fwrite(Map5_Ex_Nam, sizeof(Map5_Ex_Nam), 1, fp);
      fwrite(Map5_Prg_Reg, sizeof(Map5_Prg_Reg), 1, fp);
      fwrite(Map5_Wram_Reg, sizeof(Map5_Wram_Reg), 1, fp);
      fwrite(Map5_Chr_Reg, sizeof(Map5_Chr_Reg), 1, fp);
      fwrite(&Map5_IRQ_Enable, sizeof(Map5_IRQ_Enable), 1, fp);
      fwrite(&Map5_IRQ_Status, sizeof(Map5_IRQ_Status), 1, fp);
      fwrite(&Map5_IRQ_Line, sizeof(Map5_IRQ_Line), 1, fp);
      fwrite(&Map5_Value0, sizeof(Map5_Value0), 1, fp);
      fwrite(&Map5_Value1, sizeof(Map5_Value1), 1, fp);
      fwrite(&Map5_Wram_Protect0, sizeof(Map5_Wram_Protect0), 1, fp);
      fwrite(&Map5_Wram_Protect1, sizeof(Map5_Wram_Protect1), 1, fp);
      fwrite(&Map5_Prg_Size, sizeof(Map5_Prg_Size), 1, fp);
      fwrite(&Map5_Chr_Size, sizeof(Map5_Chr_Size), 1, fp);
      fwrite(&Map5_Gfx_Mode, sizeof(Map5_Gfx_Mode), 1, fp);
  } else if (MapperNo == 6) {
      fwrite(&Map6_IRQ_Enable, sizeof(Map6_IRQ_Enable), 1, fp);
      fwrite(&Map6_IRQ_Cnt, sizeof(Map6_IRQ_Cnt), 1, fp);
      fwrite(Map6_Chr_Ram, sizeof(Map6_Chr_Ram), 1, fp);
  } else if (MapperNo == 9) {
      fwrite(&latch1, sizeof(latch1), 1, fp);
      fwrite(&latch2, sizeof(latch2), 1, fp);
  } else if (MapperNo == 10) {
      fwrite(&latch3, sizeof(latch3), 1, fp);
      fwrite(&latch4, sizeof(latch4), 1, fp);
  }

  /* More mappers here as needed... */

  fclose(fp);
  free(fn);
  return 0;
}

int LoadState()
{
  FILE *fp;

  strcpy(fileName + (strlen(fileName)-3), "sav");
  LOGDEBUG("LoadState: Loading State from %s", fileName);

  fp = fopen(fileName, "rb");
  if ( fp == NULL )
    return -1;

  /* RAM State */

  fread(&RAM, RAM_SIZE, 1, fp);
//  fread(&DRAM, DRAM_SIZE, 1, fp);

  /* 65C02 State */

  fread(&PC, sizeof(PC), 1, fp);
  fread(&SP, sizeof(SP), 1, fp);
  fread(&A, sizeof(A), 1, fp);
  fread(&X, sizeof(X), 1, fp);
  fread(&Y, sizeof(Y), 1, fp);
  fread(&F, sizeof(F), 1, fp);

  fread(&NMI_State, sizeof(NMI_State), 1, fp);
  fread(&IRQ_State, sizeof(IRQ_State), 1, fp);
  fread(&g_wPassedClocks, sizeof(g_wPassedClocks), 1, fp);
  fread(&g_byTestTable, sizeof(g_byTestTable), 1, fp);
  fread(&g_ASLTable, sizeof(g_ASLTable), 1, fp);
  fread(&g_LSRTable, sizeof(g_LSRTable), 1, fp);
  fread(&g_ROLTable, sizeof(g_ROLTable), 1, fp);
  fread(&g_RORTable, sizeof(g_RORTable), 1, fp);

  /* PPU State */

  fread(&PPUBANK, sizeof(BYTE *)*16, 1, fp);
  fread(&PPURAM, PPURAM_SIZE, 1, fp);
  fread(&SPRRAM, SPRRAM_SIZE, 1, fp);

  fread(&PPU_R0, sizeof(PPU_R0), 1, fp);
  fread(&PPU_R1, sizeof(PPU_R1), 1, fp);
  fread(&PPU_R2, sizeof(PPU_R2), 1, fp);
  fread(&PPU_R3, sizeof(PPU_R3), 1, fp);
  fread(&PPU_R7, sizeof(PPU_R7), 1, fp);

  fread(&PPU_Scr_V, sizeof(PPU_Scr_V), 1, fp);
  fread(&PPU_Scr_V_Next, sizeof(PPU_Scr_V_Next), 1, fp);
  fread(&PPU_Scr_V_Byte, sizeof(PPU_Scr_V_Byte), 1, fp);
  fread(&PPU_Scr_V_Byte_Next, sizeof(PPU_Scr_V_Byte_Next), 1, fp);
  fread(&PPU_Scr_V_Bit, sizeof(PPU_Scr_V_Bit), 1, fp);
  fread(&PPU_Scr_V_Bit_Next, sizeof(PPU_Scr_V_Bit_Next), 1, fp);
  fread(&PPU_Scr_H, sizeof(PPU_Scr_H), 1, fp);
  fread(&PPU_Scr_H_Next, sizeof(PPU_Scr_H_Next), 1, fp);
  fread(&PPU_Scr_H_Byte, sizeof(PPU_Scr_H_Byte), 1, fp);
  fread(&PPU_Scr_H_Byte_Next, sizeof(PPU_Scr_H_Byte_Next), 1, fp);
  fread(&PPU_Scr_H_Bit, sizeof(PPU_Scr_H_Bit), 1, fp);
  fread(&PPU_Scr_H_Bit_Next, sizeof(PPU_Scr_H_Bit_Next), 1, fp);

  fread(&PPU_Addr, sizeof(PPU_Addr), 1, fp);
  fread(&PPU_Temp, sizeof(PPU_Temp), 1, fp);
  fread(&PPU_Increment, sizeof(PPU_Increment), 1, fp);
  fread(&PPU_Scanline, sizeof(PPU_Scanline), 1, fp);
  fread(&PPU_ScanTable, sizeof(PPU_ScanTable), 1, fp);
  fread(&PPU_NameTableBank, sizeof(PPU_NameTableBank), 1, fp);
  fread(&PPU_SP_Height, sizeof(PPU_SP_Height),1, fp);
  fread(&SpriteJustHit, sizeof(SpriteJustHit), 1, fp);
  fread(&byVramWriteEnable, sizeof(byVramWriteEnable), 1, fp);
  fread(&PPU_Latch_Flag, sizeof(PPU_Latch_Flag), 1, fp);
  fread(&PPU_UpDown_Clip, sizeof(PPU_UpDown_Clip), 1, fp);
  fread(&FrameIRQ_Enable, sizeof(FrameIRQ_Enable), 1, fp);
  fread(&FrameStep, sizeof(FrameStep), 1, fp);
  fread(APU_Reg, sizeof(APU_Reg), 1, fp);
  fread(&ApuCtrl, sizeof(ApuCtrl), 1, fp);
  fread(&ApuCtrlNew, sizeof(ApuCtrlNew), 1, fp);

  /* Display State */
  fread(&FrameCnt, sizeof(FrameCnt), 1, fp);
  fread(&ChrBuf, sizeof(ChrBuf), 1, fp);
  fread(&ChrBufUpdate, sizeof(ChrBufUpdate), 1, fp);
  fread(&PalTable, sizeof(PalTable), 1, fp);
  fread(&pbyPrevBanks, sizeof(pbyPrevBanks), 1, fp);

  /* Mapper States */
  if (MapperNo == 1) {
      fread(&Map1_Regs, sizeof(Map1_Regs), 1, fp);
      fread(&Map1_Cnt, sizeof(Map1_Cnt), 1, fp);
      fread(&Map1_Latch, sizeof(Map1_Latch), 1, fp);
      fread(&Map1_Last_Write_Addr, sizeof(Map1_Last_Write_Addr), 1, fp);
      fread(&Map1_Size, sizeof(Map1_Size), 1, fp);
      fread(&Map1_256K_base, sizeof(Map1_256K_base), 1, fp);
      fread(&Map1_swap, sizeof(Map1_swap), 1, fp);
      fread(&Map1_bank1, sizeof(Map1_bank1), 1, fp);
      fread(&Map1_bank2, sizeof(Map1_bank2), 1, fp);
      fread(&Map1_bank3, sizeof(Map1_bank3), 1, fp);
      fread(&Map1_bank4, sizeof(Map1_bank4), 1, fp);
      fread(&Map1_HI1, sizeof(Map1_HI1), 1, fp);
      fread(&Map1_HI2, sizeof(Map1_HI2), 1, fp);
  } else if (MapperNo == 4) {
      fread(&Map4_Regs, sizeof(Map4_Regs), 1, fp);
      fread(&Map4_Rom_Bank, sizeof(Map4_Rom_Bank), 1, fp);
      fread(&Map4_Prg0, sizeof(Map4_Prg0), 1, fp);
      fread(&Map4_Prg1, sizeof(Map4_Prg1), 1, fp);
      fread(&Map4_Chr01, sizeof(Map4_Chr01), 1, fp);
      fread(&Map4_Chr23, sizeof(Map4_Chr23), 1, fp);
      fread(&Map4_Chr4, sizeof(Map4_Chr4), 1, fp);
      fread(&Map4_Chr5, sizeof(Map4_Chr5), 1, fp);
      fread(&Map4_Chr6, sizeof(Map4_Chr6), 1, fp);
      fread(&Map4_Chr7, sizeof(Map4_Chr7), 1, fp);
      fread(&Map4_IRQ_Enable, sizeof(Map4_IRQ_Enable), 1, fp);
      fread(&Map4_IRQ_Cnt, sizeof(Map4_IRQ_Cnt), 1, fp);
      fread(&Map4_IRQ_Latch, sizeof(Map4_IRQ_Latch), 1, fp);
      fread(&Map4_IRQ_Request, sizeof(Map4_IRQ_Request), 1, fp);
      fread(&Map4_IRQ_Present, sizeof(Map4_IRQ_Present), 1, fp);
      fread(&Map4_IRQ_Present_Vbl, sizeof(Map4_IRQ_Present_Vbl), 1, fp);
  } else if (MapperNo == 5) {
      fread(&Map5_Wram, sizeof(Map5_Wram), 1, fp);
      fread(&Map5_Ex_Ram, sizeof(Map5_Ex_Ram), 1, fp);
      fread(&Map5_Ex_Vram, sizeof(Map5_Ex_Vram), 1, fp);
      fread(&Map5_Ex_Nam, sizeof(Map5_Ex_Nam), 1, fp);
      fread(&Map5_Prg_Reg, sizeof(Map5_Prg_Reg), 1, fp);
      fread(&Map5_Wram_Reg, sizeof(Map5_Wram_Reg), 1, fp);
      fread(&Map5_Chr_Reg, sizeof(Map5_Chr_Reg), 1, fp);
      fread(&Map5_IRQ_Enable, sizeof(Map5_IRQ_Enable), 1, fp);
      fread(&Map5_IRQ_Status, sizeof(Map5_IRQ_Status), 1, fp);
      fread(&Map5_IRQ_Line, sizeof(Map5_IRQ_Line), 1, fp);
      fread(&Map5_Value0, sizeof(Map5_Value0), 1, fp);
      fread(&Map5_Value1, sizeof(Map5_Value1), 1, fp);
      fread(&Map5_Wram_Protect0, sizeof(Map5_Wram_Protect0), 1, fp);
      fread(&Map5_Wram_Protect1, sizeof(Map5_Wram_Protect1), 1, fp);
      fread(&Map5_Prg_Size, sizeof(Map5_Prg_Size), 1, fp);
      fread(&Map5_Chr_Size, sizeof(Map5_Chr_Size), 1, fp);
      fread(&Map5_Gfx_Mode, sizeof(Map5_Gfx_Mode), 1, fp);
  } else if (MapperNo == 6) {
      fread(&Map6_IRQ_Enable, sizeof(Map6_IRQ_Enable), 1, fp);
      fread(&Map6_IRQ_Cnt, sizeof(Map6_IRQ_Cnt), 1, fp);
      fread(&Map6_Chr_Ram, sizeof(Map6_Chr_Ram), 1, fp);
  } else if (MapperNo == 9) {
      fread(&latch1, sizeof(latch1), 1, fp);
      fread(&latch2, sizeof(latch2), 1, fp);
  } else if (MapperNo == 10) {
      fread(&latch3, sizeof(latch3), 1, fp);
      fread(&latch4, sizeof(latch4), 1, fp);
  } 

  fclose(fp);

  int nBank, nIdx;

  for(nBank = 0; nBank < 8; nBank++) {
    pbyPrevBank[ nBank ] = PPUBANK[ pbyPrevBanks[ nBank ] ];
  }

  return 0;
}

#ifdef DEBUG
void LOGDEBUG(const char *text, ...)
{
  char debug_text[1024];
  va_list args;
  FILE *f;

  va_start (args, text);
  vsnprintf (debug_text, sizeof (debug_text), text, args);
  va_end (args);

  f = fopen("/tmp/NES.debug", "a");
  fprintf(f, "%s\n", debug_text);
  fclose(f);
}
#endif
