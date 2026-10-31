// nes-emu APU Module
#include "common/types.h"
#include "SaveState.h"

#define SETBIT(reg, bit, value)                                                                                        \
	{                                                                                                                  \
		reg &= ~(1 << (bit));                                                                                          \
		reg |= (value) << (bit);                                                                                       \
	}
#define GETBIT(reg, bit) ((reg) >> (bit) & 1)

class APU
{
public:
	APU();
	void   SetAudioBuffer(uint8* ioAudioBuffer);
	uint32 GetSampleCount() const { return mAudioBufferOffset; }

	void	Tick();

	// Access
	void Load(uint16 inAddr, uint8* outValue) const;
	void Store(uint16 inAddr, uint8 inValue);

	// Interrupt
	bool GetInterrupt() const { return mInterrupt; }
	void SetInterrupt() { mInterrupt = true; }
	void ClearInterrupt() { mInterrupt = false; }

	// State saving
	void ReadState(const SaveState& ioState);
	void WriteState(SaveState& ioState) const;

private:
	void UpdateSequencer();
	void ClockEnvelope();
	void ClockLength();
	void UpdateDAC();

	struct Channel
	{
		uint8  mRegister[4] = {};

		uint16 Period() const { return mRegister[2] | (mRegister[3] & 0x07) << 8; }
	};

	struct EnvelopeGenerator
	{
		void Tick();
		void Set(uint8 inValue)
		{
			mRegister = inValue;
			mReset = true;
		}
		float Volume() const { return (float)(GETBIT(mRegister, 4) ? mRegister & 0x0F : mValue) / 15.0f; }

		uint8 mRegister = 0;
		uint8 mDivider = 0;
		uint8 mValue = 0;
		bool  mReset = false;
	};

	struct SweepUnit
	{
		void Tick(int inIdx, uint16& ioPeriod);
		void Set(uint8 inValue)
		{
			mRegister = inValue;
			mReset = true;
		}

		uint8 mRegister = 0;
		uint8 mDivider = 0;
		bool  mReset = false;
	};

	struct Square
	{
		Square(uint8 inIdx) : mIdx(inIdx) {}

		void SetLengthCtrEnabled(bool inValue)
		{
			mLengthEnabled = inValue;
			if (!inValue)
				mLength = 0;
		}
		bool			  GetLengthCtrEnabled() const { return mLengthEnabled; }

		void			  Store(uint8 inAddr, uint8 inValue);

		void			  ClockLength();
		float			  ClockDAC();

		EnvelopeGenerator mEnvelope;
		SweepUnit		  mSweep;

		uint8			  mIdx = 0;
		uint16			  mPeriod = 0;
		bool			  mLengthEnabled = false;
		uint8			  mLength = 0;
		uint8			  mRegisters[2] = {};
		float			  mPhase = 0.0f;
	};
	Square mSquare1;
	Square mSquare2;

	struct Triangle
	{
		Triangle() : mPhase(0.0f) {}

		void SetLengthCtrEnabled(bool inValue)
		{
			mLengthEnabled = inValue;
			if (!inValue)
				mLength = 0;
		}
		bool   GetLengthCtrEnabled() const { return mLengthEnabled; }

		void   Store(uint8 inAddr, uint8 inValue);

		void   ClockLength();
		float  ClockDAC();

		uint16 mPeriod = 0;
		bool   mLengthEnabled = false;
		uint8  mLength = 0;

		uint8  mRegisters[1] = {};
		float  mPhase = 0.0f;
	};
	Triangle mTriangle;

	struct Noise
	{
		void		Reset();
		void		Tick();

		void SetLengthCtrEnabled(bool inValue)
		{
			mLengthEnabled = inValue;
			if (!inValue)
				mLength = 0;
		}
		bool			  GetLengthCtrEnabled() const { return mLengthEnabled; }

		void			  Store(uint8 inAddr, uint8 inValue);

		void			  ClockLength();
		float			  ClockDAC();

		EnvelopeGenerator mEnvelope;

		uint16			  mPeriod = 0;
		bool			  mLengthEnabled = false;
		uint8			  mLength = 0;
		bool			  mShortMode = false;
		uint16			  mNoise = 0;
		uint16			  mDivider = 0;

		uint8			  mRegisters[1] = {};
	};
	Noise mNoise;

	struct DMC
	{
		void  Store(uint8 inAddr, uint8 inValue) {}
		float ClockDAC();
	};
	DMC					 mDMC;

	uint8*				 mAudioBuffer = nullptr;
	uint32				 mAudioBufferOffset = 0;
	uint32				 mDACDivider = 2;
	uint16				 mAPUClock = 1;
	uint8				 mMode = 0;
	uint8				 mSequencerClock = 0;
	mutable bool		 mInterrupt = false;
};
