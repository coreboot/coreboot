#ifndef __M29F400BT_H__
#define __M29F400BT_H__ 1

#include <stdio.h>

extern int probe_m29f400bt(struct flashchip *flash);
extern int erase_m29f400bt(struct flashchip *flash);
extern int block_erase_m29f400bt(volatile uint8_t *bios, volatile uint8_t *dst);
extern int write_m29f400bt(struct flashchip *flash, uint8_t *buf);
extern int write_linuxbios_m29f400bt(struct flashchip *flash,
				     uint8_t *buf);

extern __inline__ void toggle_ready_m29f400bt(volatile uint8_t *dst)
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

extern __inline__ void data_polling_m29f400bt(volatile uint8_t *dst,
					      uint8_t data)
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

extern __inline__ void protect_m29f400bt(volatile uint8_t *bios)
{
	*(volatile uint8_t *) (bios + 0xAAA) = 0xAA;
	*(volatile uint8_t *) (bios + 0x555) = 0x55;
	*(volatile uint8_t *) (bios + 0xAAA) = 0xA0;

	usleep(200);
}

extern __inline__ void write_page_m29f400bt(volatile uint8_t *bios, uint8_t *src,
					    volatile uint8_t *dst,
					    int page_size)
{
	int i;

	for (i = 0; i < page_size; i++) {
		*(volatile uint8_t *) (bios + 0xAAA) = 0xAA;
		*(volatile uint8_t *) (bios + 0x555) = 0x55;
		*(volatile uint8_t *) (bios + 0xAAA) = 0xA0;

		/* transfer data from source to destination */
		*dst = *src;
		//*(volatile char *) (bios) = 0xF0;
		//usleep(5);
		toggle_ready_m29f400bt(dst);
		printf
		    ("Value in the flash at address %p = %#x, want %#x\n",
		     (uint8_t *) (dst - bios), *dst, *src);
		dst++;
		src++;
	}

}

#endif				/* !__M29F400BT_H__ */
