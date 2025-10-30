/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootsplash.h>
#include <console/console.h>
#include <limits.h>
#include <string.h>

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

		/* Calculate x-cordinate for the current line */
		int current_line_x = (effective_screen_width - line_length * FONT_WIDTH) / 2;

		/* Draw the string on the buffer */
		draw_string(current_line_x, current_line_y, line_start,
				line_length, effective_screen_width);

		/* Update the y-cordinate for the next line */
		current_line_y += FONT_HEIGHT;
	}

	truncate_bitmap_buffer(image_width, image_height);
}
