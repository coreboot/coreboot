#include <console/console.h>
#include <stdlib.h>
#include <stddef.h>
#include <stream/read_bytes.h>
#include <string.h>

/* if they set the precompressed rom stream, they better have set a type */
#if CONFIG_PRECOMPRESSED_PAYLOAD && ((!CONFIG_COMPRESSED_PAYLOAD_NRV2B) && (!CONFIG_COMPRESSED_PAYLOAD_LZMA))
#error "You set CONFIG_PRECOMPRESSED_PAYLOAD but need to set CONFIG_COMPRESSED_PAYLOAD_NRV2B or CONFIG_COMPRESSED_PAYLOAD_LZMA"
#endif

/* If they set ANY of these, then we're compressed */
#if ((CONFIG_COMPRESSED_PAYLOAD_NRV2B) || (CONFIG_COMPRESSED_PAYLOAD_LZMA))
#define UNCOMPRESSER 1
extern unsigned char _heap, _eheap;
#endif

#if (CONFIG_COMPRESSED_PAYLOAD_NRV2B) 
#define HAVE_UNCOMPRESSER 1
#include "../lib/nrv2b.c"
#endif

#if (CONFIG_COMPRESSED_PAYLOAD_LZMA)
#ifdef HAVE_UNCOMPRESSER
#error "You're defining more than one compression type, which is not allowed."
#endif
#define HAVE_UNCOMPRESSER 1
#include "../lib/lzma.c"
#endif

#ifndef CONFIG_ROM_PAYLOAD_START
#define CONFIG_ROM_PAYLOAD_START 0xffff0000UL
#endif

/* well, this is a mess, and it will get fixed, but not right away.
 * until we stop using 'ld' for building the rom image, that is.
 * problem is, that on the sc520, ROM_PAYLOAD_START has to be at 0x2000000.
 * but if you set CONFIG_ROM_PAYLOAD_START to that, then ld will try to
 * build a giant image: 0x0-0x2000000, i.e. almost 4 GB.
 * so make this non-static, non-const for now.
 */

/*XXXXXXXXXXXXXX */
unsigned char *rom_start = (unsigned char *)CONFIG_ROM_PAYLOAD_START;
unsigned char *rom_end   = (unsigned char *)(CONFIG_ROM_PAYLOAD_START + PAYLOAD_SIZE - 1);
/*XXXXXXXXXXXXXX */

static const unsigned char *rom;

#ifdef UNCOMPRESSER
unsigned long 
uncompress(uint8_t * start_addr, uint8_t *dest_addr)
{
#if (CONFIG_COMPRESSED_PAYLOAD_NRV2B) 
	unsigned long ilen; // used compressed stream length
	return unrv2b(start_addr, dest_addr, &ilen);
#endif
#if (CONFIG_COMPRESSED_PAYLOAD_LZMA)
	return ulzma(start_addr, dest_addr);
#endif
}
#endif

int stream_init(void)
{
#ifdef UNCOMPRESSER
        unsigned char *dest;
        unsigned long olen;
#endif

	printk_debug("rom_stream: 0x%08lx - 0x%08lx\n",
		(unsigned long)rom_start,
		(unsigned long)rom_end);

#ifdef UNCOMPRESSER

        dest = &_eheap; /* need a good address on RAM */

#if _RAMBASE<0x00100000
	olen = *(unsigned int *)dest;
#if (CONFIG_CONSOLE_VGA==1) || (CONFIG_PCI_ROM_RUN == 1)
	if((dest < (unsigned char *)0xa0000) && ((dest+olen)>(unsigned char *)0xa0000)) {
		dest = (unsigned char *)(CONFIG_LB_MEM_TOPK<<10);
	}
#endif
        if((dest < (unsigned char *) 0xf0000) && ((dest+olen)> (unsigned char *)0xf0000)) { // coreboot tables etc
	  dest = (unsigned char *) (CONFIG_LB_MEM_TOPK<<10);
        }
#endif

	/* ALL of those settings are too smart and also unsafe. Set the dest to 16 MB: 
	 * known to be safe for LB for now, and mostly safe for all elf images we have tried. 
	 * long term, this has got to be fixed. 
	 */
	dest  = (unsigned char *) (16 * 1024 * 1024);
        printk_debug("Uncompressing to RAM %p ", dest);
        olen = uncompress((uint8_t *) rom_start, (uint8_t *)dest );
	printk_debug(" olen = 0x%08lx done.\n", olen);
	if (olen != 0) {
		rom_end = dest + olen - 1;
		rom = dest;
	} else {
		/* Decompression failed, assume payload is uncompressed */
		printk_debug("Decompression failed. Assuming payload is uncompressed...\n");
		rom = rom_start;
	}
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
	if ((rom + bytes - 1) > rom_end) {
		printk_warning("%6d:%s() - overflowed source buffer\n",
			__LINE__, __func__);
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
