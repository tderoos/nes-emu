//
//  nesAudio.m
//  nestor
//
//  Created by Tommy de Roos on 4/1/13.
//
//

#import "nesAudio.h"
#import <AudioUnit/AudioUnit.h>

#include <AudioToolbox/AudioToolbox.h>
//#include <AudioToolbox/AudioSession.h>
#include <AudioUnit/AudioUnit.h>
#include <AudioUnit/AudioUnitProperties.h>
#include <stdio.h>

#include <OpenAL/al.h>
#include <OpenAL/alc.h>

//#include "CAStreamBasicDescription.h"

// This determines how slowly the oscilloscope lines fade away from the display.
// Larger numbers = slower fade (and more strain on the graphics processing)
#define kNumDrawBuffers 12
#define kDefaultDrawSamples 1024
#define kMinDrawSamples 64
#define kMaxDrawSamples 4096


SInt8 *drawBuffers[kNumDrawBuffers];

int drawBufferIdx = 0;
int drawBufferLen = kDefaultDrawSamples;
int drawBufferLen_alloced = 0;

//
//class CAStreamBasicDescription : public AudioStreamBasicDescription
//{
//public:
//    void	SetAUCanonical(UInt32 nChannels, bool interleaved)
//	{
//		mFormatID = kAudioFormatLinearPCM;
//#if CA_PREFER_FIXED_POINT
//		mFormatFlags = kAudioFormatFlagsCanonical | (kAudioUnitSampleFractionBits << kLinearPCMFormatFlagsSampleFractionShift);
//#else
//		mFormatFlags = kAudioFormatFlagsCanonical;
//#endif
//		mChannelsPerFrame = nChannels;
//		mFramesPerPacket = 1;
//		mBitsPerChannel = 8 * sizeof(AudioUnitSampleType);
//		if (interleaved)
//			mBytesPerPacket = mBytesPerFrame = nChannels * sizeof(AudioUnitSampleType);
//		else {
//			mBytesPerPacket = mBytesPerFrame = sizeof(AudioUnitSampleType);
//			mFormatFlags |= kAudioFormatFlagIsNonInterleaved;
//		}
//	}
//};

int SetupRemoteIO (AudioUnit& inRemoteIOUnit, AURenderCallbackStruct inRenderProc, CAStreamBasicDescription& outFormat)
{
    // Open the output unit
    AudioComponentDescription desc;
    desc.componentType = kAudioUnitType_Output;
    desc.componentSubType = kAudioUnitSubType_GenericOutput;
    desc.componentManufacturer = kAudioUnitManufacturer_Apple;
    desc.componentFlags = 0;
    desc.componentFlagsMask = 0;
    
    AudioComponent comp = AudioComponentFindNext(NULL, &desc);
    
    OSStatus stat = AudioComponentInstanceNew(comp, &inRemoteIOUnit);
    
//    UInt32 one = 1;
//    stat = AudioUnitSetProperty(inRemoteIOUnit, kAudioOutputUnitProperty_EnableIO, kAudioUnitScope_Input, 1, &one, sizeof(one));
    
    stat = AudioUnitSetProperty(inRemoteIOUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, 0, &inRenderProc, sizeof(inRenderProc));
    
    // set our required format - Canonical AU format: LPCM non-interleaved 8.24 fixed point
    outFormat.SetAUCanonical(2, false);
    outFormat.mSampleRate = 44100;
    stat = AudioUnitSetProperty(inRemoteIOUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, &outFormat, sizeof(outFormat));
    
//    stat = AudioUnitSetProperty(inRemoteIOUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 1, &outFormat, sizeof(outFormat));
    stat = AudioUnitInitialize(inRemoteIOUnit);

	return 0;
}


void rioInterruptionListener(void *inClientData, UInt32 inInterruption)
{
//	printf("Session interrupted! --- %s ---", inInterruption == kAudioSessionBeginInterruption ? "Begin Interruption" : "End Interruption");
}


static OSStatus	PerformThru(
							void						*inRefCon,
							AudioUnitRenderActionFlags 	*ioActionFlags,
							const AudioTimeStamp 		*inTimeStamp,
							UInt32 						inBusNumber,
							UInt32 						inNumberFrames,
							AudioBufferList 			*ioData)
{

	return 0;
}
@implementation nesAudio

// open the audio file
// returns a big audio ID struct
-(AudioFileID)openAudioFile:(NSString*)filePath
{
	AudioFileID outAFID;
	// use the NSURl instead of a cfurlref cuz it is easier
	NSURL * afUrl = [NSURL fileURLWithPath:filePath];
	
	// do some platform specific stuff..
#if TARGET_OS_IPHONE
	OSStatus result = AudioFileOpenURL((CFURLRef)afUrl, kAudioFileReadPermission, 0, &outAFID);
#else
	OSStatus result = AudioFileOpenURL((CFURLRef)afUrl, fsRdPerm, 0, &outAFID);
#endif
	if (result != 0) NSLog(@"cannot openf file: %@",filePath);
        return outAFID;
}

// find the audio portion of the file
// return the size in bytes
-(UInt32)audioFileSize:(AudioFileID)fileDescriptor
{
	UInt64 outDataSize = 0;
	UInt32 thePropSize = sizeof(UInt64);
	OSStatus result = AudioFileGetProperty(fileDescriptor, kAudioFilePropertyAudioDataByteCount, &thePropSize, &outDataSize);
	if(result != 0) NSLog(@"cannot find file size");
	return (UInt32)outDataSize;
}

-(id) init
{
    [super init];

    // Initialization
	mDevice = alcOpenDevice(NULL); // select the "preferred device"
	if (mDevice) {
		// use the device to make a context
		mContext=alcCreateContext(mDevice,NULL);
		// set my context to the currently active one
		alcMakeContextCurrent(mContext);
        
        // get the full path of the file
        NSString* fileName = [[NSBundle mainBundle] pathForResource:@"button_press" ofType:@"caf"];
        // first, open the file
        AudioFileID fileID = [self openAudioFile:fileName];
        
        // find out how big the actual audio data is
        UInt32 fileSize = [self audioFileSize:fileID];
        
        // this is where the audio data will live for the moment
        unsigned char * outData = (unsigned char*)malloc(fileSize);
        
        // this where we actually get the bytes from the file and put them
        // into the data buffer
        OSStatus result = noErr;
        result = AudioFileReadBytes(fileID, false, 0, &fileSize, outData);
        AudioFileClose(fileID); //close the file
        
        if (result != 0) NSLog(@"cannot load effect: %@",fileName);
        
        ALuint bufferID;
        // grab a buffer ID from openAL
        alGenBuffers(1, &bufferID);
        
        // jam the audio data into the new buffer
        alBufferData(bufferID,AL_FORMAT_STEREO16,outData,fileSize,44100);
        
        // save the buffer so I can release it later
//        [bufferStorageArray addObject:[NSNumber numberWithUnsignedInteger:bufferID]];
        
        ALuint sourceID;
        
        // grab a source ID from openAL
        alGenSources(1, &sourceID);
        
        // attach the buffer to the source
        alSourcei(sourceID, AL_BUFFER, bufferID);
        // set some basic source prefs
        alSourcef(sourceID, AL_PITCH, 1.0f);
        alSourcef(sourceID, AL_GAIN, 1.0f);
        
        bool loops = true;
        if (loops) alSourcei(sourceID, AL_LOOPING, AL_TRUE);
        
        // store this for future use
//        [soundDictionary setObject:[NSNumber numberWithUnsignedInt:sourceID] forKey:@"neatoSound"];
        
        // clean up the buffer
        if (outData)
        {
            free(outData);
            outData = NULL;
        }
        
  //      NSNumber * numVal = [soundDictionary objectForKey:soundKey];
   //     if (numVal != nil)
        {
//            NSUInteger sourceID = [numVal unsignedIntValue];
            alSourcePlay(sourceID);
        }
	}
    
//    OSStatus stat = AudioSessionInitialize(NULL, NULL, rioInterruptionListener, self);
//    
//    UInt32 audioCategory = kAudioSessionCategory_PlayAndRecord;
//    stat = AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(audioCategory), &audioCategory);
//    stat = AudioSessionAddPropertyListener(kAudioSessionProperty_AudioRouteChange, propListener, self);
    
//    Float32 preferredBufferSize = .005;
//    XThrowIfError(AudioSessionSetProperty(kAudioSessionProperty_PreferredHardwareIOBufferDuration, sizeof(preferredBufferSize), &preferredBufferSize), "couldn't set i/o buffer duration");
//    
//    UInt32 size = sizeof(hwSampleRate);
//    XThrowIfError(AudioSessionGetProperty(kAudioSessionProperty_CurrentHardwareSampleRate, &size, &hwSampleRate), "couldn't get hw sample rate");
    
//    stat = AudioSessionSetActive(true);

    
    
    
    
//    inputProc.inputProc = PerformThru;
//	inputProc.inputProcRefCon = self;
//
//    SetupRemoteIO(rioUnit, inputProc, thruFormat);
//    stat = AudioOutputUnitStart(rioUnit);
    
    return self;
}
@end
