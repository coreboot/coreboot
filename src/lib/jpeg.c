/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Provide a simple API around the Wuffs JPEG decoder
 * Uses the heap (and lots of it) for the image-size specific
 * work buffer, so ramstage-only.
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

int jpeg_fetch_size(unsigned char *filedata, size_t filesize, unsigned int *width,
		    unsigned int *height)
{
	if (!width || !height) {
		return JPEG_DECODE_FAILED;
	}

	wuffs_base__status status = wuffs_jpeg__decoder__initialize(
		&dec, sizeof(dec), WUFFS_VERSION, WUFFS_INITIALIZE__DEFAULT_OPTIONS);
	if (status.repr) {
		return JPEG_DECODE_FAILED;
	}

	wuffs_base__image_config imgcfg;
	wuffs_base__io_buffer src = wuffs_base__ptr_u8__reader(filedata, filesize, true);
	status = wuffs_jpeg__decoder__decode_image_config(&dec, &imgcfg, &src);
	if (status.repr) {
		return JPEG_DECODE_FAILED;
	}

	*width = wuffs_base__pixel_config__width(&imgcfg.pixcfg);
	*height = wuffs_base__pixel_config__height(&imgcfg.pixcfg);

	return 0;
}

int jpeg_decode(unsigned char *filedata, size_t filesize, unsigned char *pic,
		unsigned int width, unsigned int height, unsigned int bytes_per_line,
		unsigned int depth)
{
	if (!filedata || !pic) {
		return JPEG_DECODE_FAILED;
	}
	/* Relatively arbitrary limit that shouldn't hurt anybody.
	 * 300M (10k*10k*3bytes/pixel) is already larger than our heap, so
	 * it's on the safe side.
	 * This avoids overflows when width or height are used for
	 * calculations in this function.
	 */
	if ((width > 10000) || (height > 10000)) {
		return JPEG_DECODE_FAILED;
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
		return JPEG_DECODE_FAILED;
	}

	wuffs_base__status status = wuffs_jpeg__decoder__initialize(
		&dec, sizeof(dec), WUFFS_VERSION, WUFFS_INITIALIZE__DEFAULT_OPTIONS);
	if (status.repr) {
		return JPEG_DECODE_FAILED;
	}

	wuffs_base__image_config imgcfg;
	wuffs_base__io_buffer src = wuffs_base__ptr_u8__reader(filedata, filesize, true);
	status = wuffs_jpeg__decoder__decode_image_config(&dec, &imgcfg, &src);
	if (status.repr) {
		return JPEG_DECODE_FAILED;
	}

	wuffs_base__pixel_config pixcfg;
	wuffs_base__pixel_config__set(&pixcfg, pixfmt, 0, width, height);

	wuffs_base__pixel_buffer pixbuf;
	status = wuffs_base__pixel_buffer__set_interleaved(
		&pixbuf, &pixcfg,
		wuffs_base__make_table_u8(pic, width * (depth / 8), height, bytes_per_line),
		wuffs_base__empty_slice_u8());
	if (status.repr) {
		return JPEG_DECODE_FAILED;
	}

	uint64_t workbuf_len_min_incl = wuffs_jpeg__decoder__workbuf_len(&dec).min_incl;
	uint8_t *workbuf_array = malloc(workbuf_len_min_incl);
	if ((workbuf_array == NULL) && workbuf_len_min_incl) {
		return JPEG_DECODE_FAILED;
	}

	wuffs_base__slice_u8 workbuf =
		wuffs_base__make_slice_u8(workbuf_array, workbuf_len_min_incl);
	status = wuffs_jpeg__decoder__decode_frame(&dec, &pixbuf, &src,
						   WUFFS_BASE__PIXEL_BLEND__SRC, workbuf, NULL);

	free(workbuf_array);

	if (status.repr) {
		return JPEG_DECODE_FAILED;
	}

	return 0;
}
