#ifndef __JEDEC_H__
#define __JEDEC_H__ 1

extern int probe_jedec(struct flashchip *flash);
extern int erase_chip_jedec(struct flashchip *flash);
extern int write_jedec(struct flashchip *flash, unsigned char *buf);
extern int erase_sector_jedec(volatile char *bios, unsigned int page);
extern int write_sector_jedec(volatile unsigned char *bios,
			      unsigned char *src,
			      volatile unsigned char *dst,
			      unsigned int page_size);

extern __inline__ void toggle_ready_jedec(volatile char *dst)
{
	unsigned int i = 0;
	char tmp1, tmp2;

	tmp1 = *dst & 0x40;

	while (i++ < 0xFFFFFFF) {
		tmp2 = *dst & 0x40;
		if (tmp1 == tmp2) {
			break;
		}
		tmp1 = tmp2;
	}
}

extern __inline__ void data_polling_jedec(volatile char *dst, char data)
{
	unsigned int i = 0;
	char tmp;

	data &= 0x80;

	while (i++ < 0xFFFFFFF) {
		tmp = *dst & 0x80;
		if (tmp == data) {
			break;
		}
	}
}

extern __inline__ void unprotect_jedec(volatile char *bios)
{
	*(volatile char *) (bios + 0x5555) = 0xAA;
	*(volatile char *) (bios + 0x2AAA) = 0x55;
	*(volatile char *) (bios + 0x5555) = 0x80;
	*(volatile char *) (bios + 0x5555) = 0xAA;
	*(volatile char *) (bios + 0x2AAA) = 0x55;
	*(volatile char *) (bios + 0x5555) = 0x20;

	usleep(200);
}

extern __inline__ void protect_jedec(volatile char *bios)
{
	*(volatile char *) (bios + 0x5555) = 0xAA;
	*(volatile char *) (bios + 0x2AAA) = 0x55;
	*(volatile char *) (bios + 0x5555) = 0xA0;

	usleep(200);
}

#endif				/* !__JEDEC_H__ */
