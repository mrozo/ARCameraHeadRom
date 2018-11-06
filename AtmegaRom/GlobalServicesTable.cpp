/*
 * GlobalSystemTable.cpp
 *
 * Created: 2014-11-16 14:46:12
 *  Author: m
 */ 

#include "GlobalServicesTable.h"

TimerEventDispatcher timer;
SerialCommunication serial;

GLOBAL_SERVICES_TABLE gST;

void SerialEventGateway(TIMER_EVENT *e, void *c){
	gST.serial->ManageCommunication(e,c);
}


Status InitGlobalServicesTable(){
	timer = TimerEventDispatcher();
	serial = SerialCommunication();
	timer.CreateEvent(
				Interval,
				MILISECONDS_PERIOD(100),
				SerialEventGateway,
				NULL
			);
	serial.Start();
	gST.timer = & timer;
	gST.serial = & serial;
	
	sei();
	
	return Success;
}

ISR(TIMER0_COMPA_vect){
	gST.timer->SysTimerOverflow();
}
/*
ISR(USART_TX_vect){
	gST.serial->TransmitNextByte();
}

ISR(USART_RX_vect){
	gST.serial->ReceiveData();
}
*/