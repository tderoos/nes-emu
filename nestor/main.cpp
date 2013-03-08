//
//  main.cpp
//  nestor
//
//  Created by Tommy de Roos on 03/03/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include "Rom.h"
#include "Ram.h"
#include "IO.h"
#include "CPU6502.h"
#include "PPU2C07.h"

int main(int argc, const char * argv[])
{

    // insert code here...
    std::cout << "Hello, World!\n";

//    Rom rom("/Users/tderoos/Projects/nestor/smb1.nes");
    Rom rom("/Users/tderoos/Projects/nestor/nestest.nes");

    
//    Rom rom("/Users/tderoos/Projects/nestor/ehbasic.rom");
    Ram ram(0x8000);
    PPU2C07 ppu;

    IO io(&ram, &rom, &ppu);
    CPU6502 cpu(&io);
//    cpu.mRegs.mPC = 0xFF00;
    cpu.mRegs.mPC = 0xC004;

//    Rom rom2("/Users/tderoos/Projects/nestor/smb1.nes");
//    //    Rom rom("/Users/tderoos/Projects/nestor/ehbasic.rom");
//    Ram ram2(0x8000);
//    PPU2C07 ppu2;
//    
//    IO io2(&ram2, &rom2, &ppu2);
//    CPU6502 cpu2(&io2);
//    cpu2.mOldMode = true;
    //    cpu.mRegs.mPC = 0xFF00;

    uint16_t clock = 0;
    
    while (true)
    {
        cpu.Tick();
        io.Tick();
        
        if (clock > 113)
        {
            ppu.Scanline();
            io.SetNMI(ppu.GetNMI());
            clock -= 113;
        }
        
        clock++;

//        cpu2.Tick();
//        ppu2.Tick();
//        
//        if (memcmp(&cpu.mRegs, &cpu2.mRegs, sizeof(CPU6502::Status)))
//        {
//            int i = 0;
//        }
    }

    return 0;
}
