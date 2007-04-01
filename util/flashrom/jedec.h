#ifndef __JEDEC_H__
#define __JEDEC_H__ 1
int write_byte_program_jedec(volatile uint8_t *bios, uint8_t *src,
			     volatile uint8_t *dst);

extern int probe_jedec(struct flashchip *flash);
extern int erase_chip_jedec(struct flashchip *flash);
extern int write_jedec(struct flashchip *flash, uint8_t *buf);
extern int erase_sector_jedec(volatile uint8_t *bios, unsigned int page);
extern int erase_block_jedec(volatile uint8_t *bios, unsigned int page);
extern int write_sector_jedec(volatile uint8_t *bios, uint8_t *src,
			      volatile uint8_t *dst, unsigned int page_size);

extern __inline__ void toggle_ready_jedec(volatile uint8_t *dst)
{
	unsigned int i = 0;
	uint8_t tmp1, tmp2;

	tmp1 = *dst & 0x40;

	while (i++ < 0xFFFFFFF) {
		tmp2 = *dst & 0x40;
		if (tmp1 == tmp2) {
			break;
		}
		tmp1 = tmp2;
	}
}

extern __inline__ void data_polling_jedec(volatile uint8_t *dst, uint8_t data)
{
	unsigned int i = 0;
	uint8_t tmp;

	data &= 0x80;

	while (i++ < 0xFFFFFFF) {
		tmp = *dst & 0x80;
		if (tmp == data) {
			break;
		}
	}
}

extern __inline__ void unprotect_jedec(volatile uint8_t *bios)
{
	*(volatile uint8_t *)(bios + 0x5555) = 0xAA;
	*(volatile uint8_t *)(bios + 0x2AAA) = 0x55;
	*(volatile uint8_t *)(bios + 0x5555) = 0x80;
	*(volatile uint8_t *)(bios + 0x5555) = 0xAA;
	*(volatile uint8_t *)(bios + 0x2AAA) = 0x55;
	*(volatile uint8_t *)(bios + 0x5555) = 0x20;

	usleep(200);
}

extern __inline__ void protect_jedec(volatile uint8_t *bios)
{
	*(volatile uint8_t *)(bios + 0x5555) = 0xAA;
	*(volatile uint8_t *)(bios + 0x2AAA) = 0x55;
	*(volatile uint8_t *)(bios + 0x5555) = 0xA0;

	usleep(200);
}

#endif				/* !__JEDEC_H__ */
