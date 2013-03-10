//
// Created by tderoos on 3/3/13.
//
// To change the template use AppCode | Preferences | File Templates.
//



#ifndef __Ram_H_
#define __Ram_H_

#include <stdint.h>


class Ram
{
public:

    Ram(uint16_t inSize);

    // Access
    void Load(uint16_t inAddr, uint8_t* outValue) const;
    void Store(uint16_t inAddr, uint8_t inValue);

private:
    uint8_t* mData;
};

#endif //__Ram_H_
