#ifndef __JEDEC_H__
#define __JEDEC_H__ 1

extern void toggle_ready_jedec(volatile uint8_t *dst);
extern void data_polling_jedec(volatile uint8_t *dst, uint8_t data);
extern void unprotect_jedec(volatile uint8_t *bios);
extern void protect_jedec(volatile uint8_t *bios);
int write_byte_program_jedec(volatile uint8_t *bios, uint8_t *src,
			     volatile uint8_t *dst);
extern int probe_jedec(struct flashchip *flash);
extern int erase_chip_jedec(struct flashchip *flash);
extern int write_jedec(struct flashchip *flash, uint8_t *buf);
extern int erase_sector_jedec(volatile uint8_t *bios, unsigned int page);
extern int erase_block_jedec(volatile uint8_t *bios, unsigned int page);
extern int write_sector_jedec(volatile uint8_t *bios, uint8_t *src,
			      volatile uint8_t *dst, unsigned int page_size);

#endif				/* !__JEDEC_H__ */
