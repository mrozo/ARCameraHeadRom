/*
 * AtmegaTimer.cpp
 *
 * Created: 2014-11-01 18:06:23
 *  Author: m
 */
#include "Standarts.h"
#include "Timer.h"
#include <util\delay.h>

UINT C = 0;
volatile UINT x = 0;
UINT a = 0;


ISR(TIMER0_COMPA_vect)
{
	C = ~C;
	if(C==0xff)
		PORTB = 0x01;
	else
		PORTB = 0x02;
}

int main(void)
{
	/*OCR0A   = 0xC0;  //128
	TCCR0A  = 0x02; //
	TCCR0B  = 0x05; // prescaler  , tryb CTC
	TIMSK0 |= (1 << OCIE0A) | (1<<TOIE0);  // odblokuj przerwanie przy TCNT0 == OCRA
	TIFR0  |= (1 << OCF0A)  | (1<<TOV0);    // uruchom przerwanie przy TCNT0 == OCRA
	*/
	DDRB = 0x03;
	PORTB = 0x0;
	DDRD = 0xFF;
	PORTD = 0xFF;
	Timer t = Timer();
	t.SetClockSource(Internal);	
	t.SetOverflowInterval(MILISECONDS_PERIOD (261) );
	t.Start();
	_delay_ms(1000);
	sei();

    while(!x)
    {
        PORTD = TCNT0;
    }

}
