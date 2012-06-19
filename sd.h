#ifndef SD_H_
#define SD_H_

#define SD_NO_CARD					0
#define SD_VER1						1
#define SD_VER2_BYTE				2
#define SD_VER2_BLOCK				3

// COMMON COMMANDS
#define GO_IDLE_STATE 				0x00 	// CMD0
#define SEND_OP_COND				0x01	// CMD1
#define SWITCH_FUNC					0x06	// CMD6
#define SEND_IF_COND				0x08	// CMD8
#define SEND_CSD					0x09	// CMD9
#define SEND_CID					0x0A	// CMD10
#define STOP_TRANSMISSION 			0x0C	// CMD12
#define SEND_STATUS					0x0D	// CMD13
#define SET_BLOCKLEN				0x10	// CMD16
#define READ_SINGLE_BLOCK			0x11	// CMD17
#define READ_MULTIPLE_BLOCK			0x12	// CMD18
#define WRITE_BLOCK 				0x18	// CMD24
#define WRITE_MULTIPLE_BLOCK		0x19	// CMD25
#define PROGRAM_CSD					0x1b	// CMD27
#define SET_WRITE_PROT				0x1c	// CMD28
#define CLR_WRITE_PROT				0x1d	// CMD29
#define SEND_WRITE_PROT				0x1e	// CMD30
#define ERASE_WR_BLK_START_ADDR		0x20	// CMD32
#define ERASE_WR_BLK_END_ADDR		0x21	// CMD33
#define ERASE 						0x26 	// CMD38
#define LOCK_UNLOCK					0x2A	// CMD42
#define APP_CMD 					0x37	// CMD55
#define GEN_CMD						0x38	// CMD56
#define READ_OCR					0x3A 	// CMD58
#define CRC_ON_OFF					0x3b	// CMD59

// APPLICATION-SPECIFIC COMMANDS
#define SD_STATUS 					0x0D 	// ACMD13
#define SEND_NUM_WR_BLOCKS			0x16	// ACMD22
#define SET_WR_BLOCK_ERASE_COUNT 	0x17	// ACMD23
#define SEND_OP_COND				0x29	// ACMD41
#define SET_CLR_CARD_DETECT			0x2A	// ACMD42
#define SEND_SCR					0x33	// ACMD51	


// FUNCTIONS
extern uint8_t sd_init(void);
extern uint8_t sd_tx_cmd(uint8_t command, uint32_t arg, uint8_t crc);

extern uint8_t sd_single_block_read(uint32_t address, uint8_t *buffer);
extern uint8_t sd_single_block_write(uint32_t address, uint8_t *buffer);

extern uint8_t sd_multiple_block_read_start(uint32_t address);
extern uint8_t sd_multiple_block_read(uint8_t *buffer);
extern uint8_t sd_multiple_block_read_stop(void);

static uint8_t sd_enable(void);
static void sd_disable(void);
static uint8_t sd_ready(void);

extern void debug_response(uint8_t res);

extern uint8_t SD_TYPE;

#endif