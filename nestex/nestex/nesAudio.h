//
//  nesAudio.h
//  nestor
//
//  Created by Tommy de Roos on 4/1/13.
//
//

#import <Foundation/Foundation.h>

#include <OpenAL/al.h>
#include <Openal/alc.h>


@interface nesAudio : NSObject
{
    ALCcontext* mContext;
    ALCdevice* mDevice;
    
    NSMutableArray *mAudioBuffers;
    
    ALuint      mSource;
    ALuint      mBufferIDs[3];
    
    NSInteger   mCurrentBuffer;
}

-(id) init;
-(void) render:(unsigned char*)inBuffer bufferSize:(ALsizei)inBufferSize;
-(bool) needsRefresh;

@end
