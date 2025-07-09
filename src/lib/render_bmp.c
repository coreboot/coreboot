/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <boot/coreboot_tables.h>
#include <bootmode.h>
#include <bootsplash.h>
#include <bootstate.h>
#include <console/console.h>
#include <stdlib.h>
#include <string.h>

#include "render_bmp.h"

static bool is_bmp_image_valid(struct bmp_image_header *header)
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

static bool is_bmp_image_compressed(struct bmp_image_header *header)
{
	return header->CompressionType != 0;
}

static bool is_bitmap_format_supported(struct bmp_image_header *header)
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

static bool do_bmp_image_authentication(struct bmp_image_header *header)
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

static uint32_t calculate_blt_buffer_size(struct bmp_image_header *header)
{
	uint32_t blt_buffer_size;

	/* Calculate the size required for BLT buffer */
	blt_buffer_size = header->PixelWidth * header->PixelHeight *
			 sizeof(struct blt_pixel);
	if (!blt_buffer_size)
		return 0;

	return blt_buffer_size;
}

static int get_color_map_num(struct bmp_image_header *header)
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
	if (header->ImageOffset - sizeof(struct bmp_image_header) <
			 sizeof(struct bmp_color_map) * col_map_number)
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
static bool calculate_adj_height_width(const struct bmp_image_header *header, size_t *adjusted_x,
	 size_t *adjusted_y, enum lb_fb_orientation orientation, int blt_width, int blt_height)
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
static struct blt_pixel *get_gop_blt_pixel(
	struct blt_pixel *gop_blt_buffer, const struct bmp_image_header *header,
	 int blt_width, int blt_height, enum lb_fb_orientation orientation)
{
	size_t gop_x;
	size_t gop_y;
	size_t gop_width;

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
 * |  BMP Image (header: bmp_image_header)                        |
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
static void *fill_blt_buffer(struct bmp_image_header *header,
	uintptr_t logo, size_t blt_buffer_size, enum lb_fb_orientation orientation)
{
	struct blt_pixel *gop_blt_buffer;
	struct blt_pixel *gop_blt_ptr;
	struct blt_pixel *gop_blt;
	uint8_t *bmp_image;
	uint8_t *bmp_header;
	struct bmp_color_map *color_map;
	size_t image_index;

	gop_blt_ptr = malloc(sizeof(blt_buffer_size));
	if (!gop_blt_ptr)
		die("%s: out of memory. Consider increasing the `CONFIG_HEAP_SIZE`\n",
			 __func__);

	bmp_image = ((uint8_t *)logo) + header->ImageOffset;
	bmp_header = bmp_image;
	gop_blt_buffer = gop_blt_ptr;
	color_map = (struct bmp_color_map *)(logo + sizeof(struct bmp_image_header));

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
					gop_blt->Red = color_map[bit].Red;
					gop_blt->Green = color_map[bit].Green;
					gop_blt->Blue = color_map[bit].Blue;
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
				gop_blt->Red = color_map[index].Red;
				gop_blt->Green = color_map[index].Green;
				gop_blt->Blue = color_map[index].Blue;
				if (width < (header->PixelWidth - 1)) {
					width++;
					gop_blt = get_gop_blt_pixel(gop_blt_buffer, header, width, height,
								 orientation);
					if (!gop_blt) {
						free(gop_blt_ptr);
						return NULL;
					}
					index = (*bmp_image) & 0x0f;
					gop_blt->Red = color_map[index].Red;
					gop_blt->Green = color_map[index].Green;
					gop_blt->Blue = color_map[index].Blue;
				}
				break;

			/* Translate 8-bit (256 colors) BMP Palette to 24-bit color */
			case 8:
				gop_blt->Red = color_map[*bmp_image].Red;
				gop_blt->Green = color_map[*bmp_image].Green;
				gop_blt->Blue = color_map[*bmp_image].Blue;
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
		image_index = (uintptr_t)bmp_image - (uintptr_t)bmp_header;
		/* Each row in BMP Image should be 4-byte align */
		if ((image_index % 4) != 0)
			bmp_image = bmp_image + (4 - (image_index % 4));
	}

	return gop_blt_ptr;
}

/* Helper function to perform the common BMP to GOP BLT conversion logic */
static bool convert_bmp_to_gop_blt_common(uintptr_t logo, size_t logo_size,
	uintptr_t *blt, size_t *blt_size, uint32_t *pixel_height,
	uint32_t *pixel_width, enum lb_fb_orientation orientation)
{
	size_t blt_buffer_size;
	struct bmp_image_header *bmp_header;

	bmp_header = (struct bmp_image_header *)logo;

	/* Authenticate BMP header and validate size against provided logo_size */
	if (!do_bmp_image_authentication(bmp_header) || (bmp_header->Size != logo_size))
		return false;

	blt_buffer_size = calculate_blt_buffer_size(bmp_header);
	if (!blt_buffer_size)
		return false;

	if (get_color_map_num(bmp_header) < 0)
		return false;

	bool is_standard_orientation = (orientation == LB_FB_ORIENTATION_NORMAL ||
					orientation == LB_FB_ORIENTATION_BOTTOM_UP);

	*blt_size = blt_buffer_size;
	*pixel_height = is_standard_orientation ? bmp_header->PixelHeight : bmp_header->PixelWidth;
	*pixel_width = is_standard_orientation ? bmp_header->PixelWidth : bmp_header->PixelHeight;
	*blt = (uintptr_t)fill_blt_buffer(bmp_header, logo, blt_buffer_size, orientation);

	return true;
}

/* Convert a *.BMP graphics image to a blt buffer */
void load_and_convert_bmp_to_blt(uintptr_t *logo, size_t *logo_size,
	uintptr_t *blt, size_t *blt_size, uint32_t *pixel_height, uint32_t *pixel_width,
	enum lb_fb_orientation orientation)
{
	uintptr_t bmp;
	size_t bmp_size;

	if (!logo || !logo_size || !blt || !blt_size || !pixel_height || !pixel_width)
		return;

	bmp = (uintptr_t)bmp_load_logo(&bmp_size);

	if (!bmp || bmp_size < sizeof(struct bmp_image_header))
		return;

	*logo = bmp;
	*logo_size = bmp_size;

	convert_bmp_to_gop_blt_common(*logo, *logo_size, blt, blt_size,
				      pixel_height, pixel_width, orientation);
}

/* Convert a *.BMP graphics image (as per input `logo`) to a GOP blt buffer */
void convert_bmp_to_blt(uintptr_t logo, size_t logo_size,
	uintptr_t *blt, size_t *blt_size, uint32_t *pixel_height, uint32_t *pixel_width,
	enum lb_fb_orientation orientation)
{
	if (!blt || !blt_size || !pixel_height || !pixel_width)
		return;

	if (!logo || logo_size < sizeof(struct bmp_image_header))
		return;

	convert_bmp_to_gop_blt_common(logo, logo_size, blt, blt_size,
				      pixel_height, pixel_width, orientation);
}

/*
 * Calculates the destination coordinates for the logo based on both horizontal and
 * vertical alignment settings.
 *
 * horizontal_resolution: The horizontal resolution of the display panel.
 * vertical_resolution: The vertical resolution of the display panel.
 * logo_width: The width of the logo bitmap.
 * logo_height: The height of the logo bitmap.
 * halignment: The horizontal alignment setting. Use FW_SPLASH_HALIGNMENT_NONE
 *             if only vertical alignment is specified.
 * valignment: The vertical alignment setting. Use FW_SPLASH_VALIGNMENT_NONE
 *             if only horizontal alignment is specified.
 *
 * Returning `struct logo_coordinates` that contains the calculated x and y coordinates
 * for rendering the logo.
 */
static struct logo_coordinates calculate_logo_coordinates(
	uint32_t horizontal_resolution, uint32_t vertical_resolution,
	uint32_t logo_width, uint32_t logo_height,
	enum fw_splash_horizontal_alignment halignment,
	enum fw_splash_vertical_alignment valignment)
{
	struct logo_coordinates coords;

	/* Calculate X coordinate */
	switch (halignment) {
	case FW_SPLASH_HALIGNMENT_LEFT:
		coords.x = 0;
		break;
	case FW_SPLASH_HALIGNMENT_RIGHT:
		coords.x = horizontal_resolution - logo_width;
		break;
	default: /* FW_SPLASH_HALIGNMENT_CENTER (default) */
		coords.x = (horizontal_resolution - logo_width) / 2;
		break;
	}

	/* Calculate Y coordinate */
	switch (valignment) {
	case FW_SPLASH_VALIGNMENT_MIDDLE:
		coords.y = vertical_resolution / 2;
		break;
	case FW_SPLASH_VALIGNMENT_TOP:
		coords.y = 0;
		break;
	case FW_SPLASH_VALIGNMENT_BOTTOM:
		coords.y = vertical_resolution - logo_height;
		break;
	default: /* FW_SPLASH_VALIGNMENT_CENTER (default) */
		coords.y = (vertical_resolution - logo_height) / 2;
		break;
	}

	return coords;
}

/*
 * Copies the logo to the framebuffer.
 *
 * framebuffer_base: The base address of the framebuffer.
 * bytes_per_scanline: The number of bytes per scanline in the framebuffer.
 * logo_buffer: The address of the logo data in BLT format.
 * logo_width: The width of the logo bitmap.
 * logo_height: The height of the logo bitmap.
 * dest_x: The destination x-coordinate in the framebuffer for rendering the logo.
 * dest_y: The destination y-coordinate in the framebuffer for rendering the logo.
 */
static void copy_logo_to_framebuffer(
	uintptr_t framebuffer_base, uint32_t bytes_per_scanline,
	uintptr_t logo_buffer, uint32_t logo_width, uint32_t logo_height,
	uint32_t dest_x, uint32_t dest_y)
{
	size_t pixel_size = sizeof(struct blt_pixel);
	size_t bytes_per_logo_line = logo_width * pixel_size;
	uint8_t *framebuffer_offset = (uint8_t *)framebuffer_base + dest_y * bytes_per_scanline
					 + dest_x * pixel_size;
	uint8_t *dst_row_address = framebuffer_offset;
	uint8_t *src_row_address = (uint8_t *)logo_buffer;
	for (uint32_t i = 0; i < logo_height; i++) {
		memcpy(dst_row_address, src_row_address, bytes_per_logo_line);
		dst_row_address += bytes_per_scanline;
		src_row_address += bytes_per_logo_line;
	}
}

/*
 * Adjust logo layout based on the panel orientation.
 *
 * logo_type: Logo type.
 * config: Logo configuration information.
 * logo_halignment: Resultant horizontal alignment setting.
 * logo_valignment: Resultant vertical alignment setting.
 * logo_bottom_margin: Resultant bottom margin.
 */
static void get_logo_layout(
	enum bootsplash_type logo_type,
	struct logo_config *config,
	enum fw_splash_horizontal_alignment *logo_halignment,
	enum fw_splash_vertical_alignment *logo_valignment,
	uint8_t *logo_bottom_margin
)
{
	if (!config || !logo_halignment || !logo_valignment || !logo_bottom_margin)
		return;

	if (logo_type == BOOTSPLASH_LOW_BATTERY || logo_type == BOOTSPLASH_CENTER) {
		*logo_halignment = config->halignment;
		*logo_valignment = config->valignment;
		/* Override logo alignment if the default screen orientation is not normal */
		if (config->panel_orientation != LB_FB_ORIENTATION_NORMAL)
			*logo_valignment = FW_SPLASH_VALIGNMENT_CENTER;
		*logo_bottom_margin = 0;
	} else if (logo_type == BOOTSPLASH_FOOTER) {
		*logo_halignment = FW_SPLASH_HALIGNMENT_CENTER;
		*logo_valignment = FW_SPLASH_VALIGNMENT_CENTER;
		switch (config->panel_orientation) {
		case LB_FB_ORIENTATION_RIGHT_UP:
			*logo_halignment = FW_SPLASH_HALIGNMENT_LEFT;
			break;
		case LB_FB_ORIENTATION_LEFT_UP:
			*logo_halignment = FW_SPLASH_HALIGNMENT_RIGHT;
			break;
		case LB_FB_ORIENTATION_BOTTOM_UP:
			*logo_valignment = FW_SPLASH_VALIGNMENT_TOP;
			break;
		default: /* LB_FB_ORIENTATION_NORMAL (default) */
			*logo_valignment = FW_SPLASH_VALIGNMENT_BOTTOM;
			break;
		}
		*logo_bottom_margin = config->logo_bottom_margin;
	} else { // Default values
		*logo_halignment = FW_SPLASH_HALIGNMENT_CENTER;
		*logo_valignment = FW_SPLASH_VALIGNMENT_CENTER;
		*logo_bottom_margin = 0;
	}
}

/*
 * Loads, converts, and renders a BMP logo to the framebuffer.
 *
 * logo_type: Logo type.
 * config: Logo configuration information.
 *
 * Returns 0 on success, -1 on failure.
 */
static int load_and_render_logo_to_framebuffer(
	enum bootsplash_type logo_type,
	struct logo_config *config
)
{
	uintptr_t logo;
	size_t logo_size;
	size_t blt_size;
	uintptr_t blt_buffer;
	uint32_t logo_height, logo_width;
	struct logo_coordinates logo_coords;
	enum fw_splash_horizontal_alignment halignment;
	enum fw_splash_vertical_alignment valignment;
	uint8_t logo_bottom_margin;

	if (!config)
		return -1;

	logo = (uintptr_t)bmp_load_logo_by_type(logo_type, &logo_size);

	if (!logo || logo_size < sizeof(struct bmp_image_header)) {
		printk(BIOS_ERR, "%s: BMP image (%zu) is less than expected minimum size (%zu).\n",
				 __func__, logo_size, sizeof(struct bmp_image_header));
		return -1;
	}

	convert_bmp_to_blt(logo, logo_size, &blt_buffer, &blt_size,
				   &logo_height, &logo_width, config->panel_orientation);

	get_logo_layout(logo_type, config, &halignment, &valignment, &logo_bottom_margin);

	logo_coords = calculate_logo_coordinates(config->horizontal_resolution,
		 config->vertical_resolution, logo_width, logo_height, halignment, valignment);

	if (logo_bottom_margin) {
		if (config->panel_orientation == LB_FB_ORIENTATION_BOTTOM_UP ||
				 config->panel_orientation == LB_FB_ORIENTATION_NORMAL)
			logo_coords.y -= logo_bottom_margin;
		else
			logo_coords.x -= logo_bottom_margin;
	}

	copy_logo_to_framebuffer(config->framebuffer_base, config->bytes_per_scanline, blt_buffer,
				 logo_width, logo_height, logo_coords.x, logo_coords.y);

	bmp_release_logo();

	return 0;
}

/*
 * Loads, converts, and renders a BMP logo to the framebuffer.
 * The logo type (primary or low battery etc.) is seletcted dynamically.
 *
 * config: Logo configuration information.
 *
 */
void render_logo_to_framebuffer(struct logo_config *config)
{
	if (!config)
		return;

	if (config->framebuffer_base == 0) {
		/* Try to load from already populated framebuffer information */
		struct lb_framebuffer framebuffer;
		memset(&framebuffer, 0, sizeof(struct lb_framebuffer));
		fill_lb_framebuffer(&framebuffer);
		/* Exit if framebuffer is still not available */
		if (framebuffer.physical_address == 0)
			return;
		config->framebuffer_base = framebuffer.physical_address;
		config->horizontal_resolution = framebuffer.x_resolution;
		config->vertical_resolution = framebuffer.y_resolution;
		config->bytes_per_scanline = framebuffer.bytes_per_line;
	}

	/*
	 * If the device is in low-battery mode and the low-battery splash screen is being
	 * displayed, prevent further operation and bail out early.
	 */
	if (platform_is_low_battery_shutdown_needed()) {
		if (load_and_render_logo_to_framebuffer(BOOTSPLASH_LOW_BATTERY, config) != 0) {
			printk(BIOS_ERR, "%s: Failed to render low-battery logo.\n", __func__);
		}
		return;
	}

	/* Render the main logo */
	if (load_and_render_logo_to_framebuffer(BOOTSPLASH_CENTER, config) != 0) {
		printk(BIOS_ERR, "%s: Failed to render main splash screen logo.\n", __func__);
	} else if (CONFIG(SPLASH_SCREEN_FOOTER)) {
		/* Render the footer logo */
		if (load_and_render_logo_to_framebuffer(BOOTSPLASH_FOOTER, config) != 0)
			printk(BIOS_ERR, "%s: Failed to render footer logo.\n", __func__);
	}
}

static void release_logo(void *arg_unused)
{
	bmp_release_logo();
}

BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_EXIT, release_logo, NULL);
