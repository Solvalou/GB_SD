#ifndef UART_H_
#define UART_H_

#ifndef F_CPU
	#define F_CPU 8000000
#endif

#define BAUD 9600UL
#include <util/setbaud.h>

/*#define UBRR_VAL ((F_CPU + BAUD * 8) / (BAUD * 16) - 1)
#define BAUD_REAL (F_CPU / (16 * (UBRR_VAL + 1)))
#define BAUD_ERROR ((BAUD_REAL * 1000) / BAUD)

#if ((BAUD_ERROR < 990) || (BAUD_ERROR > 1010))
	#error systematical error of baud rate is higher than 1%!
#endif*/

extern void uart_init(void);

extern char uart_tx_char(char);
extern char uart_tx_string(char*);

extern char uart_rx_char(void);

void uart_tx_hex8(uint8_t hex);
void uart_tx_hex16(uint16_t hex);
void uart_tx_hex32(uint32_t hex);

static uint8_t hex_to_ascii(uint8_t hex);

#define TX_BUFSIZE 10
#define RX_BUFSIZE 10

extern ISR(SIG_UART_RECV);
extern ISR(SIG_UART_DATA);

// RX STATUS
extern uint8_t rx_status;

#define RX_ENABLE	1
#define RX_DISABLE	0

#endif