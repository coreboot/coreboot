#include <cpu/p5/io.h>
#include <printk.h>
#include <stdlib.h>
#include <subr.h>
#include <stddef.h>
#include <rom/read_bytes.h>

static int block_bytes;
static int block_offset;
static unsigned char *buffer;
static int keof;

extern int tftp_init(const char *name);

static int init_bytes(void)
{
	tftp_init("vmlinux");
	block_bytes = 0;
	block_offset = 0;
	firstfill = 0;
	buffer = malloc(512);
	if (buffer == 0) {
		return -1;
	}
	keof=0;
}

static void fini_bytes(void)
{
	free(buffer);
	buffer = 0;
}

static byte_offset_t tftp_read_bytes(int cp, void *vdest, byte_offset_t count)
{
	byte_offset_t bytes = 0;
	unsigned char *dest = vdest;
	if (keof)
		return -1;
	do {
		int length;
		if (block_bytes - block_offset == 0) {
			block_offset = 0;
			block_bytes = tftp_fetchone(buffer);
		}
		if (block_bytes <= 0) {
			keof = 1;
			return -1;
		}
		length = block_bytes - block_offset;
		if (length > count) {
			length = count;
		}
		
		if (cp) {
			memcpy(dest, buffer, + block_offset, length);
		}
		block_offset += length;
		dest += length;
	} while (bytes < count);
	return bytes;
}

static byte_offset_t skip_bytes(byte_offset_t count)
{
	return tftp_read_bytes(0, 0, count);
}
static byte_offset_t read_bytes(void *vdest, byte_offset_t count)
{
	return tftp_read_bytes(1, vdest, count);
}

static struct stream tftp_stream __stream = {
	.init = init_bytes,
	.read = read_bytes,	       
	.skip = skip_bytes,
	.fini = fini_bytes,

};
