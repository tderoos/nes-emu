//
// Created by tderoos on 3/3/13.
//
// To change the template use AppCode | Preferences | File Templates.
//


#include "IO.h"
#include "Rom.h"
#include "Ram.h"
#include "PPU2C07.h"




// Registers
enum
{
    ACIA_RXDATA=0,
    ACIA_STATUS,
    ACIA_COMMAND,
    ACIA_CONTROL,
    ACIA_TXDATA,
    ACIA_LAST
};

// Command register bits
#define ACOMB_DTR    0
#define ACOMF_DTR    (1<<ACOMB_DTR)
#define ACOMB_IRQDIS 1
#define ACOMF_IRQDIS (1<<ACOMB_IRQDIS)
#define ACOMF_TXCON 0x0c
#define ACOMB_ECHO   2
#define ACOMF_ECHO   (1<<ACOMB_ECHO)
#define ACOMF_PARITY 0xe0

// Control register bits
#define ACONF_BAUD   0x0f
#define ACONB_SRC    4
#define ACONF_SRC    (1<<ACONB_SRC)
#define ACONF_WLEN   0x60
#define ACONB_STOPB  7
#define ACONF_STOPB  (1<<ACONB_STOPB)

// Status register bits
#define ASTB_PARITYERR 0
#define ASTF_PARITYERR (1<<ASTB_PARITYERR)
#define ASTB_FRAMEERR  1
#define ASTF_FRAMEERR  (1<<ASTB_FRAMEERR)
#define ASTB_OVRUNERR  2
#define ASTF_OVRUNERR  (1<<ASTB_OVRUNERR)
#define ASTB_RXDATA    3
#define ASTF_RXDATA    (1<<ASTB_RECVDATA)
#define ASTB_TXEMPTY   4
#define ASTF_TXEMPTY   (1<<ASTB_TXEMPTY)
#define ASTB_CARRIER   5
#define ASTF_CARRIER   (1<<ASTB_CARRIER)
#define ASTB_DSR       6
#define ASTF_DSR       (1<<ASTB_DSR)
#define ASTB_IRQ       7
#define ASTF_IRQ       (1<<ASTB_IRQ)

struct acia
{
    struct machine *oric;
    uint8_t regs[ACIA_LAST];
};

acia sAcia;



IO::IO(Ram* inRam, Rom* inRom, PPU2C07* inPPU) :
    mRam(inRam),
    mRom(inRom),
    mPPU(inPPU),
    mReset(true)
{
    sAcia.regs[ACIA_RXDATA]  = 0;
    sAcia.regs[ACIA_TXDATA]  = 0;
    sAcia.regs[ACIA_STATUS]  = ASTF_TXEMPTY;
    sAcia.regs[ACIA_COMMAND] = ACOMF_IRQDIS;
    sAcia.regs[ACIA_CONTROL] = 0;
}

bool start = false;
int rdidx = 0;

void IO::Tick()
{
    mReset = false;
    mNMI   = false;
}



void IO::Load(uint16_t inAddr, uint8_t* outValue)
{
    if (inAddr < 0x2000)
        mRam->Load(inAddr, outValue);

    else if (inAddr < 0x4000)
        mPPU->Load(inAddr, outValue);
    
    // Joystick (just indicate connected for now)
    else if (inAddr == 0x4016)
    {
        *outValue = 0x40;
        
        if (rdidx == 3 && start)
            *outValue+=1;
        
        rdidx++;
//        if (rdidx == 8)
//        {
//            rdidx = 0;
//            start = !start;
//        }
    }
    
    else if (inAddr == 0x4017)
        *outValue = 0x40;
    
    
    else if (inAddr < 0x4020)
    {   // APU
    }
    else if (inAddr < 0x6000)
    {   // EROM
    }
    else if (inAddr < 0x8000)
    {   // SRAM
    }
    else
        mRom->Load(inAddr, outValue);
    
    static bool print = false;
    
    if (print && inAddr >= 0x2000 && inAddr < 0x2003)
        printf("load 0x%x -> $%x\n", inAddr, *outValue);


//    if (inAddr < 0x8000)
//        mRam->Load(inAddr, outValue);
//
//    else if (inAddr > 0xC000)
//        mRom->Load(inAddr, outValue);
//
//    else if (inAddr > 0x8000)
//    {
//        *outValue = sAcia.regs[inAddr&0x3];
//    }
}

bool IO::pushing = false;

void IO::Store(uint16_t inAddr, uint8_t inValue)
{
//    if (inAddr >= 0x2000)
//        printf("store 0x%x <- $%x\n", inAddr, inValue);
    
    if (inAddr < 0x0200 && inAddr >= 0x1F0 && !pushing)
        inAddr = inAddr;
    
    if (inAddr < 0x2000)
        mRam->Store(inAddr, inValue);

    else if (inAddr < 0x4000)
        mPPU->Store(inAddr, inValue);
    
    else if (inAddr == 0x4016)
    {
        if ((inValue&1) == 0)
        {
            start=!start;
            rdidx = 0;
        }
    }
    
    else if (inAddr < 0x4020)
    {   // APU
    }
    else if (inAddr < 0x6000)
    {   // EROM
    }
    else if (inAddr < 0x8000)
    {   // SRAM
    }
    else
        mRom->Store(inAddr, inValue);
    
    
//    if (inAddr < 0x8000)
//        mRam->Store(inAddr, inValue);
//    
//    else if (inAddr > 0xC000)
//        mRom->Store(inAddr, inValue);
//    
//    else if (inAddr > 0x8000)
//    {
//        switch( inAddr & 3 )
//        {
//            case ACIA_RXDATA:      // Data for TX
//                sAcia.regs[ACIA_TXDATA] = inValue;
//                sAcia.regs[ACIA_STATUS] &= ~(ASTF_TXEMPTY);
//                break;
//                
//            case ACIA_STATUS:      // Reset
//                sAcia.regs[ACIA_COMMAND] &= ACOMF_PARITY;
//                sAcia.regs[ACIA_COMMAND] |= ACOMF_IRQDIS;
//                sAcia.regs[ACIA_STATUS]  &= ~(ASTF_OVRUNERR);
//                break;
//                
//            case ACIA_COMMAND:
//                sAcia.regs[ACIA_COMMAND] = inValue;
//                break;
//                
//            case ACIA_CONTROL:
//                sAcia.regs[ACIA_CONTROL] = inValue;
//                break;
//        }
//        
//    }
}
