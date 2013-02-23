/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 Luc Verhaegen <libv@skynet.be>
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef VGA_H
#define VGA_H

#define VGA_FB 0xB8000
#define VGA_FB_SIZE 0x4000 /* char + attr = word sized so 0x8000 / 2 */
#define VGA_COLUMNS 80
#define VGA_LINES 25

#if CONFIG_VGA

void vga_io_init(void);

void vga_textmode_init(void);

void vga_cursor_enable(int enable);
void vga_cursor_reset(void);
void vga_cursor_set(unsigned int line, unsigned int character);

void vga_frame_set(unsigned int line, unsigned int character);

void vga_line_write(unsigned int line, const char *string);

#endif /* CONFIG_VGA */

#endif /* VGA_H */
