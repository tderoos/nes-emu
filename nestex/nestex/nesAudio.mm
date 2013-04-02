//
//  nesAudio.m
//  nestor
//
//  Created by Tommy de Roos on 4/1/13.
//
//

#import "nesAudio.h"
#include <stdio.h>

#include <OpenAL/al.h>
#include <OpenAL/alc.h>

#import <AudioToolbox/AudioToolbox.h>


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
    
    return self;
}
@end
