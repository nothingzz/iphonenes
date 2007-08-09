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
#include "InfoNES/InfoNES.h"
#include "CoreAudio.h"
#include "InfoNES_iPhone.h"
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/select.h>
#include <unistd.h>

extern void updateScreen();
extern CoreSurfaceBufferRef screenSurface;
extern int __screenOrientation;
extern int audioIsSpeaker;

extern pthread_cond_t screenUpdateLock;
extern pthread_mutex_t screenUpdateMutex;

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
    unsigned short *c;
    int x, y, i = 0;

    c = CoreSurfaceBufferGetBaseAddress(screenSurface);

    pthread_mutex_lock(&screenUpdateMutex);

    if (__screenOrientation == 3) {

        for (y=0; y < 240; y++)
        {
            for (x=0; x<256; x++) {
                    c[ ((x+1) * 240) - (y+1) ] = NesPalette[WorkFrame[(256*y)+x]];
            }
        }
    } else {

        for (y=0; y < 240; y++)
        {
            for (x=0; x<256; x++) {
                c[i++] = NesPalette[WorkFrame[(256*y)+x]];
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
#ifdef DEBUG
             FILE *f = fopen("/tmp/NES.debug", "a");
             fprintf(f, "InfoNES_SoundInit.getBasebandDevice: Found baseband output\n");
             fclose(f);
#endif
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

#ifdef DEBUG
    FILE *f;
    f = fopen("/tmp/NES.debug", "a");
#endif

    err = AudioDeviceAddIOProc((audioIsSpeaker) ? basebandDevice : defaultOutputDevice, AudioOutputProc, 0);
#ifdef DEBUG
    fprintf(f, "InfoNES_SoundOpen.AudioDeviceAddIOProc(%d) returned %d\n", audioIsSpeaker, err);
#endif
    if (err) {
#ifdef DEBUG
      fclose(f);
#endif
      return 1;
    } 

    err = AudioDeviceStart((audioIsSpeaker) ? basebandDevice : defaultOutputDevice, AudioOutputProc);
#ifdef DEBUG
    fprintf(f, "InfoNES_SoundOpen.AudioDeviceStart(%d) returned %d\n", audioIsSpeaker, err);
#endif
    if (err) {
#ifdef DEBUG
      fclose(f);
#endif
      return 1;
    }

#ifdef DEBUG
    fclose(f);
#endif
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
	InfoNES_Main();
}
