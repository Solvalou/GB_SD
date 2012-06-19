#ifndef GB_ROM_H_
#define GB_ROM_H_

#define CE 2
#define WE 3
#define OE 4

extern void gb_rom_init(void);
extern void gb_rom_write(void);
extern void gb_rom_verify(void);

#endif