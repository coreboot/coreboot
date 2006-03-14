#ifndef __SHARPLHF00L04_H__
#define __SHARPLHF00L04_H__ 1

extern int probe_lhf00l04(struct flashchip *flash);
extern int erase_lhf00l04(struct flashchip *flash);
extern int write_lhf00l04(struct flashchip *flash, uint8_t *buf);
extern __inline__ void toggle_ready_lhf00l04(volatile uint8_t *dst)
{
	unsigned int i = 0;
	uint8_t tmp1, tmp2;

	tmp1 = *dst & 0x40;

	while (i++ < 0xFFFFFF) {
		tmp2 = *dst & 0x40;
		if (tmp1 == tmp2) {
			break;
		}
		tmp1 = tmp2;
	}
}

extern __inline__ void data_polling_lhf00l04(volatile uint8_t *dst, uint8_t data)
{
	unsigned int i = 0;
	uint8_t tmp;

	data &= 0x80;

	while (i++ < 0xFFFFFF) {
		tmp = *dst & 0x80;
		if (tmp == data) {
			break;
		}
	}
}

extern __inline__ void protect_lhf00l04(volatile uint8_t *bios)
{
	*(volatile uint8_t *) (bios + 0x5555) = 0xAA;
	*(volatile uint8_t *) (bios + 0x2AAA) = 0x55;
	*(volatile uint8_t *) (bios + 0x5555) = 0xA0;

	usleep(200);
}

#endif				/* !__SHARPLHF00L04_H__ */
