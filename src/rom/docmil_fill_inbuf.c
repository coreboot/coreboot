#include <cpu/p5/io.h>
#include <printk.h>
#include <stdlib.h>
#include <subr.h>
#include <stddef.h>
#include <rom/read_bytes.h>
#include <string.h>

#ifndef DOC_KERNEL_START
#define DOC_KERNEL_START 65536
#endif

#ifndef DOC_MIL_BASE
#define DOC_MIL_BASE  0xffffe000
#endif

static unsigned char *inbuf;	/* input buffer */
static unsigned int insize;	/* valid bytes in inbuf */
static unsigned int inptr;	/* index of next byte to be processed in inbuf */

static unsigned char *nvram;
static int block_count;
static int firstfill = 1;

static void memcpy_from_doc_mil(void *dest, const void *src, size_t n);
static volatile unsigned char *doc_mil = (unsigned char *) DOC_MIL_BASE;

static unsigned char *ram;
#define K64 (64 * 1024)

static int 
fill_inbuf(void)
{
	printk_spew(__FUNCTION__ "\n");
 	if (firstfill) {
		if ((ram = malloc(K64)) == NULL) {
			printk_emerg("%6d:%s() - ram malloc failed\n",
				     __LINE__, __FUNCTION__);
			return (0);
		}

		printk_debug("%6d:%s() - ram buffer:0x%p\n",
			     __LINE__, __FUNCTION__, ram);

		block_count = 0;
		firstfill = 0;
		nvram = (unsigned char *) DOC_KERNEL_START;
	}

	memcpy_from_doc_mil(ram, nvram, K64);

	printk_debug("%6d:%s() - nvram:0x%p  block_count:%d\n",
		     __LINE__, __FUNCTION__, nvram, block_count);

	nvram += K64;
	inbuf = ram;
	insize = K64;
	inptr = 1;

	post_code(0xd0 + block_count);
	block_count++;
        printk_debug("inbuf[0] is 0x%x\n", inbuf[0]);
	return inbuf[0];
}

static void
memcpy_from_doc_mil(void *dest, const void *src, size_t n)
{
	int i;
	unsigned long address = (unsigned long) src;

	for (i = n; i > 0; i -= 0x200) {
		unsigned short c = 0x1000;
		volatile unsigned char dummy;

		/* issue Read00 flash command */
		*(volatile unsigned char *) (doc_mil + 0x1004) = 0x03;
		*(volatile unsigned char *) (doc_mil + 0x800)  = 0x00;
		*(volatile unsigned char *) (doc_mil + 0x101e) = 0x00;
		*(volatile unsigned char *) (doc_mil + 0x1004) = 0x01;

		/* issue Address to flash */
		*(volatile unsigned char *) (doc_mil + 0x1004) = 0x05;
		*(volatile unsigned char *) (doc_mil + 0x800)  = address & 0xff;
		*(volatile unsigned char *) (doc_mil + 0x800)  = (address >> 9) & 0xff;
		*(volatile unsigned char *) (doc_mil + 0x800)  = (address >> 17) & 0xff;
		*(volatile unsigned char *) (doc_mil + 0x101e) = 0x00;
		*(volatile unsigned char *) (doc_mil + 0x1004) = 0x01;

		/* We are using the "side effect" of the assignment to force GCC reload 
		 * *(doc_mil + 0x1004) on each iteration */
		while (!((dummy = *(volatile unsigned char *) (doc_mil + 0x1004)) & 0x80) && --c)
			/* wait for chip response */;

		/* copy 512 bytes of data from CDSN_IO registers */
		dummy = *(volatile unsigned char *) (doc_mil + 0x101d);
		memcpy(dest, (const void *) (doc_mil + 0x800), 0x200);

		dest += 0x200;
		address += 0x200;
	}
	printk_debug("done " __FUNCTION__ "\n");

}

/* FIXME this is a very lazy ugly port of the new interface to the doc millenium 
 * find a good way to implement this...
 */

#define get_byte()  (inptr < insize ? inbuf[inptr++] : fill_inbuf())

static int
init_bytes(void)
{
	printk_debug(__FUNCTION__ "\n");
	// it is possible that we can get in here and the 
	// doc has never been reset. So go ahead and reset it again.

	*(volatile unsigned char *) (doc_mil + 0x1002) = 0x84;
	*(volatile unsigned char *) (doc_mil + 0x1002) = 0x84;
	*(volatile unsigned char *) (doc_mil + 0x1002) = 0x85;
	*(volatile unsigned char *) (doc_mil + 0x1002) = 0x85;

	return 1;
}

static void
fini_bytes(void)
{
    printk_debug(__FUNCTION__ " \n");
}

static byte_offset_t
read_bytes(void *vdest, byte_offset_t count)
{
	byte_offset_t bytes = 0;
	unsigned char *dest = vdest;
	

	while (bytes++ < count) {
		unsigned char c = get_byte();
		printk_spew("%d:0x%x\n", inptr-1, c);
				
		*(dest++) = c;
	}
	printk_spew(__FUNCTION__ " vdest %p return count %d\n", 
			vdest, count);
	return count;
}

static byte_offset_t
skip_bytes(byte_offset_t count)
{
	byte_offset_t bytes = 0;
	printk_spew(__FUNCTION__ " count %d\n", count);

	while (bytes++ < count) {
		unsigned char byte;
		byte = get_byte();
	}
	return count;
}

static struct stream doc_mil_stream __stream = {
	.init = init_bytes,
	.read = read_bytes,	       
	.skip = skip_bytes,
	.fini = fini_bytes,
};
