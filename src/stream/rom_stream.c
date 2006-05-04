#include <console/console.h>
#include <stdlib.h>
#include <stddef.h>
#include <stream/read_bytes.h>
#include <string.h>

#if CONFIG_COMPRESSED_ROM_STREAM
// include generic nrv2b
#include "../lib/nrv2b.c"
extern unsigned char _heap, _eheap;
#endif

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

unsigned char *rom_start = (void *)CONFIG_ROM_STREAM_START;
unsigned char *rom_end   = (void *)(CONFIG_ROM_STREAM_START + PAYLOAD_SIZE - 1);
static const unsigned char *rom;

int stream_init(void)
{
#if CONFIG_COMPRESSED_ROM_STREAM
        unsigned long dest;
	unsigned long ilen;
        unsigned long olen;
#endif

	printk_debug("rom_stream: 0x%08lx - 0x%08lx\n",
		(unsigned long)rom_start,
		(unsigned long)rom_end);

#if CONFIG_COMPRESSED_ROM_STREAM

        dest = &_eheap; /* need a good address on RAM */

#if _RAMBASE<0x00100000
	olen = *(unsigned int *)dest;
#if (CONFIG_CONSOLE_VGA==1) || (CONFIG_PCI_ROM_RUN == 1)
	if((dest < 0xa0000) && ((dest+olen)>0xa0000)) {
		dest = (CONFIG_LB_MEM_TOPK<<10);
	}
#endif
        if((dest < 0xf0000) && ((dest+olen)>0xf0000)) { //linuxbios tables etc
                dest = (CONFIG_LB_MEM_TOPK<<10);
        }
#endif

        printk_debug("Uncompressing to RAM 0x%08lx ", dest);
        olen = unrv2b((uint8_t *) rom_start, (uint8_t *)dest, &ilen );
	printk_debug(" ilen = 0x%08lx olen = 0x%08lx done.\n", ilen, olen);
	rom_end = dest + olen - 1;
	rom = dest;
#else
        rom = rom_start;
#endif

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
