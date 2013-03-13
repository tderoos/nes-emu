//
// Created by tderoos on 3/3/13.
//
// To change the template use AppCode | Preferences | File Templates.
//


#include "CPU6502.h"
#include "IO.h"



// Debugging
struct DebugTrace
{
    uint16_t mAddr;
    uint8_t  mOp;
};
DebugTrace  sTrace[256];
int         sTraceHead=0;


void BREAK()
{
    int i;
    for (i = 0; i < 256; ++i)
    {
        int idx = (sTraceHead + i) % 256;
        printf("0x%4x $%2x\n", sTrace[idx].mAddr, sTrace[idx].mOp);
    }
    
    i=0;
}




uint16_t ReadAddr(uint16_t inAddr, IO* ioIO)
{
    uint8_t addr_lo, addr_hi;
    ioIO->Load(inAddr, &addr_lo);
    ioIO->Load(inAddr+1, &addr_hi);

    return (addr_hi << 8) + addr_lo;
}

uint16_t ReadAddrWrapped(uint16_t inAddr, IO* ioIO)
{
    uint8_t addr_lo, addr_hi;
    ioIO->Load(inAddr, &addr_lo);
    
    inAddr = (0xFF00 & inAddr) | (0x00FF & (inAddr+1));
    ioIO->Load(inAddr, &addr_hi);
    
    return (addr_hi << 8) + addr_lo;
}

uint16_t GetEAAbs(CPU6502::Status& ioStatus, IO* ioIO, CPU6502::ERegister inIndex)
{
    uint16_t addr = ReadAddr(ioStatus.mPC+1, ioIO);
    return addr + ioStatus.mReg[inIndex];
}

uint16_t GetEAZpg(CPU6502::Status& ioStatus, IO* ioIO, CPU6502::ERegister inIndex)
{
    uint8_t oper_offset;
    ioIO->Load(ioStatus.mPC+1, &oper_offset);

    uint16_t index_offset = ioStatus.mReg[inIndex];
    return (index_offset + oper_offset) & 0xFF;
}

uint16_t GetEAZpgInd(CPU6502::Status& ioStatus, IO* ioIO, CPU6502::ERegister inIndex)
{
    uint8_t zpg_offset;
    ioIO->Load(ioStatus.mPC+1, &zpg_offset);
    
    uint16_t pre_inc  = (inIndex == CPU6502::X ? ioStatus.mReg[CPU6502::X] : 0);
    uint16_t post_inc = (inIndex == CPU6502::Y ? ioStatus.mReg[CPU6502::Y] : 0);
    
    uint8_t addr_lo, addr_hi;
    ioIO->Load((zpg_offset + pre_inc    ) & 0xFF, &addr_lo);
    ioIO->Load((zpg_offset + pre_inc + 1) & 0xFF, &addr_hi);

    return (addr_hi << 8) + addr_lo + post_inc;
}

uint16_t GetEAInd(CPU6502::Status& ioStatus, IO* ioIO)
{
    uint16_t addr = ReadAddr(ioStatus.mPC+1, ioIO);
    return ReadAddr(addr, ioIO);
}



int8_t ReadOffsetSigned(uint16_t inAddr, IO* ioIO)
{
    int8_t offset;
    ioIO->Load(inAddr, (uint8_t*) &offset);
    return offset;
}



uint8_t LoadOper(uint16_t inAddr, IO* ioIO)
{
    uint8_t value;
    ioIO->Load(inAddr, &value);
    return value;
}

// Addressing
uint8_t LoadOperImm(CPU6502::Status& ioStatus, IO* ioIO)
{
    return LoadOper(ioStatus.mPC+1, ioIO);
}

uint8_t LoadOperZpg(CPU6502::Status& ioStatus, IO* ioIO, CPU6502::ERegister inIndex)
{
    return LoadOper(GetEAZpg(ioStatus, ioIO, inIndex), ioIO);
}

uint8_t LoadOperAbs(CPU6502::Status& ioStatus, IO* ioIO, CPU6502::ERegister inIndex)
{
    return LoadOper(GetEAAbs(ioStatus, ioIO, inIndex), ioIO);
}

uint8_t LoadOperInd(CPU6502::Status& ioStatus, IO* ioIO, CPU6502::ERegister inIndex)
{
    return LoadOper(GetEAZpgInd(ioStatus, ioIO, inIndex), ioIO);
}




void SetRegister(CPU6502::ERegister inDst, uint8_t inValue, CPU6502::Status& ioStatus)
{
    ioStatus.mReg[inDst] = inValue;

    if (inDst != CPU6502::SP)
    {
        ioStatus.mZero = inValue          == 0;
        ioStatus.mNeg  = (inValue & 0x80) != 0;
    }
}


// Stack
void Push(uint8_t inValue, CPU6502::Status& ioStatus, IO* ioIO)
{
    ioIO->Store(0x0100 + ioStatus.mSP--, inValue);
}

void PushAddr(uint16_t inAddr, CPU6502::Status& ioStatus, IO* ioIO)
{
    Push((inAddr >> 8) & 0xFF, ioStatus, ioIO);
    Push( inAddr       & 0xFF, ioStatus, ioIO);
}


uint8_t Pull(CPU6502::Status& ioStatus, IO* ioIO)
{
    uint8_t value;
    ioIO->Load(0x100 + ++ioStatus.mSP, &value);
    return value;
}

void PullPC(CPU6502::Status& ioStatus, IO* ioIO)
{
    uint16_t pc_lo = Pull(ioStatus, ioIO);
    uint16_t pc_hi = Pull(ioStatus, ioIO);
    ioStatus.mPC = pc_lo | pc_hi << 8;
}



// Operations
void OppADDC(uint8_t inValue, CPU6502::Status& ioStatus)
{
    uint8_t value = ioStatus.mReg[CPU6502::A] + inValue;
    ioStatus.mCarry = inValue != 0 && value <= ioStatus.mReg[CPU6502::A];
    SetRegister(CPU6502::A, value, ioStatus);
}



void OppBit(uint8_t inValue, CPU6502::Status& ioStatus)
{
    ioStatus.mOverflow = (inValue & 0x40) != 0;
    ioStatus.mNeg      = (inValue & 0x80) != 0;
    ioStatus.mZero     = (inValue & ioStatus.mReg[CPU6502::A]) == 0;
}


uint8_t OppDec(uint8_t inValue, CPU6502::Status& ioStatus)
{
    uint8_t result = inValue-1;
    
    ioStatus.mZero = result == 0;
    ioStatus.mNeg  = (result & 0x80) != 0;
    
    return result;
}


uint8_t OppInc(uint8_t inValue, CPU6502::Status& ioStatus)
{
    uint8_t result = inValue+1;
    
    ioStatus.mZero = result == 0;
    ioStatus.mNeg  = (result & 0x80) != 0;
    
    return result;
}



void OppBRK(CPU6502::Status& ioStatus, IO* ioIO)
{
    PushAddr(ioStatus.mPC+1, ioStatus, ioIO);
    ioStatus.mBreak = 1;
    Push(ioStatus.mFlags, ioStatus, ioIO);
    ioStatus.mInterrupt = 1;
    ioStatus.mPC = ReadAddr(0xFFFE, ioIO);
}

void OppCmp(CPU6502::ERegister inRegister, uint8_t inValue, CPU6502::Status& ioStatus)
{
    uint8_t reg = ioStatus.mReg[inRegister];
    uint8_t cmp = (~inValue)+1;
    uint8_t delta = reg + cmp;

    ioStatus.mZero  = (delta == 0);
    ioStatus.mNeg   = (delta & 0x80) != 0;
    ioStatus.mCarry = (delta <= reg);
}


void OppEOR(uint8_t inValue, CPU6502::Status& ioStatus)
{
    uint8_t value = ioStatus.mReg[CPU6502::A] | inValue;
    SetRegister(CPU6502::A, value, ioStatus);
}



uint8_t OppROR(uint8_t inValue, CPU6502::Status& ioStatus)
{
    uint8_t result  = (inValue >> 1) + (ioStatus.mCarry ? 0x80 : 0x00);

    ioStatus.mZero  = result == 0;
    ioStatus.mCarry = (inValue&0x01) != 0;
    ioStatus.mNeg   = (result&0x80)  != 0;

    return result;
}

uint8_t OppROL(uint8_t inValue, CPU6502::Status& ioStatus)
{
    uint8_t result  = (inValue << 1) + (ioStatus.mCarry ? 0x01 : 0x00);

    ioStatus.mZero  = result == 0;
    ioStatus.mCarry = (inValue&0x80) != 0;
    ioStatus.mNeg   = (result&0x80)  != 0;

    return result;
}

uint8_t OppLSR(uint8_t inValue, CPU6502::Status& ioStatus)
{
    uint8_t result  = (inValue >> 1);
    
    ioStatus.mZero  = result == 0;
    ioStatus.mCarry = (inValue&0x01) != 0;
    
    return result;
}

uint8_t OppASL(uint8_t inValue, CPU6502::Status& ioStatus)
{
    uint8_t result  = (inValue << 1);
    
    ioStatus.mZero  = result == 0;
    ioStatus.mCarry = (inValue&0x80) != 0;
    ioStatus.mNeg   = (result&0x80)  != 0;
    
    return result;
}



void OppORA(uint8_t inValue, CPU6502::Status& ioStatus)
{
    uint8_t value = ioStatus.mReg[CPU6502::A] | inValue;
    SetRegister(CPU6502::A, value, ioStatus);
}

void OppADC(uint8_t inValue, CPU6502::Status& ioStatus)
{
    uint16_t sum = ioStatus.mAcc + inValue + (ioStatus.mCarry ? 1 : 0);

    // Set overflow if
    //  - Acc and operand are same sign
    //  - and Acc and sum are different sign
    ioStatus.mOverflow  = !((ioStatus.mAcc ^ inValue) & 0x80) &&
                           ((ioStatus.mAcc ^ sum)     & 0x80);
    ioStatus.mCarry     = (sum > 0xFF);

    SetRegister(CPU6502::A, (uint8_t)(sum & 0xFF), ioStatus);
}

void OppSBC(uint8_t inValue, CPU6502::Status& ioStatus)
{
    uint16_t sum = ioStatus.mAcc - inValue - (ioStatus.mCarry ? 0 : 1);
    
    // Set overflow if
    //  - Acc and operand are different sign
    //  - and Acc and sum are different sign
    ioStatus.mOverflow  = ((ioStatus.mAcc ^ inValue) & 0x80) &&
                          ((ioStatus.mAcc ^ sum)     & 0x80);
    ioStatus.mCarry     = (sum < 0x100);
    SetRegister(CPU6502::A, (uint8_t)(sum & 0xFF), ioStatus);
    ioStatus.mNeg       = (sum & 0x8000) != 0;
}


void OppIncMem(uint16_t inAddr, CPU6502::Status& ioStatus, IO* ioIO)
{
    uint8_t value;
    ioIO->Load(inAddr, &value);
    value = value+1;
    ioIO->Store(inAddr, value);

    ioStatus.mZero = value == 0;
    ioStatus.mNeg  = (value & 0x80) != 0;
}

void OppDecMem(uint16_t inAddr, CPU6502::Status& ioStatus, IO* ioIO)
{
    uint8_t value;
    ioIO->Load(inAddr, &value);
    value = value-1;
    ioIO->Store(inAddr, value);

    ioStatus.mZero = value == 0;
    ioStatus.mNeg  = (value & 0x80) != 0;
}


CPU6502::CPU6502(IO* inIO) :
    mIO(inIO)
{
    mRegs.mFlags    = 0;
    mRegs.mReserved = 1;
    mRegs.mPC       = 0x8000;
    mRegs.mRegZero  = 0;
    mRegs.mSP       = 0xFF;
    mRegs.mAcc      = 0;
    mRegs.mX        = 0;
    mRegs.mY        = 0;
}


void CPU6502::Handle00(uint8_t opcode)
{
    if (opcode == 0x00) // BRK
    {
        PushAddr(mRegs.mPC+1, mRegs, mIO);
        mRegs.mBreak = 1;
        Push(mRegs.mFlags, mRegs, mIO);
        mRegs.mInterrupt = 1;
        mRegs.mPC = ReadAddr(0xFFFE, mIO);

        return;
    }
    
    if (opcode == 0x20) // JSR
    {
        PushAddr(mRegs.mPC+2, mRegs, mIO);
        mRegs.mPC = ReadAddr(mRegs.mPC+1, mIO);
        return;
    }
    
    if (opcode == 0x40) // RTI
    {
        mRegs.mFlags = Pull(mRegs, mIO);
        PullPC(mRegs, mIO);
        return;
    }

    if (opcode == 0x60) // RTS
    {
        PullPC(mRegs, mIO);
        mRegs.mPC += 1;
        return;
    }
    
    // Handle jump
    if (opcode == 0x4C)
    {
        mRegs.mPC = GetEAAbs(mRegs, mIO, CPU6502::ZERO);
        return;
    }
    
    if (opcode == 0x6C)
    {
        uint16_t addr = ReadAddr(mRegs.mPC+1, mIO);
        mRegs.mPC = ReadAddrWrapped(addr, mIO);
        return;
    }
    
    // Branches
    if ((opcode & 0x1F) == 0x10)
    {
        uint8_t cmp  = (opcode >> 5) & 0x01;
        uint8_t flag;
        switch(opcode >> 6)
        {
            case 0: flag = mRegs.mNeg;      break;
            case 1: flag = mRegs.mOverflow; break;
            case 2: flag = mRegs.mCarry;    break;
            case 3: flag = mRegs.mZero;     break;
        }

        if (cmp == flag)
            mRegs.mPC += ReadOffsetSigned(mRegs.mPC+1, mIO);

        mRegs.mPC += 2;
        return;
    }
    
    // 0x?8
    if ((opcode & 0x0F) == 0x08)
    {
        switch (opcode >> 4)
        {
            case 0x1: mRegs.mCarry = 0;                                     break;        // CLC
            case 0x3: mRegs.mCarry = 1;                                     break;        // SEC
            case 0x5: mRegs.mInterrupt = 0;                                 break;        // CLI
            case 0x7: mRegs.mInterrupt = 1;                                 break;        // SEI
            case 0xB: mRegs.mOverflow = 0;                                  break;        // CLV
            case 0xD: mRegs.mDecimal = 0;                                   break;        // CLD
            case 0xF: mRegs.mDecimal = 1;                                   break;        // SED

            case 0x0: Push(mRegs.mFlags|0x30, mRegs, mIO);                  break;        // PHP
            case 0x2: mRegs.mFlags = Pull(mRegs, mIO);                      break;        // PLP
            case 0x4: Push(mRegs.mAcc, mRegs, mIO);                         break;        // PHA
            case 0x6: SetRegister(CPU6502::A, Pull(mRegs, mIO), mRegs);     break;        // PLA

            case 0x8: SetRegister(CPU6502::Y, mRegs.mY-1, mRegs);           break;        // DEY
            case 0x9: SetRegister(CPU6502::A, mRegs.mY, mRegs);             break;        // TYA
            case 0xA: SetRegister(CPU6502::Y, mRegs.mAcc, mRegs);           break;        // TAY
            case 0xC: SetRegister(CPU6502::Y, mRegs.mY+1, mRegs);           break;        // INY
            case 0xE: SetRegister(CPU6502::X, mRegs.mX+1, mRegs);           break;        // INX
        }

        mRegs.mPC += 1;
        return;
    }
    
    // Else decode
    uint8_t am = (opcode >> 2) & 7;
    uint8_t op = (opcode >> 5) & 7;
    
    uint16_t ea;
    uint8_t oper;
    
    switch (am)
    {
        case 0: oper = LoadOperImm(mRegs, mIO);            break;              // #imm
        case 1: ea = GetEAZpg(mRegs, mIO, CPU6502::ZERO);  break;              // zpg
        case 3: ea = GetEAAbs(mRegs, mIO, CPU6502::ZERO);  break;              // abs
        case 5: ea = GetEAZpg(mRegs, mIO, CPU6502::X);     break;              // zpg,X
        case 7: ea = GetEAAbs(mRegs, mIO, CPU6502::X);     break;              // abs,X
            
        default:
            ea = -1; oper = 0;
            BREAK();
    }
    
    if (am != 0 && op != 4)
        mIO->Load(ea, &oper);

    switch (op)
    {
        case 1: OppBit(oper, mRegs);                        break;              // BIT
        case 4: mIO->Store(ea, mRegs.mY);                   break;              // STY
        case 5: SetRegister(CPU6502::Y, oper, mRegs);       break;              // LDY
        case 6: OppCmp(CPU6502::Y, oper, mRegs);            break;              // CPY
        case 7: OppCmp(CPU6502::X, oper, mRegs);            break;              // CPX

        default:
            BREAK();
    }
    
    uint8_t instr_size[] = { 2, 2, 0, 3, 0, 2, 0, 3 };
    mRegs.mPC += instr_size[am];
}


void CPU6502::Handle01(uint8_t opcode)
{
    uint8_t am = (opcode >> 2) & 7;
    uint8_t op = (opcode >> 5) & 7;
    
    if (op != 4)
    {
        uint8_t oper;
        
        switch (am)
        {
            case 0: oper = LoadOperInd(mRegs, mIO, CPU6502::X);     break;              // (zpg,X)
            case 1: oper = LoadOperZpg(mRegs, mIO, CPU6502::ZERO);  break;              // zpg
            case 2: oper = LoadOperImm(mRegs, mIO);                 break;              // #imm
            case 3: oper = LoadOperAbs(mRegs, mIO, CPU6502::ZERO);  break;              // abs
            case 4: oper = LoadOperInd(mRegs, mIO, CPU6502::Y);     break;              // (zpg),Y
            case 5: oper = LoadOperZpg(mRegs, mIO, CPU6502::X);     break;              // zpg,X
            case 6: oper = LoadOperAbs(mRegs, mIO, CPU6502::Y);     break;              // abs,Y
            case 7: oper = LoadOperAbs(mRegs, mIO, CPU6502::X);     break;              // abs,X
        }
        
        switch (op)
        {
            case 0: SetRegister(CPU6502::A, oper | mRegs.mAcc, mRegs);  break;          // ORA
            case 1: SetRegister(CPU6502::A, oper & mRegs.mAcc, mRegs);  break;          // AND
            case 2: SetRegister(CPU6502::A, oper ^ mRegs.mAcc, mRegs);  break;          // EOR
            case 3: OppADC(oper, mRegs);                                break;          // ADC
            case 5: SetRegister(CPU6502::A, oper, mRegs);               break;          // LDA
            case 6: OppCmp(CPU6502::A, oper, mRegs);                    break;          // CMP
            case 7: OppSBC(oper, mRegs);                                break;          // SBC
        }
    }
    else
    {
        uint16_t ea;
        switch (am)
        {
            case 0: ea = GetEAZpgInd(mRegs, mIO, CPU6502::X);  break;              // (zpg,X)
            case 1: ea = GetEAZpg(mRegs, mIO, CPU6502::ZERO);  break;              // zpg
            case 2: ea = -1; BREAK();                          break;              // Illegal
            case 3: ea = GetEAAbs(mRegs, mIO, CPU6502::ZERO);  break;              // abs
            case 4: ea = GetEAZpgInd(mRegs, mIO, CPU6502::Y);  break;              // (zpg),Y
            case 5: ea = GetEAZpg(mRegs, mIO, CPU6502::X);     break;              // zpg,X
            case 6: ea = GetEAAbs(mRegs, mIO, CPU6502::Y);     break;              // abs,Y
            case 7: ea = GetEAAbs(mRegs, mIO, CPU6502::X);     break;              // abs,X
        }
        
        mIO->Store(ea, mRegs.mAcc);
    }
    
    uint8_t instr_size[] = { 2, 2, 2, 3, 2, 2, 3, 3 };
    mRegs.mPC += instr_size[am];
}


void CPU6502::Handle10(uint8_t opcode)
{
    // Handle single byte instructions
    if ((opcode & 0x8F) == 0x8A)
    {
        switch(opcode >> 4)
        {
            case 0x8: SetRegister(CPU6502::A,  mRegs.mX,   mRegs); break;  // TXA
            case 0x9: SetRegister(CPU6502::SP, mRegs.mX,   mRegs); break;  // TXS
            case 0xA: SetRegister(CPU6502::X,  mRegs.mAcc, mRegs); break;  // TAX
            case 0xB: SetRegister(CPU6502::X,  mRegs.mSP,  mRegs); break;  // TSX
            case 0xC: SetRegister(CPU6502::X,  mRegs.mX-1, mRegs); break;  // DEX
            case 0xE:                                              break;  // NOP
                
            default:
                BREAK();
        }
        
        mRegs.mPC+=1;
        return;
    }
    
    // Else decode
    uint8_t am = (opcode >> 2) & 7;
    uint8_t op = (opcode >> 5) & 7;
   
    uint16_t ea;
    uint8_t oper;
    
    // Select offset reg - normally X, except LDX and STX
    ERegister offset_reg = ((op&6)==4) ? CPU6502::Y : CPU6502::X;

    switch (am)
    {
        case 0: oper = LoadOperImm(mRegs, mIO);            break;              // #imm
        case 1: ea = GetEAZpg(mRegs, mIO, CPU6502::ZERO);  break;              // zpg
        case 2: oper = mRegs.mAcc;                         break;              // acc
        case 3: ea = GetEAAbs(mRegs, mIO, CPU6502::ZERO);  break;              // abs
        case 5: ea = GetEAZpg(mRegs, mIO, offset_reg);     break;              // zpg,X / apg,Y (STX&LDX)
        case 7: ea = GetEAAbs(mRegs, mIO, offset_reg);     break;              // abs,X / abs,Y
            
        default:
            ea = -1; oper = 0;
            BREAK();
    }
    
    if (am != 0 && am != 2 && op != 4)
        mIO->Load(ea, &oper);
    
    switch (op)
    {
        case 0: oper = OppASL(oper, mRegs);                 break;              // ASL
        case 1: oper = OppROL(oper, mRegs);                 break;              // ROL
        case 2: oper = OppLSR(oper, mRegs);                 break;              // LSR
        case 3: oper = OppROR(oper, mRegs);                 break;              // ROR
        case 4: oper = mRegs.mX;                            break;              // STX
        case 5: SetRegister(CPU6502::X, oper, mRegs);       break;              // LDX
        case 6: oper = OppDec(oper, mRegs);                 break;              // DEC
        case 7: oper = OppInc(oper, mRegs);                 break;              // INC
    }
    
    switch (am)
    {
        case 2:
            SetRegister(CPU6502::A, oper, mRegs);
            break;
            
        case 1:
        case 3:
        case 5:
        case 7:
            if (op != 5)
                mIO->Store(ea, oper);
            break;
    }
    
    uint8_t instr_size[] = { 2, 2, 1, 3, 0, 2, 0, 3 };
    mRegs.mPC += instr_size[am];
}


void CPU6502::Tick()
{
    bool irq = false;
    
    static bool tick = false;
    
    if ((mIO->NMI() || irq || mIO->Reset()))
    {
        if (!mIO->Reset())
        {
            PushAddr(mRegs.mPC, mRegs, mIO);
            Push(mRegs.mFlags, mRegs, mIO);
        }
        
        uint16_t addr = mIO->Reset() ? 0xFFFC :
                        mIO->NMI()   ? 0xFFFA : 0xFFFE;
        
        mRegs.mPC = ReadAddr(addr, mIO);
        mRegs.mBreak = 0;
       
        tick = false;
    }
    
    tick = !tick;
    if (!tick)
        return;

//    if (mRegs.mPC == 0x8002)
//        BREAK();
    
    uint8_t opcode;
    mIO->Load(mRegs.mPC, &opcode);
    
    sTrace[sTraceHead].mAddr = mRegs.mPC;
    sTrace[sTraceHead].mOp   = opcode;
    sTraceHead = (sTraceHead+1) % 256;
    
    switch(opcode & 0x03)
    {
        case 0x00:          Handle00(opcode); break;
        case 0x01:          Handle01(opcode); break;
        case 0x02:          Handle10(opcode); break;
            
        default:
            BREAK();
    }
}