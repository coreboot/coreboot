#ifdef USE_TSUNAMI_TIGBUS_ROM

#include <printk.h>
#include <stdlib.h>
#include <subr.h>
#include <stddef.h>
#include <rom/fill_inbuf.h>

#include <arch/io.h>
#include <northbridge/alpha/tsunami/core_tsunami.h>

#ifndef TIG_KERNEL_START
#define TIG_KERNEL_START 0x20000
#endif

static unsigned long nvram;
static int block_count;
static int firstfill = 1;
static unsigned char *ram;

#define K64 (64 * 1024)

#define MAX_TIG_FLASH_SIZE (16*1024*1024)
static void tsunami_flash_copy_from(void *addr, unsigned long offset, long len)
{

	unsigned char *dest;
	dest = addr;
	while(len && (offset < MAX_TIG_FLASH_SIZE)) {
		*dest = tsunami_tig_readb(offset);
		offset++;
		dest++;
		len--;
	}
}

int fill_inbuf(void)
{
	extern unsigned char *inbuf;
	extern unsigned int insize;
	extern unsigned int inptr;

	if (firstfill) {
		block_count = 0;
		firstfill = 0;
		ram = malloc(K64);
		if (!ram) {
			printk(KERN_EMERG "%6d:%s() - "
			       "ram malloc failed\n",
			       __LINE__, __FUNCTION__);
			return 0;
		}
	}

	if (block_count > 31) {
		printk(KERN_EMERG "%6d:%s() - overflowed source buffer\n",
		       __LINE__, __FUNCTION__);
		insize = 0;
		return (0);
	}
	if (!block_count) {
		nvram = TIG_KERNEL_START;
		DBG("%6d:%s() - ram buffer:0x%08x\n",
		       __LINE__, __FUNCTION__, ram);
		DBG("%6d:%s() - TIG_KERNEL_START:0x%08x\n",
		       __LINE__, __FUNCTION__,
		       TIG_KERNEL_START);
	}


	tsunami_flash_copy_from(ram, nvram, K64);
	DBG("\n%6d:%s() - nvram:0x%lx  block_count:%d\n",
	       __LINE__, __FUNCTION__, nvram, block_count);

#if 0
	{
		int i;
		for(i = 0; i < K64; i+= 16) {
			printk("%05x: %02x %02x %02x %02x %02x %02x %02x %02x "
				"%02x %02x %02x %02x %02x %02x %02x %02x\n",
				(block_count << 16)+i,
				ram[i+0], ram[i+1], ram[i+2], ram[i+3],
				ram[i+4], ram[i+5], ram[i+6], ram[i+7],
				ram[i+8], ram[i+9], ram[i+10], ram[i+11],
				ram[i+12], ram[i+13], ram[i+14],ram[i+15]);
		}
	}
#endif
	nvram += K64;
	inbuf = ram;
	insize = K64;
	inptr = 1;
	post_code(0xd0 + block_count);
	block_count++;
	return inbuf[0];
}

#endif /* USE_TSUNAMI_TIGBUS_ROM */
