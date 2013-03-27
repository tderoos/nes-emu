//
//  Mapper.cpp
//  nestor
//
//  Created by Tommy de Roos on 3/12/13.
//
//

#include "Mapper.h"
#include "NROM.h"
#include "MMC1.h"


Mapper* Mapper::sCreate(uint8 inType, uint8 inNumPRG, uint8 inNumCHR)
{
    switch (inType)
    {
        case 0:
            return new NROM(inNumPRG, inNumCHR);
            
        case 1:
            return new MMC1(inNumPRG, inNumCHR);
    }
    
    return nullptr;
}