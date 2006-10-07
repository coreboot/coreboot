#include <console/console.h>
#include <stdlib.h>
#include <stddef.h>
#include <stream/read_bytes.h>
#include <string.h>

/* if they set the precompressed rom stream, they better have set a type */
#if CONFIG_PRECOMPRESSED_ROM_STREAM && ((!CONFIG_COMPRESSED_ROM_STREAM) && (!CONFIG_COMPRESSED_ROM_STREAM_NRV2B) && (!CONFIG_COMPRESSED_ROM_STREAM_LZMA))
#error "You set CONFIG_PRECOMPRESSED_ROM_STREAM but need to set CONFIG_COMPRESSED_ROM_STREAM (implies NRV2B, deprecated) or CONFIG_COMPRESSED_ROM_STREAM_NRV2B or CONFIG_COMPRESSED_ROM_STREAM_LZMA"
#endif

#if (CONFIG_COMPRESSED_ROM_STREAM) || (CONFIG_COMPRESSED_ROM_STREAM_NRV2B) 
#define HAVE_UNCOMPRESSER 1
#include "../lib/nrv2b.c"
#endif

#if (CONFIG_COMPRESSED_ROM_STREAM_LZMA)
#if HAVE_UNCOMPRESSER
#error "You're defining more than one compression type, which is not allowed (of course)"
#endif
#define HAVE_UNCOMPRESSER 1
#include "../lib/lzma.c"
#endif

#include "../lib/xmodem.c"

/* Blocks of RAM for storing stream data */
static unsigned char *stream_start = (unsigned char *) 0x02000000;
static unsigned char *stream_end;
static unsigned char *temp_start = (unsigned char *) 0x03000000;
static int stream_max_bytes = 0x00800000;

#if HAVE_UNCOMPRESSER
static unsigned long uncompress(uint8_t *src, uint8_t *dest)
{
#if (CONFIG_COMPRESSED_ROM_STREAM) || (CONFIG_COMPRESSED_ROM_STREAM_NRV2B) 
	unsigned long ilen;
	return unrv2b(src, dest, &ilen);
#endif
#if (CONFIG_COMPRESSED_ROM_STREAM_LZMA)
	return ulzma(src, dest);
#endif
}
#endif

int stream_init(void)
{
	int len;

	printk_debug("serial_stream: downloading to 0x%08lx; start XMODEM transfer now!\n", temp_start);
	if ((len = xmodemReceive(temp_start, stream_max_bytes)) <= 0) {
		printk_debug("\nserial_stream: XMODEM transfer failed\n");
		return -1;
	}
	printk_debug("\nserial_stream: XMODEM transfer complete; %d bytes received\n", len);
#if HAVE_UNCOMPRESSER
	printk_debug("serial_stream: uncompressing to 0x%08lx\n", stream_start);
	len = uncompress((uint8_t *) temp_start, (uint8_t *) stream_start);
	printk_debug("serial_stream: %d bytes uncompressed\n", len);
#else
	printk_debug("serial_stream: copying to 0x%08lx\n", stream_start);
	memcpy(stream_start, temp_start, len);
#endif
	stream_end = stream_start + len - 1;
	return 0;
}

void stream_fini(void)
{
}

byte_offset_t stream_skip(byte_offset_t count)
{
	if ((stream_start + count) > stream_end) {
		printk_warning("%6d:%s() - overflowed source buffer\n",
			__LINE__, __FUNCTION__);
		count = 0;
		if (stream_start <= stream_end) {
			count = (stream_end - stream_start) + 1;
		}
	}
	stream_start += count;
	return count;
}

byte_offset_t stream_read(void *vdest, byte_offset_t count)
{
	unsigned char *old_start = stream_start;
	count = stream_skip(count);
	memcpy(vdest, old_start, count);
	return count;
}
