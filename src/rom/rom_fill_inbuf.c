#include <printk.h>
#include <stdlib.h>
#include <subr.h>
#include <stddef.h>
#include <rom/read_bytes.h>

#include <string.h>

#ifndef ZKERNEL_START
#define ZKERNEL_START 0xfff00000
#endif

#ifndef ZKERNEL_MASK
#define ZKERNEL_MASK 0x0000ffff
#endif

/* The inbuf copy option has been killed... */

static unsigned char *zkernel_start = (unsigned char *)ZKERNEL_START;
static unsigned long zkernel_mask = ZKERNEL_MASK;

static unsigned char *nvram;
static int block_count;
static int block_offset;

#define K64 (64 * 1024)

static int init_bytes(void)
{
	block_count = 0;
	block_offset = 0;
	nvram = zkernel_start;

	printk_debug("%6d:%s() - zkernel_start:0x%08x  "
		"zkernel_mask:0x%08x\n",
		__LINE__, __FUNCTION__,
		zkernel_start, zkernel_mask);

	return 0;
}


static void fini_bytes(void)
{
	return;
}

static byte_offset_t rom_read_bytes(int cp, void *vdest, byte_offset_t count)
{
	unsigned char *dest = vdest;
	byte_offset_t bytes = 0;
	while (bytes < count) {
		int length;
		if (block_offset == K64) {
			block_offset = 0;
			block_count++;
			nvram+= K64;
		}
		if (!(zkernel_mask & (1 << block_count))) {
			printk_debug("%6d:%s() - skipping block %d\n",
				__LINE__, __FUNCTION__, block_count);
			continue;
		}
		if (block_count > 31) {
			printk_emerg( "%6d:%s() - overflowed source buffer\n",
				__LINE__, __FUNCTION__);
			return bytes;
		}
		length = K64 - block_offset;
		if (length > (count - bytes)) {
			length = count - bytes;
		}
		if (cp) {
			memcpy(dest, nvram + block_offset, length);
		}
		dest += length;
		block_offset += length;
		bytes += length;
	}
	return bytes;
}

static byte_offset_t skip_bytes(byte_offset_t count)
{
	return rom_read_bytes(0, 0, count);
}

static byte_offset_t read_bytes(void *vdest, byte_offset_t count)
{
	return rom_read_bytes(1, vdest, count);
}

static struct stream rom_stream __stream = {
	.init = init_bytes,
	.read = read_bytes,
	.skip = skip_bytes,
	.fini = fini_bytes,
};
