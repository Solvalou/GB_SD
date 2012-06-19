#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "fifo.h"

void fifo_init(fifo_t *f, uint8_t *buffer, const uint8_t size)
{
	f->count = 0;
	f->ptr_read = buffer;								
	f->ptr_write = buffer;							// pointer start at the same position
	f->size = size;
	f->pos_read = 0;
	f->pos_write = 0;
}

uint8_t fifo_push(fifo_t *f, const uint8_t data)
{
	if((f->pos_write + 1 == f->pos_read) || ((f->pos_read == 0) && (f->pos_write + 1 == f->size)))
	//if(f->count >= f->size)	
		return 1;
	
	*(f->ptr_write++) = data;
	f->count++;

	if(++f->pos_write >= f->size)
	{
		f->pos_write = 0;
		f->ptr_write -= f->size;
	}	
	return 0;
}

uint8_t fifo_pop(fifo_t *f)
{
	if(f->pos_read == f->pos_write)
	//if(f->count == 0)
		return 1;

	uint8_t data;		
	data = *(f->ptr_read++);
	f->count--;
	
	if(++f->pos_read >= f->size)
	{
		f->pos_read = 0;
		f->ptr_read -= f->size;
	}
	
	return data;
}