enum
{
    kAudioHardwarePropertyProcessIsMaster                   = 'mast',
    kAudioHardwarePropertyIsInitingOrExiting                = 'inot',
    kAudioHardwarePropertyDevices                           = 'dev#',
    kAudioHardwarePropertyDefaultInputDevice                = 'dIn ',
    kAudioHardwarePropertyDefaultOutputDevice               = 'dOut',
    kAudioHardwarePropertyDefaultSystemOutputDevice         = 'sOut',
    kAudioHardwarePropertyDeviceForUID                      = 'duid',
    kAudioHardwarePropertySleepingIsAllowed                 = 'slep',
    kAudioHardwarePropertyUnloadingIsAllowed                = 'unld',
    kAudioHardwarePropertyHogModeIsAllowed                  = 'hogr',
    kAudioHardwarePropertyRunLoop                           = 'rnlp',
    kAudioHardwarePropertyPlugInForBundleID                 = 'pibi'
};

enum
{
    kAudioObjectPropertyClass               = 'clas',
    kAudioObjectPropertyOwner               = 'stdv',
    kAudioObjectPropertyCreator             = 'oplg',
    kAudioObjectPropertyName                = 'lnam',
    kAudioObjectPropertyManufacturer        = 'lmak',
    kAudioObjectPropertyElementName         = 'lchn',
    kAudioObjectPropertyElementCategoryName = 'lccn',
    kAudioObjectPropertyElementNumberName   = 'lcnn',
    kAudioObjectPropertyOwnedObjects        = 'ownd',
    kAudioObjectPropertyListenerAdded       = 'lisa',
    kAudioObjectPropertyListenerRemoved     = 'lisr'
};


enum
{
    kAudioDevicePropertyDeviceName                          = 'name',
    kAudioDevicePropertyDeviceNameCFString                  = kAudioObjectPropertyName,
    kAudioDevicePropertyDeviceManufacturer                  = 'makr',
    kAudioDevicePropertyDeviceManufacturerCFString          = kAudioObjectPropertyManufacturer,
    kAudioDevicePropertyRegisterBufferList                  = 'rbuf',
    kAudioDevicePropertyBufferSize                          = 'bsiz',
    kAudioDevicePropertyBufferSizeRange                     = 'bsz#',
    kAudioDevicePropertyChannelName                         = 'chnm',
    kAudioDevicePropertyChannelNameCFString                 = kAudioObjectPropertyElementName,
    kAudioDevicePropertyChannelCategoryName                 = 'ccnm',
    kAudioDevicePropertyChannelCategoryNameCFString         = kAudioObjectPropertyElementCategoryName,
    kAudioDevicePropertyChannelNumberName                   = 'cnnm',
    kAudioDevicePropertyChannelNumberNameCFString           = kAudioObjectPropertyElementNumberName,
    kAudioDevicePropertySupportsMixing                      = 'mix?',
    kAudioDevicePropertyStreamFormat                        = 'sfmt',
    kAudioDevicePropertyStreamFormats                       = 'sfm#',
    kAudioDevicePropertyStreamFormatSupported               = 'sfm?',
    kAudioDevicePropertyStreamFormatMatch                   = 'sfmm',
    kAudioDevicePropertyDataSourceNameForID                 = 'sscn',
    kAudioDevicePropertyClockSourceNameForID                = 'cscn',
    kAudioDevicePropertyPlayThruDestinationNameForID        = 'mddn',
    kAudioDevicePropertyChannelNominalLineLevelNameForID    = 'cnlv'
};





enum
{
    kAudioDevicePropertyPlugIn                          = 'plug',
    kAudioDevicePropertyConfigurationApplication        = 'capp',
    kAudioDevicePropertyDeviceUID                       = 'uid ',
    kAudioDevicePropertyModelUID                        = 'muid',
    kAudioDevicePropertyTransportType                   = 'tran',
    kAudioDevicePropertyRelatedDevices                  = 'akin',
    kAudioDevicePropertyClockDomain                     = 'clkd',
    kAudioDevicePropertyDeviceIsAlive                   = 'livn',
    kAudioDevicePropertyDeviceHasChanged                = 'diff',
    kAudioDevicePropertyDeviceIsRunning                 = 'goin',
    kAudioDevicePropertyDeviceIsRunningSomewhere        = 'gone',
    kAudioDevicePropertyDeviceCanBeDefaultDevice        = 'dflt',
    kAudioDevicePropertyDeviceCanBeDefaultSystemDevice  = 'sflt',
    kAudioDeviceProcessorOverload                       = 'over',
    kAudioDevicePropertyHogMode                         = 'oink',
    kAudioDevicePropertyLatency                         = 'ltnc',
    kAudioDevicePropertyBufferFrameSize                 = 'fsiz',
    kAudioDevicePropertyBufferFrameSizeRange            = 'fsz#',
    kAudioDevicePropertyUsesVariableBufferFrameSizes    = 'vfsz',
    kAudioDevicePropertyStreams                         = 'stm#',
    kAudioDevicePropertySafetyOffset                    = 'saft',
    kAudioDevicePropertyIOCycleUsage                    = 'ncyc',
    kAudioDevicePropertyStreamConfiguration             = 'slay',
    kAudioDevicePropertyIOProcStreamUsage               = 'suse',
    kAudioDevicePropertyPreferredChannelsForStereo      = 'dch2',
    kAudioDevicePropertyPreferredChannelLayout          = 'srnd',
    kAudioDevicePropertyNominalSampleRate               = 'nsrt',
    kAudioDevicePropertyAvailableNominalSampleRates     = 'nsr#',
    kAudioDevicePropertyActualSampleRate                = 'asrt'
};
enum
{
    kAudioFormatLinearPCM               = 'lpcm',
    kAudioFormatAC3                     = 'ac-3',
    kAudioFormat60958AC3                = 'cac3',
    kAudioFormatAppleIMA4               = 'ima4',
    kAudioFormatMPEG4AAC                = 'aac ',
    kAudioFormatMPEG4CELP               = 'celp',
    kAudioFormatMPEG4HVXC               = 'hvxc',
    kAudioFormatMPEG4TwinVQ             = 'twvq',
    kAudioFormatMACE3                   = 'MAC3',
    kAudioFormatMACE6                   = 'MAC6',
    kAudioFormatULaw                    = 'ulaw',
    kAudioFormatALaw                    = 'alaw',
    kAudioFormatQDesign                 = 'QDMC',
    kAudioFormatQDesign2                = 'QDM2',
    kAudioFormatQUALCOMM                = 'Qclp',
    kAudioFormatMPEGLayer1              = '.mp1',
    kAudioFormatMPEGLayer2              = '.mp2',
    kAudioFormatMPEGLayer3              = '.mp3',
    kAudioFormatDVAudio                 = 'dvca',
    kAudioFormatVariableDurationDVAudio = 'vdva',
    kAudioFormatTimeCode                = 'time',
    kAudioFormatMIDIStream              = 'midi',
    kAudioFormatParameterValueStream    = 'apvs',
    kAudioFormatAppleLossless           = 'alac'
};

enum
{
    kAudioFormatFlagIsFloat                     = (1L << 0),
    kAudioFormatFlagIsBigEndian                 = (1L << 1),
    kAudioFormatFlagIsSignedInteger             = (1L << 2),
    kAudioFormatFlagIsPacked                    = (1L << 3),
    kAudioFormatFlagIsAlignedHigh               = (1L << 4),
    kAudioFormatFlagIsNonInterleaved            = (1L << 5),
    kAudioFormatFlagIsNonMixable                = (1L << 6),
    kAudioFormatFlagsAreAllClear                = (1L << 31),
    
    kLinearPCMFormatFlagIsFloat                 = kAudioFormatFlagIsFloat,
    kLinearPCMFormatFlagIsBigEndian             = kAudioFormatFlagIsBigEndian,
    kLinearPCMFormatFlagIsSignedInteger         = kAudioFormatFlagIsSignedInteger,
    kLinearPCMFormatFlagIsPacked                = kAudioFormatFlagIsPacked,
    kLinearPCMFormatFlagIsAlignedHigh           = kAudioFormatFlagIsAlignedHigh,
    kLinearPCMFormatFlagIsNonInterleaved        = kAudioFormatFlagIsNonInterleaved,
    kLinearPCMFormatFlagIsNonMixable            = kAudioFormatFlagIsNonMixable,
    kLinearPCMFormatFlagsAreAllClear            = kAudioFormatFlagsAreAllClear,
    
    kAppleLosslessFormatFlag_16BitSourceData    = 1,
    kAppleLosslessFormatFlag_20BitSourceData    = 2,
    kAppleLosslessFormatFlag_24BitSourceData    = 3,
    kAppleLosslessFormatFlag_32BitSourceData    = 4
};

enum
{
	// arm is big? can't remember
    kAudioFormatFlagsNativeEndian       = kAudioFormatFlagIsBigEndian,
    //kAudioFormatFlagsNativeEndian       = 0,
    kAudioFormatFlagsNativeFloatPacked  = kAudioFormatFlagIsFloat | kAudioFormatFlagsNativeEndian | kAudioFormatFlagIsPacked
};

/*
enum {
  kVariableLengthArray          = 1
};
*/

/*
enum {
  noErr                         = 0
};
*/


/*
typedef int OSErr;
typedef unsigned int 	UInt32;
typedef int	SInt32;
typedef SInt32                          OSStatus;
typedef double Float64;
typedef unsigned long int UInt64;

*/

typedef UInt32      AudioObjectID;
typedef AudioObjectID   AudioDeviceID;
typedef short SInt16;
typedef float Float32;


struct AudioStreamBasicDescription
{
    Float64 mSampleRate;
    UInt32  mFormatID;
    UInt32  mFormatFlags;
    UInt32  mBytesPerPacket;
    UInt32  mFramesPerPacket;
    UInt32  mBytesPerFrame;
    UInt32  mChannelsPerFrame;
    UInt32  mBitsPerChannel;
    UInt32  mReserved;
};
typedef struct AudioStreamBasicDescription  AudioStreamBasicDescription;



struct SMPTETime
{
    SInt16  mSubframes;
    SInt16  mSubframeDivisor;
    UInt32  mCounter;
    UInt32  mType;
    UInt32  mFlags;
    SInt16  mHours;
    SInt16  mMinutes;
    SInt16  mSeconds;
    SInt16  mFrames;
};
typedef struct SMPTETime    SMPTETime;

struct AudioTimeStamp
{
    Float64         mSampleTime;
    UInt64          mHostTime;
    Float64         mRateScalar;
    UInt64          mWordClockTime;
    SMPTETime       mSMPTETime;
    UInt32          mFlags;
    UInt32          mReserved;
};
typedef struct AudioTimeStamp   AudioTimeStamp;

struct AudioBuffer
{
    UInt32  mNumberChannels;
    UInt32  mDataByteSize;
    void*   mData;
};
typedef struct AudioBuffer  AudioBuffer;

struct AudioBufferList
{
    UInt32      mNumberBuffers;
    AudioBuffer mBuffers[kVariableLengthArray];
};
typedef struct AudioBufferList  AudioBufferList;


