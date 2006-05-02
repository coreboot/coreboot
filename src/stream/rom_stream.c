#include <console/console.h>
#include <stdlib.h>
#include <stddef.h>
#include <stream/read_bytes.h>
#include <string.h>


#ifndef CONFIG_ROM_STREAM_START
#define CONFIG_ROM_STREAM_START 0xffff0000UL
#endif

/* well, this is a mess, and it will get fixed, but not right away. 
 * until we stop using 'ld' for building the rom image, that is. 
 * problem is, that on the sc520, ROM_STREAM_START has to be at 0x2000000. 
 * but if you set CONFIG_ROM_STREAM_START to that, then ld will try to 
 * build a giant image: 0x0-0x2000000, i.e. almost 4 GB. 
 * so make this non-static, non-const for now. 
 */

/*XXXXXXXXXXXXXX */
/*static const */unsigned char *rom_start = (unsigned char *)CONFIG_ROM_STREAM_START;
/*static const */unsigned char *rom_end   = (unsigned char *)(CONFIG_ROM_STREAM_START + PAYLOAD_SIZE - 1);
/*XXXXXXXXXXXXXX */
static const unsigned char *rom;

int stream_init(void)
{
	rom = rom_start;

	printk_debug("%6d:%s() - rom_stream: 0x%08lx - 0x%08lx\n",
		__LINE__, __FUNCTION__,
		(unsigned long)rom_start,
		(unsigned long)rom_end);
	return 0;
}


void stream_fini(void)
{
	return;
}

byte_offset_t stream_skip(byte_offset_t count)
{
	byte_offset_t bytes;
	bytes = count;
	if ((rom + bytes) > rom_end) {
		printk_warning("%6d:%s() - overflowed source buffer\n",
			__LINE__, __FUNCTION__);
		bytes = 0;
		if (rom <= rom_end) {
			bytes = (rom_end - rom) + 1;
		}
	}
	rom += bytes;
	return bytes;
}

byte_offset_t stream_read(void *vdest, byte_offset_t count)
{
	unsigned char *dest = vdest;
	const unsigned char *src = rom;
	byte_offset_t bytes;

	bytes = stream_skip(count);
	memcpy(dest, src, bytes);
	return bytes;
}
