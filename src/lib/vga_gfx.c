/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootsplash.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <limits.h>
#include <stdint.h>
#include <string.h>

#include "render_bmp.h"

#define FONT_WIDTH 8
#define FONT_HEIGHT 16
#define FONT_PACK_SIZE 256

#define SPACE_CHAR ' '
#define NEWLINE_CHAR '\n'

extern const unsigned char vga_font_8x16[FONT_PACK_SIZE][FONT_HEIGHT];

/* Global variables */
static enum lb_fb_orientation current_orientation;
static unsigned char *buffer;
static int buffer_width;
static int buffer_height;

/*
 * Sets or clears a single pixel in the bitmap buffer,
 * considering current screen orientation.
 * @param x_logical The logical X coordinate of the pixel.
 * @param y_logical The logical Y coordinate of the pixel.
 * @param value 1 to set the pixel, 0 to clear it.
 */
static void set_pixel(int x_logical, int y_logical, int value)
{
	int buffer_x, buffer_y;

	/*
	 * Transform logical (x_logical, y_logical) to physical
	 * (buffer_x, buffer_y) based on the current_orientation.
	 * The physical buffer is always buffer_width x buffer_height.
	 */
	switch (current_orientation) {
	case LB_FB_ORIENTATION_RIGHT_UP:
		/* Logical (x,y) -> Physical (buffer_width-1-y, x) */
		buffer_x = buffer_width - 1 - y_logical;
		buffer_y = x_logical;
		break;
	case LB_FB_ORIENTATION_BOTTOM_UP:
		/*
		 * Logical (x,y) ->
		 * Physical (buffer_width-1-x, buffer_height-1-y)
		 */
		buffer_x = buffer_width - 1 - x_logical;
		buffer_y = buffer_height - 1 - y_logical;
		break;
	case LB_FB_ORIENTATION_LEFT_UP:
		/* Logical (x,y) -> Physical (y, buffer_height-1-x) */
		buffer_x = y_logical;
		buffer_y = buffer_height - 1 - x_logical;
		break;
	case LB_FB_ORIENTATION_NORMAL:
	default:
		buffer_x = x_logical;
		buffer_y = y_logical;
		break;
	}

	/* Now, perform bounds checking against the physical buffer */
	if (buffer_x >= 0 && buffer_x < buffer_width &&
			buffer_y >= 0 && buffer_y < buffer_height) {
		/*
		 * Calculate the byte index
		 * and bit position within that byte
		 */
		int pixel_index = buffer_y * buffer_width + buffer_x;
		int byte_idx = pixel_index / BITS_PER_BYTE;
		int bit_pos = (BITS_PER_BYTE - 1) - (pixel_index % BITS_PER_BYTE);

		/* Set or clear the pixel */
		if (value)
			buffer[byte_idx] |= (1 << bit_pos);
		else
			buffer[byte_idx] &= ~(1 << bit_pos);
	}
}

/*
 * Draws a single character onto the bitmap buffer.
 * @param x The X coordinate for the top-left corner of the character.
 * @param y The Y coordinate for the top-left corner of the character.
 * @param character The character to draw.
 */
static void draw_char(int x, int y, char character)
{
	const unsigned char *char_bitmap;
	char_bitmap = vga_font_8x16[(uint8_t)character];
	if (!char_bitmap)
		return;

	for (int row = 0; row < FONT_HEIGHT; row++) {
		unsigned char row_data = char_bitmap[row];
		for (int col = 0; col < FONT_WIDTH; col++) {
			if ((row_data >> (FONT_WIDTH - 1 - col)) & 0x01)
				set_pixel(x + col, y + row, 1);
		}
	}
}

/*
 * Draws a single line of text onto the bitmap buffer.
 * This function DOES NOT handle newlines or automatic wrapping.
 * @param x The X coordinate for the top-left corner of the line.
 * @param y The Y coordinate for the top-left corner of the line.
 * @param text The single line string to draw (no newlines).
 * @param length The length of the string.
 * @param effective_width The current effective screen width.
 */
static void draw_string(int x, int y, const char *text,
		int length, int effective_width)
{
	int current_x = x;
	for (int i = 0; i < length; i++) {
		char character = text[i];

		/* Stop drawing if out of logical bounds horizontally */
		if (current_x + FONT_WIDTH > effective_width)
			break;

		draw_char(current_x, y, character);

		/* Move to the next character position */
		current_x += FONT_WIDTH;
	}
}

/*
 * Reads a single raw pixel's value (0 or 1) from the bitmap
 * buffer.
 * @param x The X coordinate of the pixel.
 * @param y The Y coordinate of the pixel.
 * @return 1 if the pixel is set, 0 if it's clear.
 */
static int get_raw_pixel(int x, int y)
{
	/* Assuming the coordinates are within bounds */

	/* Calculate bit position for the given coordinates */
	int pixel_index = y * buffer_width + x;
	int byte_idx = pixel_index / BITS_PER_BYTE;
	int bit_pos = (BITS_PER_BYTE - 1) - (pixel_index % BITS_PER_BYTE);
	return (buffer[byte_idx] >> bit_pos) & 0x01;
}

/*
 * Truncates the content of the bitmap buffer in-place, moving
 * set pixels to the top-left corner and clearing the rest.
 * Ensures truncated width and height are multiples of 8.
 * @param truncated_width Output pointer for the new width.
 * @param truncated_height Output pointer for the new height.
 */
static void truncate_bitmap_buffer(
	int *truncated_width, int *truncated_height)
{
	int min_pixel_x = INT_MAX, max_pixel_x = INT_MIN;
	int min_pixel_y = INT_MAX, max_pixel_y = INT_MIN;
	bool found_pixel = false;

	/* Validate input params */
	if (!truncated_width || !truncated_height)
		return;

	/* Scan the source buffer to find the actual bounding box of
	 * set pixels
	 */
	for (int y = 0; y < buffer_height; y++) {
		for (int x = 0; x < buffer_width; x++) {
			if (get_raw_pixel(x, y)) {
				min_pixel_x = MIN(min_pixel_x, x);
				max_pixel_x = MAX(max_pixel_x, x);
				min_pixel_y = MIN(min_pixel_y, y);
				max_pixel_y = MAX(max_pixel_y, y);
				found_pixel = true;
			}
		}
	}

	if (!found_pixel) {
		/* No set pixels found, effectively empty. */
		memset(buffer, 0, buffer_width * buffer_height / BITS_PER_BYTE);
		*truncated_width = 0;
		*truncated_height = 0;
		printk(BIOS_ERR, "BITMAP: Unable to crop. No pixels\n");
		return;
	}

	/*
	 * The width of the truncated content in pixels, rounded up to
	 * multiple of FONT_WIDTH.
	 */
	int min_byte_x = (min_pixel_x / BITS_PER_BYTE);
	int max_byte_x = (max_pixel_x / BITS_PER_BYTE);
	int new_content_width = (max_byte_x - min_byte_x + 1) * BITS_PER_BYTE;
	new_content_width += FONT_WIDTH - 1;
	new_content_width /= FONT_WIDTH;
	new_content_width *= FONT_WIDTH;
	new_content_width = MIN(new_content_width, buffer_width);

	/*
	 * The height of the truncated content in pixels, rounded up to
	 * multiple of FONT_HEIGHT.
	 */
	int new_content_height = max_pixel_y - min_pixel_y + 1;
	new_content_height += FONT_HEIGHT - 1;
	new_content_height /= FONT_HEIGHT;
	new_content_height *= FONT_HEIGHT;
	new_content_height = MIN(new_content_height, buffer_height);

	/* Calculate number of bytes per row */
	int source_buffer_row_bytes = buffer_width / BITS_PER_BYTE;
	int new_content_row_bytes = new_content_width / BITS_PER_BYTE;

	/*
	 * Perform the in-place byte copy (shift)
	 * We copy bytes from their original location (aligned to
	 * min_byte_x, min_pixel_y) to the top-left of the buffer
	 * (0,0).
	 */
	for (int y_dest = 0; y_dest < new_content_height; y_dest++) {
		/* Calculate source byte index */
		int y_src_pixel = min_pixel_y + y_dest;
		int src_row_start_byte_idx = y_src_pixel * source_buffer_row_bytes;
		src_row_start_byte_idx += min_byte_x;

		/* Calculate destination byte index */
		int dest_row_start_byte_idx = y_dest * new_content_row_bytes;

		/* Ensure we don't read past the end of the source buffer's data */
		if (y_src_pixel < buffer_height) {
			memcpy(&buffer[dest_row_start_byte_idx],
				   &buffer[src_row_start_byte_idx],
				   new_content_row_bytes);
		}
	}

	/* Update the output pointers with the new effective dimensions */
	*truncated_width = new_content_width;
	*truncated_height = new_content_height;
}

/*
 * Renders the given text onto the bitmap buffer with specified
 * orientation and centers it.
 * @param image_bitmap_buffer The 1-bit-per-pixel buffer (will be modified).
 * @param orientation The screen orientation to apply.
 * @param text_to_render The text string to render.
 * @param image_width Image width (will be modified).
 * @param image_height Image height (will be modified).
 */
void render_text_to_bitmap_buffer(unsigned char *image_bitmap_buffer,
	enum lb_fb_orientation orientation, const char *text_to_render,
	int *image_width, int *image_height)
{
	/* Input validation */
	if (!image_bitmap_buffer) {
		printk(BIOS_ERR, "BITMAP: Input buffer is NULL\n");
		return;
	}

	if (!text_to_render || !image_width || !image_height) {
		printk(BIOS_ERR, "BITMAP: Input params are NULL\n");
		return;
	}

	printk(BIOS_INFO, "BITMAP: Rendering text - %s\n", text_to_render);

	/* Set global variables to use for all the operations */
	buffer = image_bitmap_buffer;
	buffer_width = *image_width;
	buffer_height = *image_height;
	current_orientation = orientation;

	/*
	 * Determine the effective screen dimensions based on
	 * orientation
	 */
	int effective_screen_width;
	int effective_screen_height;
	int effective_screen_chars;

	switch (current_orientation) {
	case LB_FB_ORIENTATION_LEFT_UP:
	case LB_FB_ORIENTATION_RIGHT_UP:
		effective_screen_width = buffer_height;
		effective_screen_height = buffer_width;
		break;
	case LB_FB_ORIENTATION_NORMAL:
	case LB_FB_ORIENTATION_BOTTOM_UP:
	default:
		effective_screen_width = buffer_width;
		effective_screen_height = buffer_height;
		break;
	}

	effective_screen_chars = effective_screen_width / FONT_WIDTH;

	memset(buffer, 0, buffer_width * buffer_height / BITS_PER_BYTE);

	int current_line_y = 0;
	char *line_end = (char *)text_to_render;
	char *line_start;

	while (*line_end && current_line_y < effective_screen_height) {
		/* Skip newline and extra space characters */
		while ((*line_end) &&
			((*line_end == NEWLINE_CHAR) ||
			 (*line_end == SPACE_CHAR)))
			line_end++;

		line_start = line_end;

		/* Find the end of the current word */
		while (*line_end && *line_end != SPACE_CHAR && *line_end != NEWLINE_CHAR)
			line_end++;

		/* Handle words longer than the screen width */
		if (line_end - line_start > effective_screen_chars)
			line_end = line_start + effective_screen_chars;

		/* Find the optimal line break point without splitting words. */
		while (*line_end) {
			char *word_end = line_end + 1;
			/* Skip leading spaces */
			while (*word_end && *word_end == SPACE_CHAR)
				word_end++;
			/* Find the end of the current word */
			while (*word_end && *word_end != SPACE_CHAR &&
				*word_end != NEWLINE_CHAR)
				word_end++;

			/* Count the word in if it fits into the current line */
			if (word_end - line_start + 1 > effective_screen_chars)
				break;

			line_end = word_end;
		}

		/*
		 * Consider newline character if it is within
		 * current line
		 */
		char *newline_pos = strchr(line_start, NEWLINE_CHAR);
		if (newline_pos && (newline_pos < line_end))
			line_end = newline_pos;

		int line_length = line_end - line_start;
		if (line_length == 0)
			continue;

		/* Calculate x-coordinate for the current line */
		int current_line_x = (effective_screen_width - line_length * FONT_WIDTH) / 2;

		/* Draw the string on the buffer */
		draw_string(current_line_x, current_line_y, line_start,
				line_length, effective_screen_width);

		/* Update the y-coordinate for the next line */
		current_line_y += FONT_HEIGHT;
	}

	truncate_bitmap_buffer(image_width, image_height);
}

/* Standard 16-color VGA Mode 12h palette (IRGB bitplane order: P3=I, P2=R, P1=G, P0=B) */
static const uint8_t vga16_palette[16][3] = {
	{ 0x00, 0x00, 0x00 }, /*  0: Black          */
	{ 0x00, 0x00, 0xaa }, /*  1: Blue           */
	{ 0x00, 0xaa, 0x00 }, /*  2: Green          */
	{ 0x00, 0xaa, 0xaa }, /*  3: Cyan           */
	{ 0xaa, 0x00, 0x00 }, /*  4: Red            */
	{ 0xaa, 0x00, 0xaa }, /*  5: Magenta        */
	{ 0xaa, 0x55, 0x00 }, /*  6: Brown          */
	{ 0xaa, 0xaa, 0xaa }, /*  7: Light Gray     */
	{ 0x55, 0x55, 0x55 }, /*  8: Dark Gray      */
	{ 0x55, 0x55, 0xff }, /*  9: Bright Blue    */
	{ 0x55, 0xff, 0x55 }, /* 10: Bright Green   */
	{ 0x55, 0xff, 0xff }, /* 11: Bright Cyan    */
	{ 0xff, 0x55, 0x55 }, /* 12: Bright Red     */
	{ 0xff, 0x55, 0xff }, /* 13: Bright Magenta */
	{ 0xff, 0xff, 0x55 }, /* 14: Yellow         */
	{ 0xff, 0xff, 0xff }, /* 15: Bright White   */
};

static uint8_t rgb_to_vga_color(uint8_t r, uint8_t g, uint8_t b, size_t num_planes)
{
	if (num_planes == 1) {
		uint32_t lum = (77U * r + 150U * g + 29U * b) >> 8;
		return lum >= 118 ? 1 : 0;
	}

	uint32_t best_dist = UINT32_MAX;
	uint8_t best_idx = 0;

	for (uint8_t i = 0; i < 16; i++) {
		int dr = (int)r - vga16_palette[i][0];
		int dg = (int)g - vga16_palette[i][1];
		int db = (int)b - vga16_palette[i][2];
		uint32_t dist = (uint32_t)(dr * dr + dg * dg + db * db);
		if (dist < best_dist) {
			best_dist = dist;
			best_idx = i;
		}
	}

	return best_idx;
}

static uint8_t read_indexed_bmp_pixel(const uint8_t *row, size_t x, uint16_t bpp)
{
	switch (bpp) {
	case 4:
		return (x & 1) ? (row[x >> 1] & 0x0f) : (row[x >> 1] >> 4);
	default:
		return 0;
	}
}

bool render_bmp_to_bitmap_buffer(unsigned char *image_bitmap_buffer,
	size_t bitmap_buffer_size, enum lb_fb_orientation orientation,
	const void *bmp_data, size_t bmp_size, size_t num_planes,
	int *image_width, int *image_height)
{
	if (!image_bitmap_buffer || !bmp_data || !image_width || !image_height) {
		printk(BIOS_ERR, "VGA12 BMP: Invalid input parameters\n");
		return false;
	}

	if (num_planes != 1 && num_planes != 4) {
		printk(BIOS_ERR, "VGA12 BMP: Unsupported plane count %zu\n", num_planes);
		return false;
	}

	if (bmp_size < sizeof(struct bmp_image_header)) {
		printk(BIOS_ERR, "VGA12 BMP: Buffer too small (%zu bytes)\n", bmp_size);
		return false;
	}

	const struct bmp_image_header *hdr = (const struct bmp_image_header *)bmp_data;
	if (!bmp_do_image_authentication(hdr) || hdr->Planes != 1 ||
	    !bmp_pixel_data_fits(hdr, bmp_size))
		return false;

	int num_colors = bmp_get_color_map_num(hdr);

	if (num_colors <= 0) {
		printk(BIOS_ERR, "VGA12 BMP: Unsupported BPP (%u) or truncated color map\n",
		       hdr->BitPerPixel);
		return false;
	}

	size_t num_entries = hdr->NumberOfColors ?
			     MIN((size_t)hdr->NumberOfColors, (size_t)num_colors) :
			     (size_t)num_colors;

	uint8_t palette_lut[256] = { 0 };

	const struct bmp_color_map *cmap =
		(const struct bmp_color_map *)((const uint8_t *)bmp_data + sizeof(*hdr));

	for (size_t i = 0; i < num_entries; i++)
		palette_lut[i] = rgb_to_vga_color(cmap[i].Red, cmap[i].Green,
						  cmap[i].Blue, num_planes);

	/*
	 * rot_w / rot_h: BMP dimensions after applying panel orientation
	 * (swapped for 90-degree LEFT_UP / RIGHT_UP rotations).
	 */
	uint32_t rot_w, rot_h;
	bmp_get_oriented_dimensions(hdr, orientation, &rot_w, &rot_h);

	/*
	 * max_w / max_h: Caller-supplied maximum dimensions for the rendered bitmap
	 * (e.g., VGA12 640x480). Width is rounded down to an 8-pixel byte boundary
	 * since each planar VGA byte packs 8 horizontal pixels.
	 */
	int max_w = ALIGN_DOWN(*image_width, BITS_PER_BYTE);
	int max_h = *image_height;
	if (max_w <= 0 || max_h <= 0 || rot_w > INT_MAX || rot_h > INT_MAX)
		return false;

	/*
	 * out_w / out_h: Actual output bitmap dimensions written to the planar buffer.
	 * rot_w is padded up to an 8-pixel boundary and both axes are clamped to
	 * max_w / max_h so the bitmap never exceeds the caller's target area.
	 */
	int out_w = MIN(ALIGN_UP((int)rot_w, BITS_PER_BYTE), max_w);
	int out_h = MIN((int)rot_h, max_h);

	/*
	 * off_x / off_y: Centering offsets mapping rotated BMP coordinates (rot_w x rot_h)
	 * into the output buffer (out_w x out_h):
	 *  - positive when out_w was rounded up to an 8-pixel boundary (centers padding)
	 *  - negative when rot_w/rot_h exceeds max_w/max_h (center-crops the image)
	 */
	int off_x = (out_w - (int)rot_w) / 2;
	int off_y = (out_h - (int)rot_h) / 2;

	size_t dst_row_bytes = (size_t)(out_w / BITS_PER_BYTE);
	size_t plane_size = dst_row_bytes * (size_t)out_h;

	if (plane_size > bitmap_buffer_size / num_planes) {
		printk(BIOS_ERR, "VGA12 BMP: Output (%zu bytes) exceeds buffer (%zu bytes)\n",
		       plane_size * num_planes, bitmap_buffer_size);
		return false;
	}

	memset(image_bitmap_buffer, 0, plane_size * num_planes);

	size_t src_row_stride = bmp_row_stride(hdr);
	const uint8_t *row = (const uint8_t *)bmp_data + hdr->ImageOffset;

	for (size_t height = 0; height < hdr->PixelHeight; height++, row += src_row_stride) {
		for (size_t width = 0; width < hdr->PixelWidth; width++) {
			uint8_t color = palette_lut[read_indexed_bmp_pixel(row, width,
									   hdr->BitPerPixel)];
			if (!color)
				continue;

			/* Map BMP (width, height) to rotated coords (rx, ry) */
			size_t rx, ry;

			bmp_calculate_adj_height_width(hdr, &rx, &ry, orientation, width, height);

			/* Shift into destination buffer coords (dx, dy) and clip if cropped */
			int dx = (int)rx + off_x;
			int dy = (int)ry + off_y;
			if (dx < 0 || dx >= out_w || dy < 0 || dy >= out_h)
				continue;

			/*
			 * Planar VGA Mode 12h packs 8 horizontal pixels per byte in
			 * MSB-first order (dx % 8 == 0 -> bit 7, dx % 8 == 7 -> bit 0):
			 *
			 *   dx % 8 :   0     1     2     3     4     5     6     7
			 *   bit    :  b7    b6    b5    b4    b3    b2    b1    b0
			 *   mask   : 0x80  0x40  0x20  0x10  0x08  0x04  0x02  0x01
			 *
			 * - byte_off locates the byte in row dy (dy * dst_row_bytes)
			 *   and 8-pixel group along that row (dx >> 3 == dx / 8).
			 * - mask isolates the single bit for pixel dx so '|=' turns
			 *   only this pixel ON without disturbing the other 7 pixels
			 *   sharing the same byte (buffer was pre-zeroed by memset).
			 * - Each bit p of the 4-bit VGA color index (P0=B, P1=G,
			 *   P2=R, P3=I) is written into its own contiguous bitplane
			 *   at offset (p * plane_size + byte_off).
			 */
			size_t byte_off = (size_t)dy * dst_row_bytes + (size_t)(dx >> 3);
			uint8_t mask = (uint8_t)(1U << (7 - (dx & 7)));

			for (size_t p = 0; p < num_planes; p++) {
				if ((color >> p) & 0x1)
					image_bitmap_buffer[p * plane_size + byte_off] |= mask;
			}
		}
	}

	*image_width = out_w;
	*image_height = out_h;

	return true;
}

void vga12_calculate_logo_position(int img_width, int img_height,
	enum lb_fb_orientation orientation,
	enum fw_updater_vertical_alignment valignment,
	uint8_t margin, int *pos_x, int *pos_y)
{
	if (!pos_x || !pos_y)
		return;

	int max_x = MAX(0, VGA12_WIDTH - img_width);
	int max_y = MAX(0, VGA12_HEIGHT - img_height);
	int x = ALIGN_DOWN(max_x / 2, BITS_PER_BYTE);
	int y = max_y / 2;

	if (valignment == FW_UPDATER_VALIGNMENT_TOP) {
		switch (orientation) {
		case LB_FB_ORIENTATION_BOTTOM_UP:
			y = MAX(0, max_y - (int)margin);
			break;
		case LB_FB_ORIENTATION_RIGHT_UP:
			x = ALIGN_DOWN(MAX(0, max_x - (int)margin), BITS_PER_BYTE);
			break;
		case LB_FB_ORIENTATION_LEFT_UP:
			x = ALIGN_DOWN(MIN((int)margin, max_x), BITS_PER_BYTE);
			break;
		case LB_FB_ORIENTATION_NORMAL:
		default:
			y = MIN((int)margin, max_y);
			break;
		}
	} else if (valignment == FW_UPDATER_VALIGNMENT_BOTTOM) {
		switch (orientation) {
		case LB_FB_ORIENTATION_BOTTOM_UP:
			y = MIN((int)margin, max_y);
			break;
		case LB_FB_ORIENTATION_RIGHT_UP:
			x = ALIGN_DOWN(MIN((int)margin, max_x), BITS_PER_BYTE);
			break;
		case LB_FB_ORIENTATION_LEFT_UP:
			x = ALIGN_DOWN(MAX(0, max_x - (int)margin), BITS_PER_BYTE);
			break;
		case LB_FB_ORIENTATION_NORMAL:
		default:
			y = MAX(0, max_y - (int)margin);
			break;
		}
	}

	*pos_x = x;
	*pos_y = y;
}
