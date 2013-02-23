/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Luc Verhaegen <libv@skynet.be>
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

#ifndef VGA_IO_H
#define VGA_IO_H

/*
 * All IO necessary to poke VGA registers.
 */

/* VGA Enable */
unsigned char vga_enable_read(void);
void vga_enable_write(unsigned char value);
void vga_enable_mask(unsigned char value, unsigned char mask);

/* Miscellaneous register */
unsigned char vga_misc_read(void);
void vga_misc_write(unsigned char value);
void vga_misc_mask(unsigned char value, unsigned char mask);

/* Sequencer registers. */
unsigned char vga_sr_read(unsigned char index);
void vga_sr_write(unsigned char index, unsigned char value);
void vga_sr_mask(unsigned char index, unsigned char value, unsigned char mask);

/* CR registers. */
unsigned char vga_cr_read(unsigned char index);
void vga_cr_write(unsigned char index, unsigned char value);
void vga_cr_mask(unsigned char index, unsigned char value, unsigned char mask);

/* Attribute registers. */
unsigned char vga_ar_read(unsigned char index);
void vga_ar_write(unsigned char index, unsigned char value);
void vga_ar_mask(unsigned char index, unsigned char value, unsigned char mask);

/* Graphics registers. */
unsigned char vga_gr_read(unsigned char index);
void vga_gr_write(unsigned char index, unsigned char value);
void vga_gr_mask(unsigned char index, unsigned char value, unsigned char mask);

/* DAC functions. */
void vga_palette_enable(void);
void vga_palette_disable(void);
unsigned char vga_dac_mask_read(void);
void vga_dac_mask_write(unsigned char mask);
void vga_dac_read_address(unsigned char address);
void vga_dac_write_address(unsigned char address);
unsigned char vga_dac_data_read(void);
void vga_dac_data_write(unsigned char data);

#endif /* VGA_IO_H */
