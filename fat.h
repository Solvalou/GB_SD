#ifndef FAT_H_
#define FAT_H_

#define BLOCK_SIZE 512

// PARTITION TYPE
#define TYPE_FAT16 1
#define TYPE_FAT32 2

// BLOCK ADDRESSES
#define MBR 0

// BYTE ADDRESSES
#define MBR_TYPE_CODE 0x1c2
#define MBR_VBR_ADDR 0x1c6

// VBR 
struct vbr{
	uint16_t 	BPB_BytesPerSec;
	uint8_t 	BPB_SecPerClus;
	uint16_t 	BPB_RsvdSecCnt;
	uint8_t	 	BPB_NumFats;
	uint32_t 	BPB_FATSz32;
	uint32_t 	BPB_RootClus;
};

#define BPB_BYTESPERSEC 0x0b
#define BPB_SECPERCLUS 	0x0d
#define BPB_RSVDSECCNT 	0x0e
#define BPB_NUMFATS 	0x10
#define BPB_FATSZ32 	0x24
#define BPB_ROOTCLUS 	0x2c

// FUNCTIONS
extern uint8_t fat_init(void);

static uint16_t little_to_big_endian16(uint16_t i);
static uint32_t little_to_big_endian32(uint16_t i);

#endif