extern int probe_jedec (struct flashchip * flash);
extern int erase_jedec (struct flashchip * flash);
extern int write_jedec (struct flashchip * flash, char * buf);

extern __inline__ void toggle_ready_jedec (volatile char * dst)
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

extern __inline__ void data_polling_jedec (volatile char * dst, char data)
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

extern __inline__ void protect_jedec (volatile char * bios)
{
	*(volatile char *) (bios + 0x5555) = 0xAA;
	*(volatile char *) (bios + 0x2AAA) = 0x55;
	*(volatile char *) (bios + 0x5555) = 0xA0;

	usleep(200);
}

extern __inline__ void write_page_jedec (volatile char * bios, char * src, volatile char * dst,
					 int page_size)
{
	int i;

	*(volatile char *) (bios + 0x5555) = 0xAA;
	*(volatile char *) (bios + 0x2AAA) = 0x55;
	*(volatile char *) (bios + 0x5555) = 0xA0;

	for (i = 0; i < page_size; i++) {
		/* transfer data from source to destination */
		*dst++ = *src++;
	}

	usleep(100);
	toggle_ready_jedec(dst-1);
}
