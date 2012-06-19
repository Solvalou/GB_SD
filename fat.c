#include <avr/io.h>
#include "sd.h"
#include "fat.h"

// preferences
uint8_t partition_type = 0;
uint8_t sectors_per_cluster;


// lba addresses
uint32_t vbr_address;
uint32_t fat_address;
uint32_t cluster_address;
uint32_t root_address;

//+==========================================+
//| FAT32 INITIALIZATION					 |
//+==========================================+

uint8_t fat_init(void)
{
	// check MBR signature
	uint8_t buffer[BLOCK_SIZE];
	if(!sd_single_block_read(MBR, buffer))
		return 0;

	if(!((buffer[510] == 0x55) && (buffer[511] == 0xaa)))
		return 0;

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
			return 0;
	}

	// calculate VBR-address
	vbr_address = buffer[MBR_VBR_ADDR] + (buffer[MBR_VBR_ADDR + 1] << 8) 
		+ (buffer[MBR_VBR_ADDR + 2] << 16) + (buffer[MBR_VBR_ADDR + 3] << 24);
	if(!sd_single_block_read(vbr_address, buffer))
		return 0;

	// check: signature; size of blocks; number of FATs
	if(!((buffer[510] == 0x55) && (buffer[511] == 0xaa)
		&& (buffer[BPB_BYTESPERSEC] == 0x00) && (buffer[BPB_BYTESPERSEC + 1] == 0x02)
		&& (buffer[BPB_NUMFATS] == 0x02)))
		return 0;

	fat_address = vbr_address + (buffer[BPB_RSVDSECCNT] + (buffer[BPB_RSVDSECCNT + 1] << 8));
	cluster_address = fat_address + (2 * (buffer[BPB_FATSZ32] + (buffer[BPB_FATSZ32 + 1] << 8) 
		+ (buffer[BPB_FATSZ32 + 2] << 16) + (buffer[BPB_FATSZ32 + 3] << 24)));

	sectors_per_cluster = buffer[BPB_SECPERCLUS];
	root_address = buffer[BPB_ROOTCLUS] + (buffer[BPB_ROOTCLUS + 1] << 8) 
		+ (buffer[BPB_ROOTCLUS + 2] << 16) + (buffer[BPB_ROOTCLUS + 3] << 24);

	return 1;
}


