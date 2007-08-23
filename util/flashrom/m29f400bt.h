#ifndef __M29F400BT_H__
#define __M29F400BT_H__ 1

#include <stdio.h>

extern int probe_m29f400bt(struct flashchip *flash);
extern int erase_m29f400bt(struct flashchip *flash);
extern int block_erase_m29f400bt(volatile uint8_t *bios,
				 volatile uint8_t *dst);
extern int write_m29f400bt(struct flashchip *flash, uint8_t *buf);
extern int write_linuxbios_m29f400bt(struct flashchip *flash, uint8_t *buf);

extern void toggle_ready_m29f400bt(volatile uint8_t *dst);
extern void data_polling_m29f400bt(volatile uint8_t *dst, uint8_t data);
extern void protect_m29f400bt(volatile uint8_t *bios);
extern void write_page_m29f400bt(volatile uint8_t *bios, uint8_t *src,
				 volatile uint8_t *dst, int page_size);

#endif				/* !__M29F400BT_H__ */
