/* 
* Serial.cpp
*
* Created: 2014-11-02 21:51:19
* Author: m
*/


#include "SerialDriver.h"

// default constructor
SerialDriver::SerialDriver()
{
  _controlRegisters[0]=0;
  _controlRegisters[1]=0;
  _controlRegisters[2]=0;
  _baudRate = 0;
} //Serial

// default destructor
SerialDriver::~SerialDriver()
{
} //~Serial

 void SerialDriver::SetDataSize(DataSize size)
{
  BYTE maskB = NUMBER2MASK(UCSZ02);
  BYTE maskC =
    NUMBER2MASK(UCSZ00)
    | NUMBER2MASK(UCSZ01)
  ;
  
  BYTE valB = 0x00;
  BYTE valC = 0x00;
  
  
  if(size == FiveBits){
    valB = 0;
    valC = 0;
    _dataSize = 5;
  } else if (size == SixBits){
    valB = 0;
    valC = (1<<UCSZ00);
    _dataSize = 6;
  } else if (size == SevenBits){
    valB = 0;
    valC = (1<<UCSZ01);
    _dataSize = 7;
  } else if (size == EightBits){
    valB = 0;
    valC = (1<<UCSZ00) | (1<<UCSZ01);
    _dataSize =8;
  } else if (size == NineBits){
    valB = (1<<UCSZ02);
    valC = (1<<UCSZ00) | (1<<UCSZ01);
    _dataSize = 9;
  }

  _controlRegisters[1] = CP_BITS(valB,_controlRegisters[1],maskB);
  _controlRegisters[2] = CP_BITS(valC,_controlRegisters[2],maskC);
  
  
}

void SerialDriver::SetMode(SerialMode mode)
{
  BYTE maskC = (1<<UMSEL00) | (1<<UMSEL01);
  BYTE valC =0;
  
  if(mode == Async){
    valC =0;
  }else if(mode == Sync){
    valC = (1<<UMSEL00);
  }else if(mode == SpiMaster){
    valC = (1<<UMSEL00) | (1<<UMSEL01);
  }
  
  _controlRegisters[2] = CP_BITS(valC,_controlRegisters[2],maskC);
}

void SerialDriver::SetParity(Parity parity)
{
  BYTE maskC = NUMBER2MASK(UPM00) | NUMBER2MASK(UPM01);
  BYTE valC = 0;
  
  if(parity == Disabled){
    valC =0;
    _parity=false;
  } else if (parity == Even) {
    valC = (1<<UPM01);
    _parity = true;
  } else if (parity == Odd) {
    valC = (1<<UPM00) | (1<<UPM01);
    _parity = true;
  }
  
  _controlRegisters[2] = CP_BITS(valC,_controlRegisters[2],maskC);
}

void SerialDriver::SetStopBits(StopBits stopBits)
{
  BYTE maskC = NUMBER2MASK(USBS0);
  BYTE valC = 0;
  
  if(stopBits == One){
    valC = 0;
    _stopBits=1;
  } else if(stopBits == Two){
    valC = (1<<USBS0);
    _stopBits=2;
  }
  
  _controlRegisters[2] = CP_BITS(valC,_controlRegisters[2],maskC);
}

void SerialDriver::SetRxSampleClockEdge(EdgeSelect edge)
{
  BYTE maskC = NUMBER2MASK(UCPOL0);
  BYTE valC = 0;
  
  if(edge == Falling){
    valC = 0;
  } else if(edge == Rising){
    valC = (1<<UCPOL0);
  }
  
  _controlRegisters[2] = CP_BITS(valC,_controlRegisters[2],maskC);
}

void SerialDriver::SendStatusRegisters()
{
  _CalculateBaudRateRegister();
  UCSR0A = _controlRegisters[0];
  UCSR0B = _controlRegisters[1];
  UCSR0C = _controlRegisters[2];
  UBRR0L = _baudRateRegister[0];
  UBRR0H = _baudRateRegister[1];

}

void SerialDriver::EnableCommunication()
{
  _controlRegisters[1] |= (1<<TXEN0) | (1<<RXEN0);
}

void SerialDriver::EnableIterrupts()
{
  EnableRxInterrupt();
  EnableTxInterrupt();
}

void SerialDriver::EnableMultiProcessorMode()
{
  _controlRegisters[0] |= (1<<MPCM0);
}

void SerialDriver::DisableMultiProcessorMode()
{
  _controlRegisters[0] &= (~(1<<MPCM0));
}

void SerialDriver::SetBaudRate(uint32_t bitsPerSecond)
{
  _baudRate = bitsPerSecond;
}

void SerialDriver::_CalculateBaudRateRegister()
{
  uint32_t baudRateRegister = 0;
  uint32_t calculatedBaudRate =0;
  uint32_t error;
  
  baudRateRegister = ((uint32_t)(F_CPU*100) / (16*_baudRate)) -1;
  baudRateRegister = (baudRateRegister % 100) > 50 ? (baudRateRegister / 100)+1 : baudRateRegister / 100;
  
  calculatedBaudRate = (uint32_t)F_CPU / (16 * (baudRateRegister+1));

  if(calculatedBaudRate>baudRateRegister)
    error = ((calculatedBaudRate*128) - (baudRateRegister*128))/baudRateRegister;
  else
    error = ((baudRateRegister*128) - (calculatedBaudRate*128))/baudRateRegister;

  
  if(error > 2*(100/128) || error < -2*(100/128)){
    baudRateRegister = ((uint32_t)F_CPU / (8*_baudRate)) -1;
    _Enable2xMode();
  } else {
    _Disable2xMode();
  }
  
  _baudRateRegister[0] = baudRateRegister;
  _baudRateRegister[1] = (baudRateRegister >> 8);
}

void SerialDriver::_Enable2xMode(){
  _controlRegisters[0] |=  (1<<U2X0);
}

void SerialDriver::_Disable2xMode(){
  _controlRegisters[0] |=  (~(1<<U2X0));
}

void SerialDriver::EnableRxInterrupt()
{
  _controlRegisters[1] |= (1<<RXCIE0);
}

void SerialDriver::EnableTxInterrupt()
{
  _controlRegisters[1] |= (1<<TXCIE0);
}

void SerialDriver::DisableInterrupts()
{
	_controlRegisters[1] = _controlRegisters[1] & (~(NUMBER2MASK(UDRIE0) | NUMBER2MASK(RXCIE0) | NUMBER2MASK(TXCIE0)));
}

