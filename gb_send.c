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
		
//===============================================================//
//	MAIN ROUTINE												 //
//===============================================================//

	//gb_rom_write();
	uint32_t cluster_nr;
	uint32_t next_cluster;
	
	init();	
	
	while(1)
	{	
		rx_status = RX_ENABLE;
		uart_tx_string("cluster to load: ");
		cluster_nr = uart_rx_char();
		uart_tx_hex32(cluster_nr);
		uart_tx_char('\n');
		while(fat_print_dir(cluster_nr) != 0xffff)
		{
			cluster_nr = fat_next_cluster(cluster_nr);
			if (cluster_nr == 0x0fffffff)
				break;
		}
	}	
	
	/*if(SD_TYPE != SD_NO_CARD)
	{
		sd_single_block_read(0, buffer);
		for(int i = 0; i < 512; i++)
		{
			uart_tx_char(buffer[i]);
		}		
	}	
	else
	{
		for(;;);
	}*/

    for(;;);
}

char init(void)
{
	uart_init();
	timer_init();
	spi_init();
	//gb_rom_init();
	sei();
	
	if(!sd_init())
		return 0;
	if(!fat_init())
		//uart_tx_string("FAT INIT FAILED\n");
	//fat_rescue();

	return 1;
}


void fat_rescue(void)
{
uint16_t buff_cursor;

uint8_t sd_buffer[512];

sd_buffer[0] = 0xEB;
sd_buffer[1] = 0x00;
sd_buffer[2] = 0x90;

for (buff_cursor = 0x03; buff_cursor < 0x0B; buff_cursor++)
{
	sd_buffer[buff_cursor] = 0x20;
}

sd_buffer[0x0B] = 0x00;
sd_buffer[0x0C] = 0x02;
sd_buffer[0x0D] = 0x20;
sd_buffer[0x0E] = 0x01;
sd_buffer[0x0F] = 0x00;
sd_buffer[0x10] = 0x02;
sd_buffer[0x11] = 0x00;
sd_buffer[0x12] = 0x02;
sd_buffer[0x13] = 0x00;
sd_buffer[0x14] = 0x00;
sd_buffer[0x15] = 0xF8;
sd_buffer[0x16] = 0x7B;
sd_buffer[0x17] = 0x00;
sd_buffer[0x18] = 0x3F;
sd_buffer[0x19] = 0x00;
sd_buffer[0x1A] = 0x10;
sd_buffer[0x1B] = 0x00;
sd_buffer[0x1C] = 0x00;
sd_buffer[0x1D] = 0x00;
sd_buffer[0x1E] = 0x00;
sd_buffer[0x1F] = 0x00;
sd_buffer[0x20] = 0x2E;
sd_buffer[0x21] = 0x86;
sd_buffer[0x22] = 0x1E;
sd_buffer[0x23] = 0x00;
sd_buffer[0x24] = 0x80;
sd_buffer[0x25] = 0x00;
sd_buffer[0x26] = 0x00;
sd_buffer[0x27] = 0x12;
sd_buffer[0x28] = 0x23;
sd_buffer[0x29] = 0x34;
sd_buffer[0x2A] = 0x45;
sd_buffer[0x2B] = 'R';
sd_buffer[0x2C] = 'E';
sd_buffer[0x2D] = 'S';
sd_buffer[0x2E] = 'C';
sd_buffer[0x2F] = 'U';
sd_buffer[0x30] = 'E';
sd_buffer[0x31] = ' ';
sd_buffer[0x32] = ' ';
sd_buffer[0x33] = ' ';
sd_buffer[0x34] = ' ';
sd_buffer[0x35] = ' ';
sd_buffer[0x36] = 'F';
sd_buffer[0x37] = 'A';
sd_buffer[0x38] = 'T';
sd_buffer[0x39] = '1';
sd_buffer[0x3A] = '6';
sd_buffer[0x3B] = ' ';
sd_buffer[0x3C] = ' ';
sd_buffer[0x3D] = ' ';


for (buff_cursor = 0x3E; buff_cursor < 0x1FE; buff_cursor++)
{
	sd_buffer[buff_cursor] = 0x00;
}

sd_buffer[0x1FE] = 0x55;
sd_buffer[0x1FF] = 0xAA;

sd_single_block_write(0, sd_buffer);
}

void uart_tx_free_ram(void)
{
	uart_tx_string("FREE RAM: ");
	uart_tx_hex16(SP - (uint16_t) &__heap_start);
	uart_tx_char('\n');
}

void buffer_ram_test(void)
{
	uint8_t buffer[512];
	sd_single_block_read(0, buffer);
	uart_tx_char('\n');
	uart_tx_free_ram();
}