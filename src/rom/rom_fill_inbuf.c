#ifdef USE_GENERIC_ROM

#include <printk.h>
#include <stdlib.h>
#include <subr.h>
#include <stddef.h>
#include <rom/fill_inbuf.h>

#include <string.h>

#ifndef ZKERNEL_START
#define ZKERNEL_START 0xfff80000
#endif

#ifndef ZKERNEL_MASK
#define ZKERNEL_MASK 0x0000ffff
#endif

static unsigned char *zkernel_start = (unsigned char *)ZKERNEL_START;
static unsigned long zkernel_mask = ZKERNEL_MASK;

static unsigned char *nvram;
static int block_count;
static int firstfill = 1;

#if defined(INBUF_COPY)
static unsigned char *ram;
#endif

#define K64 (64 * 1024)

int fill_inbuf(void)
{
	extern unsigned char *inbuf;
	extern unsigned int insize;
	extern unsigned int inptr;

	if (firstfill) {
		block_count = 0;
		firstfill = 0;
#ifdef INBUF_COPY
		ram = malloc(K64);
#endif
	}

	if (block_count > 31) {
		printk(KERN_EMERG "%6d:%s() - overflowed source buffer\n",
		       __LINE__, __FUNCTION__);
		inbuf = zkernel_start;
		inptr = 0;
		insize = 0;
		return (0);
	}

	if (!block_count) {
		nvram = zkernel_start;

#ifdef INBUF_COPY
		if (!ram) {
			printk(KERN_EMERG "%6d:%s() - "
			       "ram malloc failed\n",
			       __LINE__, __FUNCTION__);
			inbuf = zkernel_start;
			inptr = 0;
			insize = 0;
			return (0);
		}

		DBG("%6d:%s() - ram buffer:0x%08x\n",
		       __LINE__, __FUNCTION__, ram);
#endif
		DBG("%6d:%s() - zkernel_start:0x%08x  "
		       "zkernel_mask:0x%08x\n",
		       __LINE__, __FUNCTION__,
		       zkernel_start, zkernel_mask);
	} else {
		nvram += K64;

		while (!(zkernel_mask & (1 << block_count))) {
			DBG("%6d:%s() - skipping block %d\n",
			       __LINE__, __FUNCTION__, block_count);

			block_count++;
			nvram += K64;

			if (block_count > 31) {
				printk(KERN_EMERG "%6d:%s() - "
				       "overflowed source buffer\n",
				       __LINE__, __FUNCTION__);
				inbuf = zkernel_start;
				inptr = 0;
				insize = 1;
				return (0);
			}
		}
	}

#ifdef INBUF_COPY
	memcpy(ram, nvram, K64);
#endif
	DBG("%6d:%s() - nvram:0x%p  block_count:%d\n",
	       __LINE__, __FUNCTION__, nvram, block_count);

#ifdef INBUF_COPY
	inbuf = ram;
#else
	inbuf = nvram;
#endif
	insize = K64;
	inptr = 1;
	post_code(0xd0 + block_count);
	block_count++;
	return inbuf[0];
}


#endif /* USE_GENERIC_ROM */
