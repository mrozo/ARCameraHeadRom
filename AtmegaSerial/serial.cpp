/* 
* Serial.cpp
*
* Created: 2014-11-02 21:51:19
* Author: m
*/


#include "Serial.h"

// default constructor
Serial::Serial()
{
	_controlRegisters[0]=0;
	_controlRegisters[1]=0;
	_controlRegisters[2]=0;
	_baudRate = 0;
} //Serial

// default destructor
Serial::~Serial()
{
} //~Serial

 void Serial::SetDataSize(DataSize size)
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

void Serial::SetMode(SerialMode mode)
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

void Serial::SetParity(Parity parity)
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

void Serial::SetStopBits(StopBits stopBits)
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

void Serial::SetRxSampleClockEdge(EdgeSelect edge)
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

void Serial::Start()
{
	_CalculateBaudRateRegister();
	UCSR0A = _controlRegisters[0];
	UCSR0B = _controlRegisters[1];
	UCSR0C = _controlRegisters[2];
	UBRR0L = _baudRateRegister[0];
	UBRR0H = _baudRateRegister[1];
}

void Serial::EnableCommunication()
{
	BYTE maskB = NUMBER2MASK(TXEN0) | NUMBER2MASK(RXEN0);
	BYTE valB = (1<<TXEN0) | (1<<RXEN0);
	_controlRegisters[1] = CP_BITS(valB,_controlRegisters[1],maskB);
}

void Serial::EnableIterrupts()
{
	BYTE maskB = NUMBER2MASK(UDRIE0) | NUMBER2MASK(RXCIE0) | NUMBER2MASK(TXCIE0);
	BYTE valB = 0;
	
	//valB = (1<<UDRIE0) | (1<<RXCIE0) | (1<<TXCIE0);
	valB = (1<<TXCIE0) | (1<<RXCIE0) ;
	_controlRegisters[1] = CP_BITS(valB,_controlRegisters[1],maskB);
}

void Serial::EnableMultiProcessorMode()
{
	BYTE maskA = NUMBER2MASK(MPCM0);
	BYTE valA = 0;
	
	_controlRegisters[0] = CP_BITS(valA,_controlRegisters[0],maskA);
}

void Serial::DisableMultiProcessorMode()
{
	BYTE maskA = NUMBER2MASK(MPCM0);
	BYTE valA = (1<<MPCM0);
	
	_controlRegisters[0] = CP_BITS(valA,_controlRegisters[0],maskA);
}

void Serial::SetBaudRate(uint32_t bitsPerSecond)
{
	_baudRate = bitsPerSecond;
}

void Serial::_CalculateBaudRateRegister()
{
	int32_t baudRateRegister = 0;
	int32_t calculatedBaudRate =0;
	
	baudRateRegister = ((uint32_t)(F_CPU*100) / (16*_baudRate)) -1;
	baudRateRegister = (baudRateRegister % 100) > 50 ? (baudRateRegister / 100)+1 : baudRateRegister / 100;
	
	calculatedBaudRate = (uint32_t)F_CPU / (16 * (baudRateRegister+1));
	
	int32_t error = ((calculatedBaudRate*128) - (baudRateRegister*128))/baudRateRegister;
	
	if(error > 2*(100/128) || error < -2*(100/128)){
		baudRateRegister = ((uint32_t)F_CPU / (8*_baudRate)) -1;
		_Enable2xMode();
	} else {
		_Disable2xMode();
	}
	
	_baudRateRegister[0] = baudRateRegister;
	_baudRateRegister[1] = (baudRateRegister >> 8);
}

void Serial::_Enable2xMode(){
	BYTE maskA = NUMBER2MASK(U2X0);
	BYTE valA = (1<<U2X0);
	
	_controlRegisters[0] = CP_BITS(valA,_controlRegisters[0],maskA);
}

void Serial::_Disable2xMode(){
	BYTE maskA = NUMBER2MASK(U2X0);
	BYTE valA = 0;
	
	_controlRegisters[0] = CP_BITS(valA,_controlRegisters[0],maskA);
	
}

