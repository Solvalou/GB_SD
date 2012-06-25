#include <avr/io.h>
#include "sd.h"
#include "fat.h"

// preferences
uint8_t partition_type = 0;
uint8_t sectors_per_cluster;


// lba addresses
uint32_t vbr_address;
uint32_t fat_address;
uint32_t root_address;
uint32_t root_cluster;

//+==========================================+
//| FAT32 INITIALIZATION					 |
//+==========================================+

uint8_t fat_init(void)
{
	// check MBR signature
	uint8_t buffer[BLOCK_SIZE];
	if(!sd_single_block_read(MBR, buffer))
		return 0;
	uart_tx_string("MBR LOAD SUCCEEDED\n");

	if(!((buffer[510] == 0x55) && (buffer[511] == 0xaa)))
		return 0;
	uart_tx_string("MBR SIGNATURE CORRECT\n");

	switch (buffer[MBR_TYPE_CODE])
	{
		case 0x0b:
			partition_type = TYPE_FAT32;
			break;
		case 0x0c:
			partition_type = TYPE_FAT32;
			break;
		case 0x0e:
			partition_type = TYPE_FAT16;
			break;
		default:
			uart_tx_string("UNKNOWN PARTITION TYPE\n");
			return 0;
	}

	// calculate VBR-address
	vbr_address = buffer[MBR_VBR_ADDR] + (buffer[MBR_VBR_ADDR + 1] << 8) 
		+ (buffer[MBR_VBR_ADDR + 2] << 16) + (buffer[MBR_VBR_ADDR + 3] << 24);
		
	if(!sd_single_block_read(vbr_address, buffer))
		return 0;
	uart_tx_string("VBR LOAD SUCCEEDED\n");

	// check: signature; size of blocks; number of FATs
	if(!((buffer[510] == 0x55) && (buffer[511] == 0xaa)
		&& (buffer[BPB_BYTESPERSEC] == 0x00) && (buffer[BPB_BYTESPERSEC + 1] == 0x02)
		&& (buffer[BPB_NUMFATS] == 0x02)))
		return 0;
	uart_tx_string("VBR SIGNATURE CORRECT\n");

	fat_address = vbr_address + (buffer[BPB_RSVDSECCNT] + (buffer[BPB_RSVDSECCNT + 1] << 8));
	root_address = fat_address + (2 * (buffer[BPB_FATSZ32] + (buffer[BPB_FATSZ32 + 1] << 8) 
		+ (buffer[BPB_FATSZ32 + 2] << 16) + (buffer[BPB_FATSZ32 + 3] << 24)));

	sectors_per_cluster = buffer[BPB_SECPERCLUS];
	root_cluster = buffer[BPB_ROOTCLUS] + (buffer[BPB_ROOTCLUS + 1] << 8) 
		+ (buffer[BPB_ROOTCLUS + 2] << 16) + (buffer[BPB_ROOTCLUS + 3] << 24);

	// DEBUG PRINT-SECTION
		uart_tx_string("VBR-address: ");
		uart_tx_hex32(vbr_address);
		uart_tx_char('\n');
		
		uart_tx_string("FAT-address: ");
		uart_tx_hex32(fat_address);
		uart_tx_char('\n');
		
		uart_tx_string("ROOT-address: ");
		uart_tx_hex32(root_address);
		uart_tx_char('\n');
	return 1;
}

uint16_t fat_print_dir(uint32_t dir_cluster)
{
	uint8_t buffer[512];
	uint32_t cluster;
	uint32_t size;
	struct DIR_Entry *entry;
	
	for(int y = 0; y < sectors_per_cluster; y++)
	{
		sd_single_block_read((root_address + y + ((dir_cluster - 2) * sectors_per_cluster)), buffer);
		for(int i = 0; i < BLOCK_SIZE; i += 32)
		{
			entry = (struct DIR_Entry*)&buffer[i];

			if((entry->DIR_Attr & 0x0f) != ATTR_LONG_NAME)
			{
				if(entry->DIR_Name[0] == 0x00)
					return 0xffff;
				if(entry->DIR_Name[0] != 0xe5)
				{
					for(int x = 0; x < 8; x++)
					{
						if(entry->DIR_Name[x] != 0x20)
						uart_tx_char(entry->DIR_Name[x]);
					}
					uart_tx_char('.');
					for(int x = 8; x < 11; x++)
					{
						uart_tx_char(entry->DIR_Name[x]);
					}
					uart_tx_string("  ");
					
					cluster = entry->DIR_FstClusLO | (entry->DIR_FstClusHI << 32);
					uart_tx_hex32(cluster);
					uart_tx_string("  ");
					
					size = entry->DIR_FileSize;
					uart_tx_hex32(size);
					uart_tx_char('\n');
				}
			}
		}
	}
	return 1;	
}

fat_load_uart()
{
	
}

uint32_t fat_next_cluster(uint32_t cluster)
{
	uint8_t buffer[512];
	uint32_t *pointer;

	sd_single_block_read(fat_address + (cluster / 128), buffer);
	
	if(cluster > 128)
		pointer = &buffer[4 * (cluster - (cluster % 128))];
	pointer = &buffer[4 * cluster];
	
	return (*pointer);
}
