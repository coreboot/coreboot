#include <printk.h>
#include <stdlib.h>
#include <subr.h>
#include <stddef.h>
#include <rom/read_bytes.h>

#include <string.h>
#include <floppy_subr.h>

static unsigned long offset;

/* 
 * ONE_TRACK is 18 sectors (18*512) which is 
 * really half the sectors for a track 
 * on a 2 head floppy.  Unfortunately DISK_H1440_TRACK is defined
 * in the floppy_subr.c C file and not in a header so we define
 * it again here for clarity.
 */
#define DISK_H1440_SECT	18
#define ONE_TRACK	DISK_H1440_SECT*512
static unsigned char buffer[ONE_TRACK];
static unsigned int block_num = 0;
static unsigned int first_fill = 1;

static byte_offset_t floppy_read_b(void *vdest, byte_offset_t offset, byte_offset_t count);

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
	len = floppy_read_b(vdest, offset, count);
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


static byte_offset_t floppy_read_b(void *vdest, byte_offset_t offset, byte_offset_t count)
{
	byte_offset_t bytes = 0;
	unsigned char *dest = vdest;

	//printk_debug("floppy_read count = %x\n", count);
	while (bytes < count) {
		unsigned int byte_offset, len;

		/* The block is not cached in memory or first time called */
		if (block_num != offset / (ONE_TRACK) || first_fill) {
			block_num  = offset / (ONE_TRACK);
			floppy_read(buffer, block_num*(ONE_TRACK), (ONE_TRACK));
			first_fill = 0;
			//printk_debug("floppy_read_b dest= 0x%x offset= %d block_num= %d\n", dest, offset, block_num);
		}

		byte_offset = offset % (ONE_TRACK);
		len = (ONE_TRACK) - byte_offset;
		if (len > (count - bytes)) {
			len = (count - bytes);
		}

		memcpy(dest, buffer + byte_offset, len);

		offset += len;
		bytes += len;
		dest += len;

	}
	return bytes;
}
