// nes-emu CPU module
#pragma once

#include "common/types.h"

// Forward declarations
class IO;
class SaveState;

void BREAK();

class CPU6502
{

public:
	CPU6502(IO* inIO);
	void Tick(uint16 inPPUClock, int16 inScanline);

	// State saving
	void ReadState(const SaveState& ioState);
	void WriteState(SaveState& ioState) const;

	enum ERegister
	{
		SP = 0,
		A,
		X,
		Y,
		ZERO
	};

	struct Status
	{
		uint16 mPC = 0;

		union
		{
			struct
			{
				uint8 mSP;
				uint8 mA;
				uint8 mX;
				uint8 mY;
				uint8 mRegZero;
			};
			uint8 mReg[5] = {};
		};

		union
		{
			struct
			{
				uint8 mCarry : 1;
				uint8 mZero : 1;
				uint8 mInterrupt : 1;
				uint8 mDecimal : 1;
				uint8 mBreak : 1;
				uint8 mReserved : 1;
				uint8 mOverflow : 1;
				uint8 mNeg : 1;
			};
			uint8 mFlags = 0;
		};
	};

private:
	uint8  Handle00(uint8 opcode);
	uint8  Handle01(uint8 opcode);
	uint8  Handle10(uint8 opcode);
	uint8  Handle11(uint8 opcode);

	bool   mNMI;

	uint8  mInstrTimer;
	Status mRegs;
	IO*	   mIO;
};
