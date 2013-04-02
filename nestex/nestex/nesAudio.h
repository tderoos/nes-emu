//
//  nesAudio.h
//  nestor
//
//  Created by Tommy de Roos on 4/1/13.
//
//

#import <Foundation/Foundation.h>

#import <AudioUnit/AudioUnit.h>
#import <AudioToolbox/AudioToolbox.h>

typedef struct ALCdevice_struct ALCdevice;
typedef struct ALCcontext_struct ALCcontext;


class CAStreamBasicDescription : public AudioStreamBasicDescription
{
public:
    void	SetAUCanonical(UInt32 nChannels, bool interleaved)
	{
		mFormatID = kAudioFormatLinearPCM;
#if CA_PREFER_FIXED_POINT
		mFormatFlags = kAudioFormatFlagsCanonical | (kAudioUnitSampleFractionBits << kLinearPCMFormatFlagsSampleFractionShift);
#else
		mFormatFlags = kAudioFormatFlagsCanonical;
#endif
		mChannelsPerFrame = nChannels;
		mFramesPerPacket = 1;
		mBitsPerChannel = 8 * sizeof(AudioUnitSampleType);
		if (interleaved)
			mBytesPerPacket = mBytesPerFrame = nChannels * sizeof(AudioUnitSampleType);
		else {
			mBytesPerPacket = mBytesPerFrame = sizeof(AudioUnitSampleType);
			mFormatFlags |= kAudioFormatFlagIsNonInterleaved;
		}
	}
};


@interface nesAudio : NSObject
{
    AURenderCallbackStruct		inputProc;
	CAStreamBasicDescription	thruFormat;
	AudioUnit					rioUnit;

    
    ALCcontext* mContext;
    ALCdevice* mDevice;
}

-(id) init;

@end
