#ifndef SPI_H_
#define SPI_H_

#include <stdint.h> 

#define DDR_SPI DDRB
#define PORT_SPI PORTB
#define SS		4
#define MOSI	5
#define MISO	6
#define SCK	7

extern void spi_init(void);
extern void spi_tx_byte(uint8_t byte);
extern uint8_t spi_rx_byte(void);

#define nop()  __asm__ __volatile__ ("nop" ::)

#endif