//
// Created by tderoos on 3/3/13.
//
// To change the template use AppCode | Preferences | File Templates.
//

#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <mm_malloc.h>
#include <fcntl.h>
#include "Rom.h"
#include "Mapper.h"


Rom::Rom(char const *inFilename) :
    mData(NULL),
    mCHRData(NULL)
{
    int fd = -1;
    if ((fd = open(inFilename, O_RDONLY, 0)) != -1)
    {
        struct stat file_stats;
        fstat(fd, &file_stats);
        mData = (uint8_t *) mmap(NULL, file_stats.st_size, PROT_READ, MAP_FILE|MAP_PRIVATE, fd, 0);
        
        UInt8 num_prg = mData[4];
        UInt8 num_chr = mData[5];
        
        
        
        UInt8 mapper_id = mData[6] >> 4;
        mapper_id       = mapper_id | (mData[7] & 0xF0);
        
        mSRam        = (mData[10] & 0x10) != 0;
        mSRamBattery = (mData[6]  & 0x02) != 0;
        
        switch(mData[6]&9)
        {
            case 0: mVRamMirrorRom = HORIZONTAL;    break;
            case 1: mVRamMirrorRom = VERTICAL;      break;
            case 8: mVRamMirrorRom = FOUR_SCREEN;   break;
        }
        mVRamMirrorMapped = mVRamMirrorRom;

        printf("Loaded %s (PRG:%d CHR:%d MAP:%d VRM:%d SR:%d SRB:%d)\n", inFilename, num_prg, num_chr, mapper_id, (int)GetVRamMirroring(), (int)mSRam, (int)mSRamBattery);
        
        mMapper = Mapper::sCreate(mapper_id, num_prg, num_chr);
    }
}


// Set VRam - allows mapper to update CHR page directly into VRam
void Rom::SetVRam(UInt8* mVRam)
{
    mCHRData = mVRam;
    mMapper->UpdateMapping(mData+0x10, mPRGData, mCHRData, mVRamMirrorRom, &mVRamMirrorMapped);
}


// Store are passed on to the mapper
void Rom::Store(UInt16 inAddr, UInt8 inValue)
{
    mMapper->Store(inAddr, inValue);
    
    if (mMapper->IsDirty())
        mMapper->UpdateMapping(mData+0x10, mPRGData, mCHRData, mVRamMirrorRom, &mVRamMirrorMapped);
}




