#if defined(USE_DOC_MIL) || defined(USE_DOC_MIL_PLUS)

#include <cpu/p5/io.h>
#include <printk.h>
#include <stdlib.h>
#include <subr.h>
#include <stddef.h>
#include <rom/fill_inbuf.h>

#ifndef DOC_KERNEL_START
#define DOC_KERNEL_START 65536
#endif

#ifndef DOC_MIL_BASE
#define DOC_MIL_BASE  0xffffe000
#endif

static unsigned char *nvram;
static int block_count;
static int firstfill = 1;

static void memcpy_from_doc_mil(void *dest, const void *src, size_t n);
static unsigned char *doc_mil = (unsigned char *) DOC_MIL_BASE;
#ifdef CHECK_DOC_MIL
static unsigned char *checkbuf;
#endif /* CHECK_DOC_MIL */

static unsigned char *ram;
#define K64 (64 * 1024)


int
fill_inbuf(void)
{
	if (firstfill) {
		if ((ram = malloc(K64)) == NULL) {
			printk_emerg("%6d:%s() - ram malloc failed\n",
			      __LINE__, __FUNCTION__);
			return (0);
		}

#ifdef CHECK_DOC_MIL
		if ((checkbuf = malloc(K64)) == NULL) {
			printk_emerg("%6d:%s() - checkbuf malloc failed\n",
			      __LINE__, __FUNCTION__);
			printk_emerg("Checking disabled\n");
		}
#endif

		printk_debug("%6d:%s() - ram buffer:0x%p\n",
		       __LINE__, __FUNCTION__, ram);

		block_count = 0;
		firstfill = 0;
		nvram = (unsigned char *) DOC_KERNEL_START;
	}

	memcpy_from_doc_mil(ram, nvram, K64);

#ifdef CHECK_DOC_MIL
	if (checkbuf) {
		memcpy_from_doc_mil(checkbuf, nvram, K64);
		if (memcmp(checkbuf, ram, K64)) {
			printk_emerg("CHECKBUF FAILS for doc mil!\n");
			printk_emerg( "address 0x%x\n", nvram);
		}
	}
	{ 
		int i;
		printk_info( "First 16 bytes of block: ");
		for (i = 0; i < 16; i++)
			printk_info("0x%x ", ram[i]);
		printk_info( "\n");
	}
#endif

	printk_debug("%6d:%s() - nvram:0x%p  block_count:%d\n",
	       __LINE__, __FUNCTION__, nvram, block_count);

	nvram += K64;
	inbuf = ram;
	insize = K64;
	inptr = 1;

	post_code(0xd0 + block_count);
	block_count++;
	return inbuf[0];
}

static void
memcpy_from_doc_mil(void *dest, const void *src, size_t n)
{
	int i;
	unsigned long address = (unsigned long) src;

	for (i = n; i >= 0; i -= 0x200) {
		unsigned short c = 0x1000;
		volatile unsigned char dummy;

		/* issue Read00 flash command */
		*(unsigned char *) (doc_mil + 0x1004) = 0x03;
		*(unsigned char *) (doc_mil + 0x800)  = 0x00;
		*(unsigned char *) (doc_mil + 0x101e) = 0x00;
		*(unsigned char *) (doc_mil + 0x1004) = 0x01;

		/* issue Address to flash */
		*(unsigned char *) (doc_mil + 0x1004) = 0x05;
		*(unsigned char *) (doc_mil + 0x800)  = address & 0xff;
		*(unsigned char *) (doc_mil + 0x800)  = (address >> 9) & 0xff;
		*(unsigned char *) (doc_mil + 0x800)  = (address >> 17) & 0xff;
		*(unsigned char *) (doc_mil + 0x101e) = 0x00;
		*(unsigned char *) (doc_mil + 0x1004) = 0x01;

		/* We are using the "side effect" of the assignment to force GCC reload 
		 * *(doc_mil + 0x1004) on each iteration */
		while (!((dummy = *(unsigned char *) (doc_mil + 0x1004)) & 0x80) && --c)
			/* wait for chip response */;

		/* copy 512 bytes of data from CDSN_IO registers */
		dummy = *(unsigned char *) (doc_mil + 0x101d);
		memcpy(dest, doc_mil + 0x800, 0x200);

		dest += 0x200;
		address += 0x200;
	}
}

#endif /* USE_DOC_MIL */
