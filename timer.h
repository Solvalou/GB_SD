#ifndef TIMER_H_
#define TIMER_H_

#define PRESCALE_TIMER 64
#define F_TIMER F_CPU/PRESCALE_TIMER

extern void timer_init(void);
extern ISR(TIMER0_COMP_vect);

volatile uint8_t timer;

#endif