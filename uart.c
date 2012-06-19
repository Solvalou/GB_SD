#include <avr/io.h>
#include <stdint.h> 
#include <avr/interrupt.h>
#include "uart.h"
#include "fifo.h"


uint8_t tx_buf[TX_BUFSIZE];
fifo_t tx_fifo =
	{
		.count = 0,
		.size = TX_BUFSIZE,
		.ptr_read = tx_buf,
		.ptr_write = tx_buf
	};

uint8_t rx_buf[RX_BUFSIZE];
fifo_t rx_fifo =
{
	.count = 0,
	.size = RX_BUFSIZE,
	.ptr_read = rx_buf,
	.ptr_write = rx_buf
};

void uart_init(void)
{
	UBRRH = UBRRH_VALUE;
	UBRRL = UBRRL_VALUE;
	
#if USE_2X
		UCSRA |= (1 << U2X);
#else
		UCSRA &= ~(1 << U2X);
#endif
	
	UCSRB |= ((1 << RXCIE ) | (1 << RXEN) | (1 << TXEN));		// RX Complete Interrupt Enable; Receiver Enable; Transmitter Enable
	UCSRC |= ((1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0));		// 8-Bit Character Size
}

char uart_tx_char(char byte)
{
	while(fifo_push(&tx_fifo, byte) != 0);						// if buffer full, wait for interrupt to clear it
	UCSRB |= (1 << UDRIE);										// UART Data Register Empty Interrupt Enable
	
	return 0;
}

char uart_rx_char(void)
{
	while(rx_fifo.count == 0);
	return fifo_pop(&rx_fifo);
}

char uart_tx_string(char *str)
{
	while(*str)
	{
		uart_tx_char(*str);
		str++;
	}
	return 0;
}

void uart_tx_hex8(uint8_t hex)
{
	uint8_t digit1 = hex_to_ascii(((hex >> 4) & 0x0f));
	uint8_t digit2 = hex_to_ascii((hex & 0x0f));
	
	uart_tx_string("0x");
	uart_tx_char(digit1);
	uart_tx_char(digit2);
}

void uart_tx_hex16(uint16_t hex)
{
	uint8_t shift = 16;
	uart_tx_string("0x");
	
	do
	{
		shift -= 4;
		uart_tx_char(hex_to_ascii((((uint8_t)(hex >> shift) & 0x0f))));
	}while(shift > 0);
}

void uart_tx_hex32(uint32_t hex)
{
	uint8_t shift = 32;
	uart_tx_string("0x");
	
	do
	{
		shift -= 4;
		uart_tx_char(hex_to_ascii((((uint8_t)(hex >> shift) & 0x0f))));
	}while(shift > 0);	
}

static uint8_t hex_to_ascii(uint8_t hex)
{
	uint8_t ascii;
	if(hex < 0x0A)
	{
		ascii = hex + 0x30;
	}
	else
	{
		ascii = hex + 0x37;
	}
	
	return ascii;
}

ISR(SIG_UART_RECV)
{
	fifo_push(&rx_fifo, UDR);
}

ISR(SIG_UART_DATA)
{
	if (tx_fifo.count > 0)										// if buffer filled
		UDR = fifo_pop(&tx_fifo);								//		pop data from buffer and send
	else														// if buffer empty
		UCSRB &= ~(1 << UDRIE);									//		Reset UART Data Register Empty Interrupt
}