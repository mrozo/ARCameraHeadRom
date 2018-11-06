/*
 * AtmegaSerial.cpp
 *
 * Created: 2014-11-02 21:05:06
 *  Author: m
 */ 


#include <avr/io.h>
#include <util/delay.h>
#include "Standarts.h"
#include "Serial.h"
char c = 'b';
uint8_t l = 0;

ISR(USART_TX_vect){
	
}

ISR(USART_RX_vect){
	c = UDR0;
	PORTB=0x03;
	UDR0 = c;
	
}

int main(void)
{
	//DDRD = 0xff;
	//PORTD=0x00;
	DDRB = 0x03;
	PORTB = 0x00;

	Serial s = Serial();

	s.SetBaudRate(9600);
	s.SetDataSize(EightBits);
	s.SetMode(Async);
	s.SetParity(Disabled);
	s.SetStopBits(One);
	s.EnableCommunication();
	s.EnableIterrupts();
	s.Start();

	sei();
    while(1)
    {
		l++;
		if(l%2)
			PORTB=0x03;
		else
			PORTB=0x00;
			
		UDR0 = c;
    }
}