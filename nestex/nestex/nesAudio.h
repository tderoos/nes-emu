//
//  nesAudio.h
//  nestor
//
//  Created by Tommy de Roos on 4/1/13.
//
//

#import <Foundation/Foundation.h>

typedef struct ALCdevice_struct ALCdevice;
typedef struct ALCcontext_struct ALCcontext;



@interface nesAudio : NSObject
{
    ALCcontext* mContext;
    ALCdevice* mDevice;
}

-(id) init;

@end
