extern int probe_82802ab (struct flashchip * flash);
extern int erase_82802ab (struct flashchip * flash);
extern int write_82802ab (struct flashchip * flash, char * buf);

extern __inline__ void toggle_ready_82802ab (volatile char * dst)
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

extern __inline__ void data_polling_82802ab (volatile char * dst, char data)
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

extern __inline__ void protect_82802ab (volatile char * bios)
{
	*(volatile char *) (bios + 0x5555) = 0xAA;
	*(volatile char *) (bios + 0x2AAA) = 0x55;
	*(volatile char *) (bios + 0x5555) = 0xA0;

	usleep(200);
}

