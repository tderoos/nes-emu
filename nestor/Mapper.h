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

    Mapper(UInt8 inNumPRG, UInt8 inNumCHR) :
        mNumPRG(inNumPRG),
        mNumCHR(inNumCHR),
        mDirty(true)                        { }

    inline bool     IsDirty() const         { return mDirty; }
    
    virtual void    Store(UInt16 inAddr, UInt8 inData) = 0;
    virtual void    UpdateMapping(const UInt8* inData, UInt8* ioPRG, UInt8* ioCHR) const = 0;
    
protected:
    UInt8           mNumPRG;
    UInt8           mNumCHR;
    mutable bool    mDirty;
};

#endif
