#ifndef __JEDEC_H__
#define __JEDEC_H__ 1

extern int probe_jedec (struct flashchip * flash);
extern int erase_jedec (struct flashchip * flash);
extern int write_jedec (struct flashchip * flash, unsigned char * buf);

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

extern int erase_sector_jedec (volatile char * bios, unsigned int page);

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

static __inline__ int write_sector_jedec(volatile char * bios, 
					 unsigned char * src,
					 volatile unsigned char * dst, 
					 unsigned int page_size)
{
	int i;
	volatile char *Temp;

	for (i = 0; i < page_size; i++) {
		if (*dst != 0xff) {
			printf("FATAL: dst %p not erased (val 0x%x\n", dst, *dst);
			return(-1);
		}
		/* transfer data from source to destination */
		if (*src == 0xFF) {
			dst++, src++;
			/* If the data is 0xFF, don't program it */
			continue;
		}
		Temp =   (bios + 0x5555); 
		*Temp = 0xAA;                   
		Temp =  bios + 0x2AAA; 
		*Temp = 0x55; 
		Temp =  bios + 0x5555; 
		*Temp = 0xA0;                   
		*dst = *src;
		toggle_ready_jedec(bios);
		if (*dst != *src)
			printf("BAD! dst 0x%lx val 0x%x src 0x%x\n",
			       (unsigned long)dst, *dst, *src);
		dst++, src++;
	}

	return(0);
}

#endif /* !__JEDEC_H__ */
