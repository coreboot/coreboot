#ifndef __SST_FWHUB_H__
#define __SST_FWHUB_H__ 1

extern int probe_sst_fwhub(struct flashchip *flash);
extern int erase_sst_fwhub(struct flashchip *flash);
extern int write_sst_fwhub(struct flashchip *flash, unsigned char *buf);

extern __inline__ void toggle_ready_sst_fwhub(volatile char *dst)
{
	unsigned int i = 0;
	char tmp1, tmp2;

	tmp1 = *dst & 0x40;

	while (i++ < 0xFFFFFF) {
		tmp2 = *dst & 0x40;
		if (tmp1 == tmp2) {
			break;
		}
		tmp1 = tmp2;
	}
}

extern __inline__ void data_polling_sst_fwhub(volatile char *dst, char data)
{
	unsigned int i = 0;
	char tmp;

	data &= 0x80;

	while (i++ < 0xFFFFFF) {
		tmp = *dst & 0x80;
		if (tmp == data) {
			break;
		}
	}
}

extern __inline__ void protect_sst_fwhub(volatile char *bios)
{
	*(volatile char *) (bios + 0x5555) = 0xAA;
	*(volatile char *) (bios + 0x2AAA) = 0x55;
	*(volatile char *) (bios + 0x5555) = 0xA0;

	usleep(200);
}

#endif				/* !__SST_FWHUB_H__ */
