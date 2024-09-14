/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Provide a simple API around the Wuffs JPEG decoder.
 */

#include <stdint.h>

#include "jpeg.h"

#define WUFFS_CONFIG__AVOID_CPU_ARCH
#define WUFFS_CONFIG__MODULES
#define WUFFS_CONFIG__MODULE__BASE
#define WUFFS_CONFIG__MODULE__JPEG
#define WUFFS_CONFIG__STATIC_FUNCTIONS
#define WUFFS_IMPLEMENTATION
#include "../vendorcode/wuffs/wuffs-v0.4.c"

/* ~16K is big enough to move this off the stack */
static wuffs_jpeg__decoder dec;

const char *jpeg_fetch_size(unsigned char *filedata, size_t filesize, unsigned int *width,
			    unsigned int *height)
{
	if (!width || !height) {
		return "invalid arg";
	}

	wuffs_base__status status = wuffs_jpeg__decoder__initialize(
		&dec, sizeof(dec), WUFFS_VERSION, WUFFS_INITIALIZE__DEFAULT_OPTIONS);
	if (status.repr) {
		return status.repr;
	}

	wuffs_base__image_config imgcfg;
	wuffs_base__io_buffer src = wuffs_base__ptr_u8__reader(filedata, filesize, true);
	status = wuffs_jpeg__decoder__decode_image_config(&dec, &imgcfg, &src);
	if (status.repr) {
		return status.repr;
	}

	*width = wuffs_base__pixel_config__width(&imgcfg.pixcfg);
	*height = wuffs_base__pixel_config__height(&imgcfg.pixcfg);

	return NULL;
}

const char *jpeg_decode(unsigned char *filedata, size_t filesize, unsigned char *pic,
			unsigned int width, unsigned int height, unsigned int bytes_per_line,
			unsigned int depth)
{
	if (!filedata || !pic) {
		return "invalid arg";
	}
	/* Relatively arbitrary limit that shouldn't hurt anybody.
	 * 300M (10k*10k*3bytes/pixel) is already larger than our heap, so
	 * it's on the safe side.
	 * This avoids overflows when width or height are used for
	 * calculations in this function.
	 */
	if ((width > 10000) || (height > 10000)) {
		return "invalid arg";
	}

	uint32_t pixfmt;
	switch (depth) {
	case 16:
		pixfmt = WUFFS_BASE__PIXEL_FORMAT__BGR_565;
		break;
	case 24:
		pixfmt = WUFFS_BASE__PIXEL_FORMAT__BGR;
		break;
	case 32:
		pixfmt = WUFFS_BASE__PIXEL_FORMAT__BGRA_NONPREMUL;
		break;
	default:
		return "invalid arg";
	}

	wuffs_base__status status = wuffs_jpeg__decoder__initialize(
		&dec, sizeof(dec), WUFFS_VERSION, WUFFS_INITIALIZE__DEFAULT_OPTIONS);
	if (status.repr) {
		return status.repr;
	}

	/* Opting in to lower quality means that we can pass an empty slice as the
	 * "work buffer" argument to wuffs_jpeg__decoder__decode_frame below.
	 *
	 * Decoding progressive (not sequential) JPEGs would still require dynamic
	 * memory allocation (and the amount of work buffer required depends on the
	 * image dimensions), but we choose to just reject progressive JPEGs. It is
	 * simpler than sometimes calling malloc (which can fail, especially for
	 * large allocations) and free.
	 *
	 * More commentary about these quirks is at
	 * https://github.com/google/wuffs/blob/beaf45650085a16780b5f708b72daaeb1aa865c8/std/jpeg/decode_quirks.wuffs
	 */
	wuffs_jpeg__decoder__set_quirk(
		&dec, WUFFS_BASE__QUIRK_QUALITY,
		WUFFS_BASE__QUIRK_QUALITY__VALUE__LOWER_QUALITY);
	wuffs_jpeg__decoder__set_quirk(
		&dec, WUFFS_JPEG__QUIRK_REJECT_PROGRESSIVE_JPEGS, 1);

	wuffs_base__image_config imgcfg;
	wuffs_base__io_buffer src = wuffs_base__ptr_u8__reader(filedata, filesize, true);
	status = wuffs_jpeg__decoder__decode_image_config(&dec, &imgcfg, &src);
	if (status.repr) {
		return status.repr;
	}

	wuffs_base__pixel_config pixcfg;
	wuffs_base__pixel_config__set(&pixcfg, pixfmt, 0, width, height);

	wuffs_base__pixel_buffer pixbuf;
	status = wuffs_base__pixel_buffer__set_interleaved(
		&pixbuf, &pixcfg,
		wuffs_base__make_table_u8(pic, width * (depth / 8), height, bytes_per_line),
		wuffs_base__empty_slice_u8());
	if (status.repr) {
		return status.repr;
	}

	status = wuffs_jpeg__decoder__decode_frame(&dec, &pixbuf, &src,
						   WUFFS_BASE__PIXEL_BLEND__SRC,
						   wuffs_base__empty_slice_u8(), NULL);
	return status.repr;
}
