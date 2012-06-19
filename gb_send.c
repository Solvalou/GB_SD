#include <avr/io.h>
#include <stdint.h> 
#include <avr/interrupt.h>
#include "uart.h"
#include "timer.h"
#include "fifo.h"
#include "spi.h"
#include "gb_send.h"
#include "gb_rom.h"
#include "sd.h"
#include "fat.h"

int main(void)
{
	if(!init())
	{
		uart_tx_string("INITIALIZATION FAILED\n");
		for(;;);
	}	
		
//===============================================================//
//	MAIN ROUTINE												 //
//===============================================================//

	//gb_rom_write();
	uint8_t buffer[512];
	
	if(SD_TYPE != SD_NO_CARD)
	{
		/*for(int y = 0; y < 0xff; y++)
		{
			sd_single_block_read(y, buffer);
			for(int i = 0; i < 512; i++)
			{
				UART_tx_byte(buffer[i]);
			}
		}*/
		
		
	}	
	else
	{
		for(;;);
	}

    for(;;);
}

char init(void)
{
	uart_init();
	timer_init();
	//gb_rom_init();
	sei();
	spi_init();
	if(!sd_init())
		return 0;
	if(!fat_init())
		uart_tx_string("FAT INIT FAILED");

	return 1;
}