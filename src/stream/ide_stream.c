#include <console/console.h>
#include <stdlib.h>
#include <stddef.h>
#include <stream/read_bytes.h>
#include <delay.h>
#include <string.h>

/* read a sector or a partial sector */
extern int ide_read(int drive, unsigned long block, void * buffer);
extern int ide_init(void);

static unsigned long offset;
int stream_init(void)
{
	int i,res;

        printk_debug ("Trying polled ide\n");
        printk_debug ("Waiting for ide disks to spin up\n");
        printk_notice ("This is a hard coded delay and longer than necessary.\n");
	for (i = 0; i < 2; i++) {
		printk_notice (".");
		delay(1);
	}
	printk_info ("\n");

#ifdef ONE_TRACK
	offset = (ONE_TRACK*512);
#elif defined(IDE_OFFSET)
	offset = IDE_OFFSET;
#else
	offset = 0x7e00;
#endif
	res = ide_init();
	delay(1);
	return res;
}

void stream_fini(void)
{
	return;
}

#ifdef IDE_SWAB
/* from string/swab.c */
void
swab (const char *from, char *to, int n)
{
	n &= ~1;
	while (n > 1)
	{
		const char b0 = from[--n], b1 = from[--n];
		to[n] = b0;
		to[n + 1] = b1;
	}
}
#endif

static unsigned char buffer[512];
static unsigned int block_num = 0;
static unsigned int first_fill = 1;
#ifndef IDE_BOOT_DRIVE
#define IDE_BOOT_DRIVE 0
#endif
static byte_offset_t stream_ide_read(void *vdest, byte_offset_t offset, byte_offset_t count)
{
	byte_offset_t bytes = 0;
	unsigned char *dest = vdest;

	//printk_debug("stream_ide_read count = %x\n", count);
	while (bytes < count) {
		unsigned int byte_offset, len;

		/* The block is not cached in memory or frist time called */
		if (block_num != offset / 512 || first_fill) {
			block_num  = offset / 512;
			printk_notice (".");
			ide_read(IDE_BOOT_DRIVE, block_num, buffer);
			first_fill = 0;
		}

		byte_offset = offset % 512;
		len = 512 - byte_offset;
		if (len > (count - bytes)) {
			len = (count - bytes);
		}

#ifdef IDE_SWAB
		swab(buffer + byte_offset, dest, len);
#else
		memcpy(dest, buffer + byte_offset, len);
#endif

		offset += len;
		bytes += len;
		dest += len;

	}
	return bytes;
}

byte_offset_t stream_read(void *vdest, byte_offset_t count)
{
	byte_offset_t len;

	len = stream_ide_read(vdest, offset, count);
	if (len > 0) {
		offset += len;
	}

	return len;
}

byte_offset_t stream_skip(byte_offset_t count)
{
	offset += count;
	return count;
}
