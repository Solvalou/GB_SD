#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"

//+==========================================+
//| TIMER INITIALIZATION					 |
//+==========================================+

void timer_init(void)
{
	timer = 0;

	OCR0 = 0x7c;
	TCNT0  = 0x83;

	TIMSK |= (1 << OCIE0);
	TCCR0 = ((1 << WGM01) | (1 << CS02) | (1<< CS00));		// 1/64 Prescale
}

//+==========================================+
//| TIMER INTERRUPT ROUTINE					 |
//+==========================================+

ISR(TIMER0_COMP_vect)
{
	if(timer > 0)
		timer--;
}