
/*
 * Timer.c
 *
 * Created: 2014-11-01 18:35:13
 *  Author: m
 */ 

#include "TimerDriver.h"

void TimerDriver::SetClockSource(ClockSource source)
{
  switch(source){
    case Internal:
      _clockSource = source;
      break;
    case External:
    default:
    
      break;
  }
}

void TimerDriver::SetOverflowInterval(NANOSECONDS time)
{
  uint64_t cpuPeriod = (uint64_t)1000000000 / (uint64_t)F_CPU;
  uint64_t cpuPeriodsNumber = (uint64_t)time / ((uint64_t)cpuPeriod);
  
  if(cpuPeriodsNumber >= ((uint64_t)1 << (COUNTER_RESOLUTION + 10))){
    return; // nah
    
  } else if(cpuPeriodsNumber >= ((uint64_t)1 << (COUNTER_RESOLUTION + 8))){
    
    _prescalerValue = TwoToThePowerOfTen;
    _counterClkPeriod =  cpuPeriod * ((uint64_t)1 << (10));
    
  } else if(cpuPeriodsNumber >= ((uint64_t)1 << (COUNTER_RESOLUTION + 6))){
    
    _prescalerValue = TwoToThePowerOfEight;
    _counterClkPeriod = cpuPeriod * ((uint64_t)1 << (8));
    
  } else if(cpuPeriodsNumber >= ((uint64_t)1 << (COUNTER_RESOLUTION + 3))){
    
    _prescalerValue = TwoToThePowerOfSix;
    _counterClkPeriod = cpuPeriod * ((uint64_t)1 << (6));
    
  } else if(cpuPeriodsNumber >= ((uint64_t)1 << (COUNTER_RESOLUTION))){
    
    _prescalerValue = TwoToThePowerOfThree;
    _counterClkPeriod = cpuPeriod * ((uint64_t)1 << (3));
    
  } else {

    _prescalerValue = NoPrescaling;
    _counterClkPeriod = cpuPeriod * ((uint64_t)1 << (0));
    
  }

  _overflowTimeout = time;  
}

void TimerDriver::Start()
{
  _ComputeCotrolRegisters();
  _ComputeInterruptsRegisters();
  
  _UpdateControlRegisters();
  _UpdateOutputCompareRegisters();
  _UpdateInterruptsRegisters();
}

void TimerDriver::_UpdateOutputCompareRegisters(){
  OCR0A = _outputCompareA;
  OCR0B = _outputCompareB;
}

void TimerDriver::_ComputeInterruptsRegisters(){
  _interruptRegister = 0;
  _interruptMaskRegister = 0;
  _ComputeInterruptMaskRegister();
  _ComputeInterruptRegister();
}

void TimerDriver:: _ComputeInterruptRegister()
{
  BYTE interruptRegisterMask = 
      NUMBER2MASK(OCF0A)
      | NUMBER2MASK(OCF0B)
      | NUMBER2MASK(TOV0);
  BYTE interruptRegisterVal =
      NUMBER2MASK(OCF0A);
      //| NUMBER2MASK(OCF0B)
      //| NUMBER2MASK(TOV0)
  _interruptRegister = CP_BITS(
      interruptRegisterVal
      ,_interruptRegister
      ,interruptRegisterMask
    );
}

void TimerDriver::_ComputeInterruptMaskRegister()
{
  BYTE interruptMaskRegisterMask =
      NUMBER2MASK(OCIE0A)
      | NUMBER2MASK(OCIE0B)
      | NUMBER2MASK(TOIE0);
  BYTE interruptMaskRegisterVal =
      NUMBER2MASK(OCIE0A);
      //| NUMBER2MASK(OCIE0B)
      //| NUMBER2MASK(TOIE0);
  _interruptMaskRegister = CP_BITS(
      interruptMaskRegisterVal
      ,_interruptMaskRegister
      ,interruptMaskRegisterMask
    );
}


void TimerDriver::_ComputeCotrolRegisters()
{
  _controlRegister[0]=0;
  _controlRegister[1]=0;
  _ComputeOutputCompareModeRegisters();
  _ComputeOutputCompareRegisters();
  _ComputeClockSourceRegister();
  _ComputeWaveFormGenerationModeRegister();
}

void TimerDriver::_ComputeWaveFormGenerationModeRegister(){
  BYTE regAMask = NUMBER2MASK(WGM00) | NUMBER2MASK(WGM01);
  BYTE regBMask = NUMBER2MASK(WGM02);
  BYTE regAVal = (1<<WGM01);
  BYTE regBVal = 0;
  _controlRegister[0] = CP_BITS(regAVal,_controlRegister[0],regAMask);
  _controlRegister[1] = CP_BITS(regBVal,_controlRegister[1],regBMask);
}

void TimerDriver::_ComputeClockSourceRegister(){
  BYTE mask = NUMBER2MASK(CS00) | NUMBER2MASK(CS01) | NUMBER2MASK(CS02);
  BYTE val = 0;
  if (_clockSource == Internal)
    val = _prescalerValue;
   
  _controlRegister[1] = CP_BITS(val,_controlRegister[1],mask);
}

void TimerDriver::_ComputeOutputCompareRegisters()
{
  _outputCompareA =  _overflowTimeout / _counterClkPeriod;
  _outputCompareB = 0x0;
}

void TimerDriver::_ComputeOutputCompareModeRegisters()
{
  _ComputeOutputCompareAControlRegister();
  _ComputeOutputCompareBControlRegister();

}

void TimerDriver::_ComputeOutputCompareAControlRegister()
{
  BYTE mask = NUMBER2MASK(COM0A0) | NUMBER2MASK(COM0A1);
  BYTE val = 0;
  _controlRegister[0] = CP_BITS(val,_controlRegister[0],mask);
}

void TimerDriver::_ComputeOutputCompareBControlRegister()
{
  BYTE mask = NUMBER2MASK(COM0B0) | NUMBER2MASK(COM0B1);
  BYTE val = 0;
  _controlRegister[0] = CP_BITS(val,_controlRegister[0],mask);
}

void TimerDriver::_UpdateControlRegisters(){
  TCCR0A = _controlRegister[0];
  TCCR0B = _controlRegister[1];
}

void TimerDriver::_UpdateInterruptsRegisters(){
  TIFR0 = _interruptRegister;
  TIMSK0 = _interruptMaskRegister;
}

TimerDriver::TimerDriver(){
  
}

TimerDriver::~TimerDriver(){
  
}