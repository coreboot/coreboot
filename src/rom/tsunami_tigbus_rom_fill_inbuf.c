#include <printk.h>
#include <stdlib.h>
#include <subr.h>
#include <stddef.h>
#include <rom/read_bytes.h>

#include <arch/io.h>
#include <northbridge/alpha/tsunami/core_tsunami.h>

#ifndef TIG_KERNEL_START
#define TIG_KERNEL_START 0x20000
#endif

static unsigned long offset;

#define MAX_TIG_FLASH_SIZE (16*1024*1024)
static void tsunami_flash_copy_from(void *addr, unsigned long offset, long len)
{

	unsigned char *dest;
	dest = addr;
	while(len && (offset < MAX_TIG_FLASH_SIZE)) {
		*dest = tsunami_tig_readb(offset);
		offset++;
		dest++;
		len--;
	}
}

static int init_bytes(void)
{
	offset = 0;
	printk_debug("%6d:%s() - TIG_KERNEL_START:0x%08x\n",
		__LINE__, __FUNCTION__,
		TIG_KERNEL_START);
	return 0;
}
static void fini_bytes(void)
{
	return;
}
static byte_offset_t skip_bytes(byte_offset_t count)
{
	unsigned long new_offset;
	byte_offset_t len;
	new_offset = offset + count;
	if (new_offset > MAX_TIG_FLASH_SIZE) {
		new_offset = MAX_TIG_FLASH_SIZE;
	}
	len = new_offset - offset;
	offset = new_offset;
	return len;
}

static byte_offset_t read_bytes(void *vdest, byte_offset_t count)
{
	long length;
	length = MAX_TIG_FLASH_SIZE - offset;
	if (count < 0)
		count = 0;
	if (count < length) {
		length = count;
	}
	tsunami_flash_copy_from(vdest, offset, length);
	offset += length;
	return length;
}


static struct stream tsunami_tigbus_rom_stream __stream = {
	.init = init_bytes,
	.read = read_bytes,	       
	.skip = skip_bytes,
	.fini = fini_bytes,

};
