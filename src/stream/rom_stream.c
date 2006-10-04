#include <console/console.h>
#include <stdlib.h>
#include <stddef.h>
#include <stream/read_bytes.h>
#include <string.h>

/* if they set the precompressed rom stream, they better have set a type */
#if CONFIG_PRECOMPRESSED_ROM_STREAM && ((!CONFIG_COMPRESSED_ROM_STREAM) && (!CONFIG_COMPRESSED_ROM_STREAM_NRV2B) && (!CONFIG_COMPRESSED_ROM_STREAM_LZMA))
#error "You set CONFIG_PRECOMPRESSED_ROM_STREAM but need to set CONFIG_COMPRESSED_ROM_STREAM (implies NRV2B, deprecated) or CONFIG_COMPRESSED_ROM_STREAM_NRV2B or CONFIG_COMPRESSED_ROM_STREAM_LZMA"
#endif

/* If they set ANY of these, then we're compressed */
#if ((CONFIG_COMPRESSED_ROM_STREAM) || (CONFIG_COMPRESSED_ROM_STREAM_NRV2B) || (CONFIG_COMPRESSED_ROM_STREAM_LZMA))
#define UNCOMPRESSER 1
extern unsigned char _heap, _eheap;
#endif

#if (CONFIG_COMPRESSED_ROM_STREAM) || (CONFIG_COMPRESSED_ROM_STREAM_NRV2B) 
#define HAVE_UNCOMPRESSER 1
// include generic nrv2b
#include "../lib/nrv2b.c"
#endif

#if (CONFIG_COMPRESSED_ROM_STREAM_LZMA)
#if HAVE_UNCOMPRESSER
#error "You're defining more than one compression type, which is not allowed (of course)"
#endif
#define HAVE_UNCOMPRESSER 1
// include generic nrv2b
#include "../lib/lzma.c"
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

static const unsigned char *rom;

#if UNCOMPRESSER
unsigned long 
uncompress(uint8_t * rom_start, uint8_t *dest )
{
#if (CONFIG_COMPRESSED_ROM_STREAM) || (CONFIG_COMPRESSED_ROM_STREAM_NRV2B) 
	unsigned long ilen; // used compressed stream length
	return unrv2b(rom_start, dest, &ilen);
#endif
#if (CONFIG_COMPRESSED_ROM_STREAM_LZMA)
	return ulzma(rom_start, dest);
#endif
}
#endif
int stream_init(void)
{
#if (UNCOMPRESSER)
        unsigned char *dest;
        unsigned long olen;
#endif

	printk_debug("rom_stream: 0x%08lx - 0x%08lx\n",
		(unsigned long)rom_start,
		(unsigned long)rom_end);

#if (UNCOMPRESSER) 

        dest = &_eheap; /* need a good address on RAM */

#if _RAMBASE<0x00100000
	olen = *(unsigned int *)dest;
#if (CONFIG_CONSOLE_VGA==1) || (CONFIG_PCI_ROM_RUN == 1)
	if((dest < 0xa0000) && ((dest+olen)>0xa0000)) {
		dest = (CONFIG_LB_MEM_TOPK<<10);
	}
#endif
        if((dest < (unsigned char *) 0xf0000) && ((dest+olen)> (unsigned char *)0xf0000)) { //linuxbios tables etc
	  dest = (unsigned char *) (CONFIG_LB_MEM_TOPK<<10);
        }
#endif

	/* ALL of those settings are too smart and also unsafe. Set the dest to 16 MB: 
	 * known to be safe for LB for now, and mostly safe for all elf images we have tried. 
	 * long term, this has got to be fixed. 
	 */
	dest  = (unsigned char *) (16 * 1024 * 1024);
        printk_debug("Uncompressing to RAM 0x%08lx ", dest);
        olen = uncompress((uint8_t *) rom_start, (uint8_t *)dest );
	printk_debug(" olen = 0x%08lx done.\n", olen);
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
