/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <boot/coreboot_tables.h>
#include <bootsplash.h>
#include <console/console.h>
#include <fsp/api.h>
#include <fsp/fsp_gop_blt.h>
#include <stdlib.h>

static bool is_bmp_image_valid(efi_bmp_image_header *header)
{
	if (header == NULL)
		return false;

	/* Check if the BMP Header Signature is valid */
	if (header->CharB != 'B' || header->CharM != 'M')
		return false;

	/* Check if the BMP Image Header Length is valid */
	if (!header->PixelHeight || !header->PixelWidth)
		return false;

	if (header->Size < header->ImageOffset)
		return false;

	if (header->ImageOffset < sizeof(efi_bmp_image_header))
		return false;

	return true;
}

static bool is_bmp_image_compressed(efi_bmp_image_header *header)
{
	if (header == NULL)
		return false;

	if (header->CompressionType != 0)
		return true;

	return false;
}

static bool is_bitmap_format_supported(efi_bmp_image_header *header)
{
	if (header == NULL)
		return false;

	/*
	 * Check BITMAP format is supported
	 * BMP_IMAGE_HEADER = BITMAP_FILE_HEADER + BITMAP_INFO_HEADER
	 */
	if (header->HeaderSize != sizeof(efi_bmp_image_header) -
			 OFFSET_OF(efi_bmp_image_header, HeaderSize))
		return false;

	return true;
}

static bool do_bmp_image_authentication(efi_bmp_image_header *header)
{
	if (header == NULL)
		return false;

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

static uint32_t calculate_blt_buffer_size(efi_bmp_image_header *header)
{
	uint32_t blt_buffer_size;

	if (header == NULL)
		return 0;

	/* Calculate the size required for BLT buffer */
	blt_buffer_size = header->PixelWidth * header->PixelHeight *
			 sizeof(efi_graphics_output_blt_pixel);
	if (!blt_buffer_size)
		return 0;

	return blt_buffer_size;
}

static int get_color_map_num(efi_bmp_image_header *header)
{
	int col_map_number;

	if (header == NULL)
		return -1;

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
		 * For other bit depths (e.g., 24-bit and 32-bit) that doesn't have
		 * a standard palette, col_map_number remains 0.
		 */
		col_map_number = 0;
		break;
	}

	/*
	 * At times BMP file may have padding data between its header section and the
	 * data section.
	 */
	if (header->ImageOffset - sizeof(efi_bmp_image_header) <
			 sizeof(efi_bmp_color_map) * col_map_number)
		return -1;

	return col_map_number;
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
static bool calculate_adj_height_width(const efi_bmp_image_header *header, size_t *adjusted_x,
	 size_t *adjusted_y, enum lb_fb_orientation orientation, int blt_width, int blt_height)
{
	if (!header || !adjusted_x || !adjusted_y)
		return false;

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

/*
 * Visual Representation of gop_width and calculate linear offset into the GOP buffer:
 *
 * GOP Display Buffer:
 *
 *      +------------------------------------------------+
 *      | 0                                              |
 *      | |--> gop_width pixels in this row              |
 *      +------------------------------------------------+
 *      | gop_width                                      |
 *      |                                                |
 *      | gop_width                                      |
 *      | ...                                            |
 *      +------------------------------------------------+
 *      | gop_width                                      |
 *      +------------------------------------------------+
 *
 * gop_width: Represents the width of a row in the GOP buffer.
 * - It is determined by the `orientation` of the image.
 * - For `LEFT_UP` and `RIGHT_UP`, the GOP width is the BMP image's `PixelHeight`.
 * - For `BOTTOM_UP` and `NORMAL`, the GOP width is the BMP image's `PixelWidth`.
 *
 * gop_x, gop_y: Coordinates within the GOP buffer where the blit will start.
 * - These are calculated by `calculate_adj_height_width` based on the `orientation`,
 * `blt_width`, and `blt_height`.
 *
 * Pointer to the calculated pixel in the GOP blit buffer, or NULL on error.
 * - Calculate `gop_blt_offset`as `gop_y * gop_width + gop_x` to determine the offset
 *   to access the correct pixel within `gop_blt_buffer`.
 * - `gop_y * gop_width` calculates the offset of the row.
 * - `gop_x` calculates the offset within that row.
 */
static efi_graphics_output_blt_pixel *get_gop_blt_pixel(
	efi_graphics_output_blt_pixel *gop_blt_buffer, const efi_bmp_image_header *header,
	 int blt_width, int blt_height, enum lb_fb_orientation orientation)
{
	size_t gop_x;
	size_t gop_y;
	size_t gop_width;

	if (!header || !gop_blt_buffer)
		return NULL;

	switch (orientation) {
	case LB_FB_ORIENTATION_LEFT_UP:
	case LB_FB_ORIENTATION_RIGHT_UP:
		gop_width = header->PixelHeight;
		break;

	case LB_FB_ORIENTATION_BOTTOM_UP:
	case LB_FB_ORIENTATION_NORMAL:
	default:
		gop_width = header->PixelWidth;
		break;
	}

	if (!calculate_adj_height_width(header, &gop_x, &gop_y, orientation,
			 blt_width, blt_height))
		return NULL;

	/*
	 * Calculated pixel in the Graphics Output Protocol (GOP) blit buffer.
	 * This offset represents the starting position where the blitted region will be placed
	 * in the framebuffer.
	 */
	return &gop_blt_buffer[gop_y * gop_width + gop_x];
}

/*
 * Fill BMP image into BLT buffer format with optional orientation
 *
 * +------------------------------------------------------------------+
 * |  BMP Image (header: efi_bmp_image_header)                        |
 * |  (PixelWidth, PixelHeight)                                       |
 * +------------------------------------------------------------------+
 *        |
 *        | (blt_width, blt_height) : Region to blit
 *        V
 * +------------------------------------------------------------------+
 * |  calculate_adj_height_width(header, adjusted_x, adjusted_y,      |
 * |                               orientation, blt_width, blt_height)|
 * +------------------------------------------------------------------+
 *        |
 *        | Calculates adjusted coordinates based on orientation:
 *        |   - flipped_x = header->PixelWidth - blt_width - 1
 *        |   - flipped_y = header->PixelHeight - blt_height - 1
 *        |   - Depending on 'orientation', adjusted_x and adjusted_y are set.
 *        V
 * +-------------------------------------------------------------------+
 * |  get_gop_blt_pixel(gop_blt_buffer, header, blt_width, blt_height, |
 * |                                                  orientation)     |
 * +-------------------------------------------------------------------+
 *        |
 *        | 1. Determines gop_width based on orientation:
 *        |    - LEFT_UP/RIGHT_UP: gop_width = header->PixelHeight,
 *        |    - BOTTOM_UP/NORMAL: gop_width = header->PixelWidth,
 *        | 2. Calls calculate_adj_height_width to get gop_x and gop_y.
 *        | 3. GOP BLT offset: calculates linear offset into the GOP buffer
 *        |    - gop_blt_offset = gop_y * gop_width + gop_x
 *        | 4. GOP BLT offset is used to access the correct regions in:
 *        V
 * +------------------------------------------------------------------+
 * |  GOP Blit Buffer (Framebuffer)                                   |
 * +------------------------------------------------------------------+
 */
static void *fill_blt_buffer(efi_bmp_image_header *header,
	uintptr_t logo_ptr, size_t blt_buffer_size, enum lb_fb_orientation orientation)
{
	efi_graphics_output_blt_pixel *gop_blt_buffer;
	efi_graphics_output_blt_pixel *gop_blt_ptr;
	efi_graphics_output_blt_pixel *gop_blt;
	uint8_t *bmp_image;
	uint8_t *bmp_image_header;
	efi_bmp_color_map *bmp_color_map;
	size_t image_index;

	if (header == NULL)
		return NULL;

	gop_blt_ptr = malloc(sizeof(blt_buffer_size));
	if (!gop_blt_ptr)
		die("%s: out of memory. Consider increasing the `CONFIG_HEAP_SIZE`\n",
			 __func__);

	bmp_image = ((uint8_t *)logo_ptr) + header->ImageOffset;
	bmp_image_header = bmp_image;
	gop_blt_buffer = gop_blt_ptr;
	bmp_color_map = (efi_bmp_color_map *)(logo_ptr + sizeof(efi_bmp_image_header));

	for (size_t height = 0; height < header->PixelHeight; height++) {
		for (size_t width = 0; width < header->PixelWidth; width++, bmp_image++) {
			size_t index = 0;

			gop_blt = get_gop_blt_pixel(gop_blt_buffer, header, width, height,
							 orientation);
			if (!gop_blt) {
				free(gop_blt_ptr);
				return NULL;
			}

			switch (header->BitPerPixel) {
			/* Translate 1-bit (2 colors) BMP to 24-bit color */
			case 1:
				for (index = 0; index < 8 && width < header->PixelWidth; index++) {
					uint8_t bit = ((*bmp_image) >> (7 - index)) & 0x1;
					gop_blt->Red = bmp_color_map[bit].Red;
					gop_blt->Green = bmp_color_map[bit].Green;
					gop_blt->Blue = bmp_color_map[bit].Blue;
					width++;
					gop_blt = get_gop_blt_pixel(gop_blt_buffer, header, width, height,
								 orientation);
					if (!gop_blt) {
						free(gop_blt_ptr);
						return NULL;
					}
				}
				width--;
				break;

			/* Translate 4-bit (16 colors) BMP Palette to 24-bit color */
			case 4:
				index = (*bmp_image) >> 4;
				gop_blt->Red = bmp_color_map[index].Red;
				gop_blt->Green = bmp_color_map[index].Green;
				gop_blt->Blue = bmp_color_map[index].Blue;
				if (width < (header->PixelWidth - 1)) {
					width++;
					gop_blt = get_gop_blt_pixel(gop_blt_buffer, header, width, height,
								 orientation);
					if (!gop_blt) {
						free(gop_blt_ptr);
						return NULL;
					}
					index = (*bmp_image) & 0x0f;
					gop_blt->Red = bmp_color_map[index].Red;
					gop_blt->Green = bmp_color_map[index].Green;
					gop_blt->Blue = bmp_color_map[index].Blue;
				}
				break;

			/* Translate 8-bit (256 colors) BMP Palette to 24-bit color */
			case 8:
				gop_blt->Red = bmp_color_map[*bmp_image].Red;
				gop_blt->Green = bmp_color_map[*bmp_image].Green;
				gop_blt->Blue = bmp_color_map[*bmp_image].Blue;
				break;

			/* For 24-bit BMP */
			case 24:
				gop_blt->Blue = *bmp_image++;
				gop_blt->Green = *bmp_image++;
				gop_blt->Red = *bmp_image;
				break;

			/* Convert 32 bit to 24bit bmp - just ignore the final byte of each pixel */
			case 32:
				gop_blt->Blue = *bmp_image++;
				gop_blt->Green = *bmp_image++;
				gop_blt->Red = *bmp_image++;
				break;

			/* Other bit format of BMP is not supported. */
			default:
				free(gop_blt_ptr);
				gop_blt_ptr = NULL;

				printk(BIOS_ERR, "%s, BMP Bit format not supported. 0x%X\n", __func__,
					 header->BitPerPixel);
				return NULL;
			}
		}
		image_index = (uintptr_t)bmp_image - (uintptr_t)bmp_image_header;
		/* Each row in BMP Image should be 4-byte align */
		if ((image_index % 4) != 0)
			bmp_image = bmp_image + (4 - (image_index % 4));
	}

	return gop_blt_ptr;
}

/* Convert a *.BMP graphics image to a GOP blt buffer */
void fsp_convert_bmp_to_gop_blt(efi_uintn_t *logo, uint32_t *logo_size,
	efi_uintn_t *blt_ptr, efi_uintn_t *blt_size, uint32_t *pixel_height, uint32_t *pixel_width,
	enum lb_fb_orientation orientation)
{
	uintptr_t logo_ptr;
	size_t logo_ptr_size, blt_buffer_size;
	efi_bmp_image_header *bmp_header;

	if (!logo || !logo_size || !blt_ptr || !blt_size || !pixel_height || !pixel_width)
		return;

	logo_ptr = (uintptr_t)bmp_load_logo(&logo_ptr_size);

	if (!logo_ptr || logo_ptr_size < sizeof(efi_bmp_image_header)) {
		printk(BIOS_ERR, "%s: BMP Image size is too small.\n", __func__);
		return;
	}

	bmp_header = (efi_bmp_image_header *)logo_ptr;
	if (!do_bmp_image_authentication(bmp_header) || (bmp_header->Size != logo_ptr_size))
		return;

	blt_buffer_size = calculate_blt_buffer_size(bmp_header);
	if (!blt_buffer_size)
		return;

	if (get_color_map_num(bmp_header) < 0)
		return;

	bool is_standard_orientation = (orientation == LB_FB_ORIENTATION_NORMAL ||
			orientation == LB_FB_ORIENTATION_BOTTOM_UP);
	*logo = logo_ptr;
	*logo_size = logo_ptr_size;
	*blt_size = blt_buffer_size;
	*pixel_height = is_standard_orientation ? bmp_header->PixelHeight : bmp_header->PixelWidth;
	*pixel_width = is_standard_orientation ? bmp_header->PixelWidth : bmp_header->PixelHeight;
	*blt_ptr = (uintptr_t)fill_blt_buffer(bmp_header, logo_ptr, blt_buffer_size, orientation);
}
