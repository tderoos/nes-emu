//
// Created by tderoos on 3/3/13.
//
// To change the template use AppCode | Preferences | File Templates.
//



#ifndef __Rom_H_
#define __Rom_H_

#include <iostream>


class Rom {
public:
    Rom(const char* inFilename);

    // Properties
    char GetNumPRGBanks() const;
    char GetNumCHRBanks() const;
    char GetMapperID() const;

    // Access
    void Load(uint16_t inAddr, uint8_t* outValue) const;
    void Store(uint16_t inAddr, uint8_t inValue);

private:
    uint8_t* mData;
};

#endif //__Rom_H_
