#ifdef USE_TFTP

#include <cpu/p5/io.h>
#include <printk.h>
#include <stdlib.h>
#include <subr.h>
#include <stddef.h>
#include <rom/fill_inbuf.h>

#ifndef DOC_KERNEL_START
#define DOC_KERNEL_START 65536
#endif

static unsigned char *nvram;
static int block_count;
static int firstfill = 1;

static void memcpy_from_doc_mil(void *dest, const void *src, size_t n);
static unsigned char *doc_mil = (unsigned char *) 0xffffe000;
#ifdef CHECK_DOC_MIL
static unsigned char *checkbuf;
#endif /* CHECK_DOC_MIL */

static unsigned char *ram;
#define K64 (64 * 1024)


int fill_inbuf(void)
{
	extern unsigned char *inbuf;
	extern unsigned int insize;
	extern unsigned int inptr;

	int rc;
	static keof;

	if(keof)
		return(-1);

	if(firstfill) {	
		tftp_init("vmlinux");
		block_count = 0;
		firstfill = 0;
		inbuf = ram = malloc(512);
		keof=0;
	}

	rc = tftp_fetchone(ram);
	insize = rc;
	inptr = 1;
	return inbuf[0];
}

#endif	// USE_TFTP
