#include <printk.h>
#include <stdlib.h>
#include <subr.h>
#include <stddef.h>
#include <rom/read_bytes.h>

#include <string.h>
#include <floppy_subr.h>

static unsigned long offset;

static int init_bytes(void)
{
	offset = 0;
	return floppy_init();
}

static void fini_bytes(void)
{
	floppy_fini();
}

static byte_offset_t read_bytes(void *vdest, byte_offset_t count)
{
	byte_offset_t len;
	len = floppy_read(vdest, offset, count);
	if (len > 0) {
		offset += len;
	}
	return len;
}

static byte_offset_t skip_bytes(byte_offset_t count)
{
	offset += count;
	return count;
}

static struct stream floppy_bytes __stream = {
	.init = init_bytes,
	.read = read_bytes,	       
	.skip = skip_bytes,
	.fini = fini_bytes,
};
