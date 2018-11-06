/* 
* Serial.h
*
* Created: 2014-11-02 21:51:19
* Author: m
*/


#ifndef __SERIAL_H__
#define __SERIAL_H__

#include "Standarts.h"

class Serial
{
//variables
public:
protected:
private:

//functions
public:
	Serial();
	void SetBaudRate(uint32_t bitsPerSecond);
	void SetDataSize(DataSize size);
	void SetMode(SerialMode mode);
	void SetParity(Parity parity);
	void SetStopBits(StopBits stopBits);
	void SetRxSampleClockEdge(EdgeSelect edge);
	void EnableCommunication();
	void EnableMultiProcessorMode();
	void DisableMultiProcessorMode();
	void EnableIterrupts();
	void Start();
	~Serial();
	
	
	BYTE _controlRegisters[3];
protected:
private:
	uint32_t _baudRate;
	uint8_t _dataSize;
	uint8_t _stopBits;
	bool _parity;
	uint8_t _baudRateRegister[2];

	void _CalculateBaudRateRegister();
	void _Enable2xMode();
	void _Disable2xMode();
}; //Serial

#endif //__SERIAL_H__
