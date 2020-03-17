/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef VGA_H
#define VGA_H

#define VGA_FB 0xB8000
#define VGA_FB_SIZE 0x4000 /* char + attr = word sized so 0x8000 / 2 */
#define VGA_COLUMNS 80
#define VGA_LINES 25

void vga_io_init(void);

void vga_textmode_init(void);

void vga_cursor_enable(int enable);
void vga_cursor_reset(void);
void vga_cursor_set(unsigned int line, unsigned int character);

void vga_frame_set(unsigned int line, unsigned int character);

void vga_line_write(unsigned int line, const char *string);

#endif /* VGA_H */
