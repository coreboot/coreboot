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
	for (i = 0; i < 15; i++) {
		printk_debug(".");
		delay(1);
	}
	printk_debug("\n");

	offset = 0x7e00;
	return ide_init();
}

static void fini_bytes(void)
{
	return;
}

static unsigned char buffer[512];
static unsigned int block_num = 0;
static unsigned int first_fill = 1;

static byte_offset_t ide_read(void *vdest, byte_offset_t offset, byte_offset_t count)
{
	byte_offset_t bytes = 0;
	unsigned char *dest = vdest;

	//printk_debug("ide_read count = %x\n", count);
	while (bytes < count) {
		unsigned int byte_offset, len;
		int result;
		int i, j;

		/* The block is not cached in memory or frist time called */
		if (block_num != offset / 512 || first_fill) {
			block_num  = offset / 512;
			ide_read_sector(0, buffer, block_num,
					0, 512);
			first_fill = 0;
#if 1
			//printk_debug("ide_read offset = %x\n", offset);
			//printk_debug("ide_read block_num = %x\n", block_num);
			for (i = 0; i < 16; i++) {
			    for (j = 0; j < 16; j++) {
				printk_debug("%02x ", buffer[i*16 +j]);
			    }
			    printk_debug("\n");
			}

			printk_debug("\n");
#endif
		}

		byte_offset = offset % 512;
		len = 512 - byte_offset;
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
