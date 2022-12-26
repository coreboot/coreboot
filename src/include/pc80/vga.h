/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef VGA_H
#define VGA_H

#define VGA_FB 0xB8000
#define VGA_FB_SIZE 0x4000 /* char + attr = word sized so 0x8000 / 2 */
#define VGA_COLUMNS 80
#define VGA_LINES 25

#define VGA_TEXT_HORIZONTAL_TOP 0
#define VGA_TEXT_HORIZONTAL_MIDDLE (VGA_LINES / 2)

enum VGA_TEXT_ALIGNMENT {
	VGA_TEXT_LEFT,
	VGA_TEXT_CENTER,
	VGA_TEXT_RIGHT,
};

void vga_io_init(void);

void vga_textmode_init(void);

void vga_cursor_enable(int enable);
void vga_cursor_reset(void);
void vga_cursor_set(unsigned int line, unsigned int character);

void vga_frame_set(unsigned int line, unsigned int character);

void vga_line_write(unsigned int line, const char *string);

/*
 * vga_write_text() writes a line of text aligned left/center/right
 * horizontally on the screen (i.e. enum VGA_TEXT_ALIGNMENT)
 */
void vga_write_text(enum VGA_TEXT_ALIGNMENT alignment, unsigned int line, const char *string);

#endif /* VGA_H */
