#include <string.h>
#include <stdlib.h>

/*
 * gzip support routine declartions..
 * =========================================================
 */

#ifdef DEBUG
#  define Trace(x)
#  define Tracev(x)
#  define Tracevv(x)
#  define Tracec(c,x)
#  define Tracecv(c,x)
#  define DBG(x) printf x
#else
#  define Trace(x)
#  define Tracev(x)
#  define Tracevv(x)
#  define Tracec(c,x)
#  define Tracecv(c,x)
#  define DBG(x)
#endif

void error(char *str)
{
	DBG(("%s\n", str));
}

static unsigned char *inbuf;	/* input buffer */
static unsigned int insize;	/* valid bytes in inbuf */
static unsigned int inptr;	/* index of next byte to be processed in inbuf */

#if !defined(DEBUG)
#define get_byte()  (inptr < insize ? inbuf[inptr++] : 0)
#else
static unsigned char get_byte(void)
{
	static int count;
	unsigned char byte = (inptr < insize ? inbuf[inptr++] : 0);
#if 0
	printf("%02x ", byte);
	if ((++count & 0x0f) == 0) {
		printf("\n");
	}
#endif
	return byte;
}

#endif

static void flush_window(void);

static long bytes_out;		/* total bytes compressed */
static unsigned outcnt;		/* bytes in output buffer */

#define WSIZE 0x8000		/* Window size must be at least 32k, and a power of two */
static unsigned char window[WSIZE];	/* Sliding window buffer */

/*
 * gzip declarations
 */

#define OF(args)  args
#define STATIC static


#define memzero(s, n)     memset ((s), 0, (n))

typedef unsigned char uch;
typedef unsigned short ush;
typedef unsigned long ulg;



#include "inflate.c"


/* Variables that gunzip doesn't need to see... */
static unsigned char *output_ptr;
static unsigned long end_offset;
static struct unzip_region {
	unsigned long start;
	unsigned long end_offset;
} unzip_region;

/* Data provided by the header */
extern unsigned char zipped_data[];
extern unsigned char zipped_data_end[];
extern unsigned char entry;
/* Assembly language routines */
extern void jmp_to_program_entry(void *);

/* ===========================================================================
 * Write the output window window[0..outcnt-1] and update crc and bytes_out.
 * (Used for the decompressed data only.)
 */
static void flush_window(void)
{
	ulg c = crc;		/* temporary variable */
	unsigned n;
	unsigned long limit;
	uch *in, *out, ch;

	limit = outcnt;


	n = 0;
	in = window;
	while (n < outcnt) {
		limit = end_offset - bytes_out +n;
		if (limit > outcnt) {
			limit = outcnt;
		}
		out = output_ptr;
		DBG(("flush 0x%08lx start 0x%08lx limit 0x%08lx\n",
			(unsigned long) out, (unsigned long)n, limit));
		for (; n < limit; n++) {
			ch = *out++ = *in++;
			c = crc_32_tab[((int) c ^ ch) & 0xff] ^ (c >> 8);
		}
		crc = c;
		bytes_out += (out - output_ptr);
		output_ptr = out;
		if (bytes_out == end_offset) {
			if (output_ptr == (unsigned char *)(&unzip_region+1)) {
				output_ptr = (unsigned char *)(unzip_region.start);
				end_offset = unzip_region.end_offset;
			} else {
				output_ptr = (unsigned char *)&unzip_region;
				end_offset += sizeof(unzip_region);
			}
		}
	}
	outcnt = 0;
}


void gunzip_setup(void)
{
	DBG(("gunzip_setup\n"));
	outcnt = 0;
	bytes_out = 0;

	end_offset = sizeof(unzip_region);
	output_ptr = (unsigned char *)&unzip_region;

	inbuf = &zipped_data[0];
	insize = zipped_data_end - zipped_data;
	inptr = 0;

	makecrc();
	DBG(("gunzip_setup_done\n"));
}


int kunzip(int argc, char **argv)
{
	DBG(("kunzip\n"));
	gunzip_setup();
	DBG(("pre_gunzip\n"));
	if (gunzip() != 0) {
		error("gunzip failed");
		while(1) {}
		return -1;
	}
	DBG(("pre_jmp_to_program_entry: %p\n", &entry ));
	jmp_to_program_entry(&entry);
	return 0;
}
