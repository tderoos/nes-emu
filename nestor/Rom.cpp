//
// Created by tderoos on 3/3/13.
//
// To change the template use AppCode | Preferences | File Templates.
//


#include <sys/mman.h>
#include <sys/stat.h>
#include <mm_malloc.h>
#include <fcntl.h>
#include "Rom.h"


Rom::Rom(char const *inFilename) :
    mData(NULL)
{
    int fd = -1;
    if ((fd = open(inFilename, O_RDONLY, 0)) != -1)
    {
        struct stat file_stats;
        fstat(fd, &file_stats);
        mData = (uint8_t *) mmap(NULL, file_stats.st_size, PROT_READ, MAP_FILE|MAP_PRIVATE, fd, 0);
        
        printf("Loaded %s (PRG:%D CHR:%D)\n", inFilename, GetNumPRGBanks(), GetNumCHRBanks());
    }
}


char Rom::GetNumPRGBanks() const
{
    return mData[4];
}

char Rom::GetNumCHRBanks() const
{
    return mData[5];
}

char Rom::GetMapperID() const
{
    char low = mData[6] >> 4;
    char hi  = mData[7] >> 4;

    return hi << 4 | low;
}



// Access
void Rom::Load(uint16_t inAddr, uint8_t* outValue) const
{
    // Calculate offset - should take bank switching into account for 0x8000 - 0xC000 area
    // 0xC000 should always be mapped to last bank. Current setup works for single bank.
    
    uint16_t offset = 0x0010;
    int num_banks = GetNumPRGBanks();
    
    if (inAddr >= 0xC000)
    {
        offset += (num_banks-1) * 0x4000;
        offset += inAddr -    0xC000;
    }
    else
    {
        int cur_bank = 0;
        offset += cur_bank  * 0x4000;
        offset += inAddr -    0x8000;
    }

    uint8_t * addr = mData + offset;
    *outValue = *addr;
}



void Rom::Store(uint16_t inAddr, uint8_t inValue)
{
    // Ignore - bank switching should be triggered here.
}
