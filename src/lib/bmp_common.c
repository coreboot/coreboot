/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <commonlib/helpers.h>
#include <console/console.h>
#include <limits.h>

#include "render_bmp.h"

static bool is_bmp_image_valid(const struct bmp_image_header *header)
{
	/* Check if the BMP Header Signature is valid */
	if (header->CharB != 'B' || header->CharM != 'M')
		return false;

	/* Check if the BMP Image Header Length is valid */
	if (!header->PixelHeight || !header->PixelWidth)
		return false;

	if (header->Size < header->ImageOffset)
		return false;

	if (header->ImageOffset < sizeof(struct bmp_image_header))
		return false;

	return true;
}

static bool is_bmp_image_compressed(const struct bmp_image_header *header)
{
	return header->CompressionType != 0;
}

static bool is_bitmap_format_supported(const struct bmp_image_header *header)
{
	/*
	 * Check BITMAP format is supported
	 * BMP_IMAGE_HEADER = BITMAP_FILE_HEADER + BITMAP_INFO_HEADER
	 */
	if (header->HeaderSize != sizeof(struct bmp_image_header) -
			 OFFSET_OF(struct bmp_image_header, HeaderSize))
		return false;

	return true;
}

bool bmp_do_image_authentication(const struct bmp_image_header *header)
{
	if (!is_bmp_image_valid(header)) {
		printk(BIOS_ERR, "%s: BMP Image Header is invalid.\n", __func__);
		return false;
	}

	/*
	 * BMP image compression is unsupported by FSP implementation,
	 * hence, exit if the BMP image is compressed.
	 */
	if (is_bmp_image_compressed(header)) {
		printk(BIOS_ERR, "%s: BMP Image Compression is unsupported.\n", __func__);
		return false;
	}

	if (!is_bitmap_format_supported(header)) {
		printk(BIOS_ERR, "%s: BmpHeader Header Size (0x%x) is not as expected.\n",
		       __func__, header->HeaderSize);
		return false;
	}

	return true;
}

size_t bmp_row_stride(const struct bmp_image_header *header)
{
	/* BMP row stride: 4-byte aligned (DWORD aligned); use 64-bit math for 32-bit romstage */
	const uint64_t width_in_bits = (uint64_t)header->PixelWidth * header->BitPerPixel;
	const uint64_t stride = DIV_ROUND_UP(width_in_bits, 32ULL) * 4;

	if (stride > SIZE_MAX)
		return 0;

	return (size_t)stride;
}

/* Reconcile pixel-data length implied by Width/Height/bpp with the actual file size. */
bool bmp_pixel_data_fits(const struct bmp_image_header *header, size_t logo_size)
{
	if (header->ImageOffset < sizeof(struct bmp_image_header) ||
	    header->ImageOffset > logo_size)
		return false;

	const size_t bytes_per_row = bmp_row_stride(header);
	if (!bytes_per_row)
		return false;

	const size_t available = (logo_size - header->ImageOffset) / bytes_per_row;

	/* Prevent multiplication overflow: required <= available */
	return (size_t)header->PixelHeight <= available;
}

int bmp_get_color_map_num(const struct bmp_image_header *header)
{
	int col_map_number;

	switch (header->BitPerPixel) {
	case 1:
		col_map_number = 2;
		break;
	case 4:
		col_map_number = 16;
		break;
	case 8:
		col_map_number = 256;
		break;
	default:
		/*
		 * For other bit depths (e.g., 24-bit and 32-bit) that don't have
		 * a standard palette, col_map_number remains 0.
		 */
		col_map_number = 0;
		break;
	}

	/*
	 * At times BMP file may have padding data between its header section and the
	 * data section.
	 */
	if (header->ImageOffset < sizeof(struct bmp_image_header) ||
	    header->ImageOffset - sizeof(struct bmp_image_header) <
			sizeof(struct bmp_color_map) * (size_t)col_map_number)
		return -1;

	return col_map_number;
}

void bmp_get_oriented_dimensions(const struct bmp_image_header *header,
				 enum lb_fb_orientation orientation,
				 uint32_t *width, uint32_t *height)
{
	bool is_standard = (orientation == LB_FB_ORIENTATION_NORMAL ||
			    orientation == LB_FB_ORIENTATION_BOTTOM_UP);

	*width = is_standard ? header->PixelWidth : header->PixelHeight;
	*height = is_standard ? header->PixelHeight : header->PixelWidth;
}

/*
 * Visual Representation of the Flipping:
 *
 * Original BMP Image:
 *
 *      0 -----------------------------------------PixelWidth-1
 *      |                                          |
 *      |                                          |
 *      |                                          |
 *      PixelHeight-1------------------------------|
 *
 * Blitting Region:
 *
 *      (x, y) ----------------------------------- (x + blt_width, y)
 *      |                                          |
 *      |                                          |
 *      (x, y + blt_height) -------------------- --(x + blt_width, y + blt_height)
 *
 * Flipped Coordinates:
 *
 * flipped_x:  Represents the horizontal coordinate from the `right` edge of the BMP,
 *             accounting for the blit width.
 *
 * flipped_y:  Represents the vertical coordinate from the `bottom` edge of the BMP,
 *             accounting for the blit height.
 *
 * Example (Orientation: LB_FB_ORIENTATION_BOTTOM_UP):
 *
 * If blt_width is small, flipped_x will be near PixelWidth.
 * If blt_height is small, flipped_y will be near PixelHeight.
 *
 * The blit then starts at (flipped_x, blt_height), effectively flipping the
 * horizontal position and using the original blit height.
 *
 * This allows for blitting from the bottom-left corner of the display panel.
 */
bool bmp_calculate_adj_height_width(const struct bmp_image_header *header,
				    size_t *adjusted_x, size_t *adjusted_y,
				    enum lb_fb_orientation orientation,
				    size_t blt_width, size_t blt_height)
{
	size_t flipped_x = header->PixelWidth - blt_width - 1;
	size_t flipped_y = header->PixelHeight - blt_height - 1;

	switch (orientation) {
	case LB_FB_ORIENTATION_LEFT_UP:
		*adjusted_x = flipped_y;
		*adjusted_y = flipped_x;
		break;

	case LB_FB_ORIENTATION_BOTTOM_UP:
		*adjusted_x = flipped_x;
		*adjusted_y = blt_height;
		break;

	case LB_FB_ORIENTATION_RIGHT_UP:
		*adjusted_x = blt_height;
		*adjusted_y = blt_width;
		break;

	case LB_FB_ORIENTATION_NORMAL:
	default:
		*adjusted_x = blt_width;
		*adjusted_y = flipped_y;
		break;
	}

	return true;
}
