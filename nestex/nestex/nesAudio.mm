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

-(id) init
{
    [super init];
    
    // Initialization
	mDevice = alcOpenDevice(NULL); // select the "preferred device"
	if (mDevice)
    {
		// use the device to make a context
		mContext=alcCreateContext(mDevice,NULL);

		// set my context to the currently active one
		alcMakeContextCurrent(mContext);

        // Generate an audio source and set buffer to 0 - no buffer (we'll queue them)
        alGenSources(1, &mSource);
        alSourcei(mSource, AL_BUFFER, 0);
        alSourcei(mSource, AL_LOOPING, AL_FALSE);
        alSourcef(mSource, AL_GAIN, 1.0f);
        
        // Generate 3 buffers;
        alGenBuffers(3, mBufferIDs);
    }
    
    return self;
}


-(void) render:(unsigned char*)inBuffer bufferSize:(ALsizei)inBufferSize
{
    ALint buffersProcessed = 0;
    alGetSourcei(mSource, AL_BUFFERS_PROCESSED, &buffersProcessed);
    ALenum error = alGetError();
    
    ALuint buffer = 0;
    if (buffersProcessed > 0)
    {
        alSourceUnqueueBuffers(mSource, 1, &buffer);
        error = alGetError();
    }
    else if (mCurrentBuffer != 3)
        buffer = mBufferIDs[mCurrentBuffer++];
    
    assert(buffer!=0);

    if (buffer != 0)
    {
        alBufferData(buffer,AL_FORMAT_MONO8,inBuffer,inBufferSize,44100);
        error = alGetError();
        
        alSourceQueueBuffers(mSource, 1, &buffer);
        error = alGetError();
    }
    
    ALint state;
    alGetSourcei(mSource, AL_SOURCE_STATE, &state);
    if(state != AL_PLAYING)
    {
        printf("audio drop\n");
        alSourcePlay(mSource);
        error = alGetError();
    }
}


-(bool) needsRefresh
{
    ALint buffersProcessed = 0, buffersQueued;
    alGetSourcei(mSource, AL_BUFFERS_PROCESSED, &buffersProcessed);
    alGetSourcei(mSource, AL_BUFFERS_QUEUED, &buffersQueued);

    return buffersProcessed > 0 || buffersQueued < 3;
}

@end
