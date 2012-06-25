#ifndef GB_SEND_H_
#define GB_SEND_H_

#define DEBUG

extern char init(void);
extern void fat_rescue(void);

extern unsigned char __heap_start;
extern void uart_tx_free_ram(void);

#endif