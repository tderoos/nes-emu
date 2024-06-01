//
// Created by tderoos on 3/3/13.
//
// To change the template use AppCode | Preferences | File Templates.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "Rom.h"
#include "Mapper.h"


Rom::Rom(char const *inFilename) :
    mFilename(inFilename),
    mData(NULL),
    mCHRData(NULL)
{
    FILE* fd = nullptr;
    if (fopen_s(&fd, inFilename, "r") == 0)
    {
        fseek(fd, 0L, SEEK_END);
        int size = ftell(fd);
        fseek(fd, 0l, SEEK_SET);

        mData = (uint8*) malloc(size);
        if (mData == nullptr)
            return;
        fread_s(mData, size, 1, size, fd);
        fclose(fd);

        uint8 num_prg = mData[4];
        uint8 num_chr = mData[5];
        
        uint8 mapper_id = mData[6] >> 4;
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
        
        // Load savegame
        if (mSRamBattery)
        {
            mSavename = mFilename;
            size_t idx = mSavename.find(".nes");
            mSavename.replace(idx, idx + 4, ".sav");
            
            FILE* savegame = fopen(mSavename.c_str(), "r");
            if (savegame != NULL)
            {
                fread(mPRGRam, 1, 0x2000, savegame);
                fclose(savegame);
            }
            else
                memset(mPRGRam, 0, 0x2000);
        }
    }
}


// Set VRam - allows mapper to update CHR page directly into VRam
void Rom::SetVRam(uint8* mVRam)
{
    mCHRData = mVRam;
    mMapper->UpdateMapping(mData+0x10, mPRGData, mCHRData, mVRamMirrorRom, &mVRamMirrorMapped);
}


// Store are passed on to the mapper
void Rom::Store(uint16 inAddr, uint8 inValue)
{
    if (inAddr >= 0x6000 && inAddr < 0x8000)
    {
        mPRGRam[inAddr - 0x6000] = inValue;
    }
    else
    {
        mMapper->Store(inAddr, inValue);
        
        if (mMapper->IsDirty())
            mMapper->UpdateMapping(mData+0x10, mPRGData, mCHRData, mVRamMirrorRom, &mVRamMirrorMapped);
    }
}


// If rom has SRam - save state on exit
void Rom::SaveGameState()
{
    if (mSRamBattery)
    {
        FILE* savegame = fopen(mSavename.c_str(), "w");
        if (savegame != NULL)
        {
            fwrite(mPRGRam, 1, 0x2000, savegame);
            fclose(savegame);
        }
    }
}
