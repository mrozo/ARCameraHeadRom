#ifndef _TIMER_H_
#define _TIMER_H_
#pragma once

#include "Standarts.h"

#define COUNTER_RESOLUTION 8

class Timer{
	public:
		void SetClockSource(ClockSource source);
		void SetOverflowInterval(NANOSECONDS microSeconds);
		void Start();
		void Stop();
		void ReinitializeTimer();
		Timer();
		~Timer();
		
	private:
		UINT			_TopValue;
		ClockSource		_clockSource;
		PrescalerValues _prescalerValue;
		EdgeSelect		_externalClockActiveEdge;
		BYTE			_controlRegister[2];
		BYTE			_interruptMaskRegister;
		BYTE			_interruptRegister;
		NANOSECONDS		_counterClkPeriod;
		NANOSECONDS		_overflowTimeout;
		uint8_t			_outputCompareA;
		uint8_t			_outputCompareB;
		
		void _ComputeCotrolRegisters();
		void _ComputeClockSourceRegister();
		void _ComputeOutputCompareModeRegisters();
		void _ComputeOutputCompareAControlRegister();
		void _ComputeOutputCompareBControlRegister();
		void _ComputeWaveFormGenerationModeRegister();
		void _UpdateControlRegisters();

		void _ComputeOutputCompareRegisters();
		void _UpdateOutputCompareRegisters();
		
		void _ComputeInterruptsRegisters();
		void _ComputeInterruptMaskRegister();
		void _ComputeInterruptRegister();
		void _UpdateInterruptsRegisters();
		
		void _InterruptHandler();

};

#endif
