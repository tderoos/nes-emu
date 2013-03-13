//
//  Mapper.h
//  nestor
//
//  Created by Tommy de Roos on 3/12/13.
//
//

#ifndef nestor_Mapper_h
#define nestor_Mapper_h

#include "types.h"

class Mapper
{
public:
    static Mapper* sCreate(UInt8 inType, UInt8 inNumPRG, UInt8 inNumCHR);

    virtual UInt32  Map(UInt16 inAddr) const = 0;
    virtual void    Store(UInt16 inAddr, UInt8 inData) = 0;
    
    virtual UInt8   GetPRGBankIdx() const = 0;
};

#endif
