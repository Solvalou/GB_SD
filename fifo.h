#ifndef FIFO_H_
#define FIFO_H_

typedef struct  
{
	uint8_t volatile count;		// # of elements in buffer
	uint8_t size;				// buffer size
	uint8_t *ptr_read;			// read pointer
	uint8_t *ptr_write;			// write pointer
	uint8_t pos_read;
	uint8_t pos_write;
} fifo_t;

extern void fifo_init(fifo_t*, uint8_t*, const uint8_t);
extern uint8_t fifo_push(fifo_t*, const uint8_t);
extern uint8_t fifo_pop(fifo_t*);

#endif