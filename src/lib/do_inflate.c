#include <printk.h>
#include <string.h>
#include <stdlib.h>
#include <rom/read_bytes.h>
#include <subr.h>
#include "definitions.h"
#include "do_inflate.h"

#define MALLOC_WINDOW 1
/*
 * gzip support routine declartions..
 * =========================================================
 */

static void flush_window(void);

static long bytes_out;		/* total bytes compressed */
static unsigned outcnt;		/* bytes in output buffer */

#define WSIZE 0x8000		/* Window size must be at least 32k, and a power of two */
#if !MALLOC_WINDOW
static unsigned char window[WSIZE];	/* Sliding window buffer */
#else
static unsigned char *window;		/* Sliding window buffer */
#endif

static unsigned char get_byte(void)
{
	unsigned char result;
	if (streams->read(&result, 1) != 1) {
		printk_err("Unexpected! Out of bytes...\n");
		while(1) ;
	}
	return result;
}


/*
 * gzip declarations
 */

#define OF(args)  args
#define STATIC static


#define memzero(s, n)     memset ((s), 0, (n))

typedef unsigned char uch;
typedef unsigned short ush;
typedef unsigned long ulg;


#ifdef TRACEV
#  define Trace(x) printk_debug(x)
#  define Tracev(x) {if (verbose) printk_debug(x);}
#  define Tracevv(x) {if (verbose>1) printk_debug(x);}
#  define Tracec(c,x) {if (verbose && (c)) printk_debug(x);}
#  define Tracecv(c,x) {if (verbose>1 && (c)) printk_debug(x);}
#else
#  define Trace(x)
#  define Tracev(x)
#  define Tracevv(x)
#  define Tracec(c,x)
#  define Tracecv(c,x)
#endif

#include "inflate.c"

/* Variables that gunzip doesn't need to see... */
static unsigned char *output_data;
static unsigned long output_ptr;


/* ===========================================================================
 * Write the output window window[0..outcnt-1] and update crc and bytes_out.
 * (Used for the decompressed data only.)
 */
static void flush_window(void)
{
	ulg c = crc;		/* temporary variable */
	unsigned n;
	uch *in, *out, ch;

	in = window;
	out = &output_data[output_ptr];
	printk_debug("flush 0x%08x count 0x%08x\n", (unsigned long) out, outcnt);

	for (n = 0; n < outcnt; n++) {
		ch = *out++ = *in++;
		c = crc_32_tab[((int) c ^ ch) & 0xff] ^ (c >> 8);
	}
	crc = c;
	bytes_out += (ulg) outcnt;
	output_ptr += (ulg) outcnt;
	outcnt = 0;
}

void gunzip_setup(void)
{
	/* common globals -- don't rely on init! */
	printk_debug("%s\n", __FUNCTION__ );
	outcnt = 0;
	bytes_out = 0;
	output_ptr = 0;

#if MALLOC_WINDOW
	window = malloc(WSIZE);
#endif
	output_data = (char *) KERNEL_START;
	printk_debug("output data is 0x%08x\n", (unsigned long) output_data);
	makecrc();
}
