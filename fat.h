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

// DATA TYPE
struct DIR_Entry{
	uint8_t DIR_Name[11];
	uint8_t DIR_Attr;
	uint8_t DIR_NTRes;
	uint8_t DIR_CrtTimeTenth;
	uint16_t DIR_CrtTime;
	uint16_t DIR_CrtDate;
	uint16_t DIR_LstAccDate;
	uint16_t DIR_FstClusHI;
	uint16_t DIR_WrtTime;
	uint16_t DIR_WrtDate;
	uint16_t DIR_FstClusLO;
	uint32_t DIR_FileSize;
	};

// FAT ATTRIBUTES
#define ATTR_READ_ONLY	0x01
#define ATTR_HIDDEN		0x02
#define ATTR_SYSTEM		0x04
#define ATTR_VOLUME_ID	0x08
#define ATTR_DIRECTORY	0x10
#define ATTR_ARCHIVE	0x20
#define ATTR_LONG_NAME	(ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID)

#define ROOT 0x02

// FUNCTIONS
extern uint8_t fat_init(void);
uint32_t fat_next_cluster(uint32_t);

#endif