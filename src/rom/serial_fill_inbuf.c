#ifdef USE_SERIAL_FILL_INBUF

#include <printk.h>
#include <stdlib.h>
#include <subr.h>
#include <stddef.h>
#include <rom/fill_inbuf.h>

#include <string.h>
#include <serial_subr.h>


static int firstfill = 1;
static unsigned char *ram;

#define K64 (64*1024)

int fill_inbuf(void)
{
	int rc;
	if (firstfill) {
		firstfill = 0;
		ram = malloc(K64);
	}
	inbuf = ram;
	insize = ttys0_rx_bytes(inbuf, K64);
	inptr = 1;
	return inbuf[0];
}


#endif /* USE_SERIAL_FILL_INBUF */
