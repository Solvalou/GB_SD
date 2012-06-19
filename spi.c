#include "spi.h"
#include <avr/io.h>

//+==========================================+
//| SPI INITIALIZATION						 |
//+==========================================+

void spi_init(void)
{
	DDR_SPI |= ((1 << SS) | (1 << MOSI) | (1 << SCK));					// SS = output in Master Mode
	SPCR = ((1 << SPE) | (1 << MSTR) | (1 << SPR1));
	SPSR = (1 << SPI2X);
	/* SPE: Enable SPI
	// MSTR: Set MCU as master
	// SPR0, SPR1, SPI2X: Clock = f_osc/32
	*/
}

//+==========================================+
//| SPI: TRANSFER BYTE 						 |
//+==========================================+

void spi_tx_byte(uint8_t byte)
{
	SPDR = byte;														// start transmission
	while(!(SPSR & (1 << SPIF)));										// wait for completion
}

//+==========================================+
//| SPI: RECIEVE BYTE 						 |
//+==========================================+

uint8_t spi_rx_byte(void)
{
	SPDR = 0xff;
	while(!(SPSR & (1 << SPIF)));
	return SPDR;
}