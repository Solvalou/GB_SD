#include "gb_rom.h"
#include <avr/io.h>

void gb_rom_init(void)
{
	// Address bus
	DDRA = 0xff;						// A[0:7]
	DDRB = 0x0f;						// A[8:11]
	
	// Data bus
	DDRC = 0xff;						// D[0:7]

	// Address and Control bus
	DDRD = 0xfc;						// PD[2:4] => CE, WE, OE; PD[5:7] => A[12:14]
}

void gb_rom_write(void)
{
	PORTD = ((1 << CE) | (1 << OE));
	for(int i = 0x00; i <= 0x7f; i++)
	{
		PORTB &= ~(0x0f);
		PORTB |= (i & 0x0f);
		PORTD &= ~(0xe0);
		PORTD |= ((i & 0xf0) << 1);
		for(int z = 0; z <= 0xff; z++)
		{
			PORTA = z;
			PORTC = uart_rx_char();
			//UART_tx_byte(PORTC);
			PORTD &= ~(1 << CE);
			PORTD |= (1 << CE);
		}
	}
	
	//uart_tx_string("DONE!\n");
}

void gb_rom_verify(void)
{
		char hex_string;
		PORTB = 0;
		PORTC = 0;
		DDRC = 0x00;					// Data Bus set to input
		PORTD |= (1 << WE);
		PORTD &= ~(1 << OE);
		for(int z = 0; z <= 0xff; z++)	// Address: 0x0000 to 0x00ff
		{
			PORTA = z;
			PORTD &= ~(1 << CE);
			hex_string = PINC;
			//uart_putc(hex_string);
			PORTD |= (1 << CE);
		}
		
		// Disable output
		PORTD &= ~(1 << CE);
		PORTA = 0;
		PORTB = 0;
		PORTC = 0;
		PORTD = 0;
}