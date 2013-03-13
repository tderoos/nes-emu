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
    mData(NULL)
{
    int fd = -1;
    if ((fd = open(inFilename, O_RDONLY, 0)) != -1)
    {
        struct stat file_stats;
        fstat(fd, &file_stats);
        mData = (uint8_t *) mmap(NULL, file_stats.st_size, PROT_READ, MAP_FILE|MAP_PRIVATE, fd, 0);
        
        mNumPRG = mData[4];
        mNumCHR = mData[5];
        
        mMapperID  = mData[6] >> 4;
        mMapperID |= mData[7] & 0xF0;
        
        mSRam        = (mData[10] & 0x10) != 0;
        mSRamBattery = (mData[6]  & 0x02) != 0;
        
        printf("Loaded %s (PRG:%d CHR:%d MAP:%d SR:%d SRB:%d)\n", inFilename, GetNumPRGBanks(), GetNumCHRBanks(), mMapperID, (int)mSRam, (int)mSRamBattery);
        
        mMapper = Mapper::sCreate(mMapperID, GetNumPRGBanks(), GetNumCHRBanks());
    }
}


char Rom::GetMapperID() const
{
    char low = mData[6] >> 4;
    char hi  = mData[7] >> 4;

    return hi << 4 | low;
}



// Access
void Rom::Load(UInt16 inAddr, UInt8* outValue) const
{
    // Calculate offset - should take bank switching into account for 0x8000 - 0xC000 area
    // 0xC000 should always be mapped to last bank. Current setup works for one or two banks.
    
    UInt32 offset = 0;
    if (mMapper != nullptr)
        offset = mMapper->Map(inAddr);
    
    else
    {
        if (inAddr >= 0xC000)
        {
            offset += (mNumPRG-1) * 0x4000;
            offset += inAddr      - 0xC000;
        }
        else
        {
            int cur_bank = mMapper != nullptr ? mMapper->GetPRGBankIdx() : 0;
            offset += cur_bank  * 0x4000;
            offset += inAddr -    0x8000;
        }
    }
    
    UInt8* addr = mData + offset + 0x0010;
    *outValue = *addr;
}

const UInt8*  Rom::GetCHRData(int inBank) const
{
    UInt16 offset = 0x0010;
    int num_banks = GetNumPRGBanks();

    return mData + offset + num_banks * 0x4000;
}



void Rom::Store(UInt16 inAddr, UInt8 inValue)
{
    if (mMapper != NULL)
        mMapper->Store(inAddr, inValue);
}




