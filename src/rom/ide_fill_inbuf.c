#include <printk.h>
#include <stdlib.h>
#include <subr.h>
#include <stddef.h>
#include <rom/read_bytes.h>
#include <delay.h>

#include <string.h>
#include <floppy_subr.h>

/* read a sector or a partial sector */
extern int ide_read_sector(int drive, void * buffer, unsigned int block, int byte_offset, 
	int n_bytes);
extern int ide_init(void);



static unsigned long offset;
static int init_bytes(void)
{
	int i;
        printk_debug ("Trying polled ide\n");
        printk_debug ("Waiting for ide disks to spin up\n");
        printk_debug ("This is a hard coded delay and longer than necessary.\n");
	for(i = 0; i < 25; i++) {
		printk_debug(".");
		delay(1);
	}
	printk_debug("\n");
	offset = 0;
	return ide_init();
}

static void fini_bytes(void)
{
	return;
}

static byte_offset_t ide_read(void *vdest, byte_offset_t offset, byte_offset_t count)
{
	byte_offset_t bytes = 0;
	unsigned char *dest = vdest;
	while(bytes < count) {
		unsigned int block, byte_offset, len;
		int result;
		block = offset / 512;
		byte_offset = offset %512;
		len = 512 - byte_offset;
		if (len > (count - bytes)) {
			len = (count - bytes);
		}
		result = ide_read_sector(0, dest, block , byte_offset, len);
		if (result != 0) {
			return bytes;
		}
		offset += len;
		bytes += len;
		dest += len;
	}
	return bytes;
}

static byte_offset_t read_bytes(void *vdest, byte_offset_t count)
{
	byte_offset_t len;
	len = ide_read(vdest, offset, count);
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

static struct stream ide_stream __stream = {
	.init = init_bytes,
	.read = read_bytes,	       
	.skip = skip_bytes,
	.fini = fini_bytes,
};
