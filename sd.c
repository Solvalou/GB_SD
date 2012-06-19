#include <avr/io.h>
#include "timer.h"
#include "sd.h"
#include "spi.h"

uint8_t SD_TYPE = SD_NO_CARD;

//+==========================================+
//| SD-CARD INITIALIZATION					 |
//+==========================================+

uint8_t sd_init(void)
{
	uint8_t res[4];

	uart_tx_string("SD INIT START\n");
	timer = 2;
	while(timer);							// wait 2ms

	for(int i = 0; i <= 0x0f; i++)			// wait for at least 74 SCLK-cycles
		spi_tx_byte(0xff);
	
	for(int i = 0; i <= 100; i++)
	{
		uart_tx_string("SD-CARD INIT TRY\n");
		if(sd_tx_cmd(GO_IDLE_STATE, 0, 0x95) == 0x01)
		{
			uart_tx_string("GO IDLE STATE ACCEPTED\n");
			if(sd_tx_cmd(SEND_IF_COND, 0x1aa, 0x87) == 0x01)
			{
				uart_tx_string("SEND IF CONDITION ACCEPTED\n");
				for(int i = 0; i < 4; i++)
				{
					res[i] = spi_rx_byte();
				}
				if(res[2] == 0x01 && res[3] == 0xaa)
				{
					timer = 1000;
					while(timer)
					{
						sd_tx_cmd(APP_CMD, 0, 0x01);
						if(!sd_tx_cmd(SEND_OP_COND, 0x40000000, 0x01))
						{
							if(sd_tx_cmd(READ_OCR, 0, 0x01) == 0x00)
							{
								for(int i = 0; i < 4; i++)
								{
									res[i] = spi_rx_byte();
								}
								if(res[0] & 0x40)
								{
									uart_tx_string("SD-VERSION 2 BLOCK DETECTED\n");
									SD_TYPE = SD_VER2_BLOCK;
								}
								else
								{
									uart_tx_string("SD-VERSION 2 BYTE DETECTED\n");
									SD_TYPE = SD_VER2_BYTE;
									sd_tx_cmd(SET_BLOCKLEN, 0x200, 0x01);
								}
							}
							break;
						}
					}
					break;
				}
			}
			else
			{
				timer = 1000;
				while(timer)
				{
					sd_tx_cmd(APP_CMD, 0, 0x01);
					if(!sd_tx_cmd(SEND_OP_COND, 0, 0x01))
					{
						uart_tx_string("SD-VERSION 1 DETECTED\n");
						SD_TYPE = SD_VER1;
						sd_tx_cmd(SET_BLOCKLEN, 0x200, 0x01);
						break;
					}
				}
				break;
			}
		}
	}

	if(SD_TYPE == SD_NO_CARD)
	{
		uart_tx_string("SD INIT FAILED\n");
		return 0;
	}
	else											// set highest SPI-speed
	{
		uart_tx_string("SD INIT SUCCEDED\n");
		SPCR &= ~((1 << SPR0) | (1 << SPR1));
		SPSR |= (1 << SPI2X);
		return 1;
	}
}

//+==========================================+
//| SD: SEND COMMAND						 |
//+==========================================+

uint8_t sd_tx_cmd(uint8_t command, uint32_t arg, uint8_t crc)
{
	uint8_t res;
	int timeout = 0x0f;

	sd_disable();
	if(!sd_enable())
		return 0xff;

	spi_tx_byte(0x40 | command);
	spi_tx_byte((uint8_t)(arg >> 24));
	spi_tx_byte((uint8_t)(arg >> 16));
	spi_tx_byte((uint8_t)(arg >> 8));
	spi_tx_byte((uint8_t)arg);
	spi_tx_byte(crc);

	do
	{
		res = spi_rx_byte();
	}while((res & 0x80) && --timeout);
	return res;
}

//+==========================================+
//| SD ENABLE								 |
//+==========================================+

static uint8_t sd_enable(void)
{
	PORT_SPI &= ~(1 << SS);
	if(!sd_ready())
	{
		sd_disable();
		return 0;
	}
	return 1;
}

//+==========================================+
//| SD DISABLE								 |
//+==========================================+

static void sd_disable(void)
{
	PORT_SPI |= (1 << SS);
	spi_rx_byte();
}

//+==========================================+
//| SD READY?								 |
//+==========================================+

static uint8_t sd_ready(void)
{
	timer = 100;
	while(timer)
	{
		if(spi_rx_byte() == 0xff)
			return 1;
	}
	return 0;
}

//+==========================================+
//| SD SINGLE BLOCK READ OF 512Bytes		 |
//+==========================================+

uint8_t sd_single_block_read(uint32_t address, uint8_t *buffer)
{
	uint8_t token;

	if(SD_TYPE != SD_VER2_BLOCK)			// if SD-type isn't using block-addressing
		address *= 512;						// convert to byte-address

	if(sd_tx_cmd(READ_SINGLE_BLOCK, address, 0x01) != 0)
		return 0;

	timer = 200;
	do
	{
		token = spi_rx_byte();
	}while((token == 0xff) && timer);

	if(token != 0xfe)
		return 0;

	for(int i = 0; i < 512; i++)
	{
		*buffer++ = spi_rx_byte();
	}

	spi_rx_byte();
	spi_rx_byte();							// discard 16bit CRC-token

	sd_disable();
	return 1;
}

//+==========================================+
//| SD SINGLE BLOCK WRITE OF 512Bytes		 |
//+==========================================+
uint8_t sd_single_block_write(uint32_t address, uint8_t *buffer)
{
	uint8_t res;

	if(SD_TYPE != SD_VER2_BLOCK)
		address *= 512;

	if(sd_tx_cmd(WRITE_BLOCK, address, 0x01) != 0)
		return 0;

	spi_tx_byte(0xfe);						// send start-token
	for(int i = 0; i < 512; i++)
	{
		spi_tx_byte(*buffer++);
	}

	spi_tx_byte(0xff);
	spi_tx_byte(0xff);						// send dummy CRC-token

	timer = 200;
	do
	{
		res = spi_rx_byte();
	}while(((res & 0x1f) != 0x05) && timer);

	if((res & 0x1f) != 0x05)
		return 0;

	sd_disable();
	return 1;
}

//+==========================================+
//| SD MULTIPLE BLOCK READ START			 |
//+==========================================+

uint8_t sd_multiple_block_read_start(uint32_t address)
{
	uint8_t token;

	if(SD_TYPE != SD_VER2_BLOCK)
		address *= 512;

	if(sd_tx_cmd(READ_MULTIPLE_BLOCK, address, 0x01) != 0)
		return 0;

	timer = 200;
	do
	{
		token = spi_rx_byte();
	}while((token == 0xff) && timer);

	if(token != 0xfe)
		return 0;

	return 1;
}

//+==========================================+
//| SD MULTIPLE BLOCK READ 					 |
//+==========================================+

uint8_t sd_multiple_block_read(uint8_t *buffer)
{
	uint8_t token;
	
	for(int i = 0; i < 512; i++)
	{
		*buffer++ = spi_rx_byte();
	}

	spi_rx_byte();
	spi_rx_byte();							// discard 16bit CRC-token

	timer = 200;
	do
	{
		token = spi_rx_byte();
	}while((token == 0xff) && timer);

	if(token != 0xfe)
		return 0;

	return 1;
}

//+==========================================+
//| SD MULTIPLE BLOCK READ STOP				 |
//+==========================================+

uint8_t sd_multiple_block_read_stop(void)
{
	if(sd_tx_cmd(STOP_TRANSMISSION, 0, 0x01) != 0)
		return 0;

	sd_disable();
	return 1;
}

//+==========================================+
//| SD MULTIPLE BLOCK WRITE START			 |
//+==========================================+

uint8_t sd_multiple_block_write_start(uint32_t address)
{
	uint8_t res;

	if(SD_TYPE != SD_VER2_BLOCK)
		address *= 512;

	if(sd_tx_cmd(WRITE_MULTIPLE_BLOCK, address, 0x01) != 0)
		return 0;

	return 1;
}

//+==========================================+
//| SD MULTIPLE BLOCK WRITE 				 |
//+==========================================+

uint8_t sd_multiple_block_write(uint8_t *buffer)
{
	uint8_t res;
	
	spi_tx_byte(0xfe);						// send start-token
	for(int i = 0; i < 512; i++)
	{
		spi_tx_byte(*buffer++);
	}

	spi_tx_byte(0xff);
	spi_tx_byte(0xff);						// send dummy CRC-token

	timer = 200;
	do
	{
		res = spi_rx_byte();
	}while(((res & 0x1f) != 0x05) && timer);

	if((res & 0x1f) != 0x05)
		return 0;

	if(!sd_ready())
		return 0;

	return 1;
}

//+==========================================+
//| SD MULTIPLE BLOCK WRITE STOP			 |
//+==========================================+

uint8_t sd_multiple_block_write_stop()
{
	spi_tx_byte(0xfd);
	if(!sd_ready())
		return 0;

	if(sd_tx_cmd(SEND_STATUS, 0 , 0x01) != 0)
		return 0;
	if(spi_rx_byte() != 0)
		return 0;

	sd_disable();
	return 1;
}

//+==========================================+
//| SD: R1 DEBUG RESPONSE					 |
//+==========================================+

void debug_response(uint8_t res)
{
	if(res & 0%00000001)
		uart_tx_string("in idle state\n");
	if(res & 0%00000010)
		uart_tx_string("erase reset\n");
	if(res & 0%00000100)
		uart_tx_string("illegal command\n");
	if(res & 0%00001000)
		uart_tx_string("com crc error\n");
	if(res & 0%00010000)
		uart_tx_string("erase sequence error\n");
	if(res & 0%00100000)
		uart_tx_string("address error\n");
	if(res & 0%01000000)
		uart_tx_string("parameter error\n");
}