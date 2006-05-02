#include <console/console.h>
#include <stdlib.h>
#include <stddef.h>
#include <stream/read_bytes.h>
#include <string.h>

// include generic nrv2b 
#include "../lib/nrv2b.c"

#ifndef CONFIG_ROM_STREAM_START
#define CONFIG_ROM_STREAM_START 0xffff0000UL
#endif

unsigned char *rom_start = (unsigned char *)CONFIG_ROM_STREAM_START;
unsigned char *rom_end = (unsigned char *)(CONFIG_ROM_STREAM_START + PAYLOAD_SIZE - 1);;

extern unsigned char _heap, _eheap;

static const unsigned char *rom;

int stream_init(void)
{
	unsigned long compressed_rom_start=rom_start; 
	unsigned long compressed_rom_end=rom_end; 
	unsigned int len;
	
	len=*(unsigned int *)compressed_rom_start; // LE only for now

	printk_debug (" compressed rom stream: 0x%08lx - 0x%08lx\n",
				compressed_rom_start, compressed_rom_end);

	rom_start = &_eheap;
	rom_end = rom_start + len; // LE only for now
#if 0
	{
	int i;
	for (i=0; i<512; i++) {
		if( i%16==0) printk_spew("\n%04x :", i);
		printk_spew(" %02x", *(unsigned char *)(compressed_rom_start+i));
	}
	}
#endif
	printk_debug(" rom stream: 0x%08lx - 0x%08lx\n", (unsigned long) 
			rom_start, (unsigned long) rom_end);

	printk_debug("Uncompressing...");
	unrv2b((uint8_t *) compressed_rom_start, (uint8_t *)rom_start);
	printk_debug(" done.\n");
	
	rom = rom_start;

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
		printk_warning(" overflowed source buffer\n");
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
