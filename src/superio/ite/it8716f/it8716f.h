/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 Uwe Hermann <uwe@hermann-uwe.de>
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

#ifndef SUPERIO_ITE_IT8716F_IT8716F_H
#define SUPERIO_ITE_IT8716F_IT8716F_H

/* Datasheet: http://www.ite.com.tw/product_info/PC/Brief-IT8716_2.asp */

/* Logical device numbers (LDNs). */
#define IT8716F_FDC  0x00 /* Floppy */
#define IT8716F_SP1  0x01 /* Com1 */
#define IT8716F_SP2  0x02 /* Com2 */
#define IT8716F_PP   0x03 /* Parallel port */
#define IT8716F_EC   0x04 /* Environment controller */
#define IT8716F_KBCK 0x05 /* PS/2 keyboard */
#define IT8716F_KBCM 0x06 /* PS/2 mouse */
#define IT8716F_GPIO 0x07 /* GPIO */
#define IT8716F_MIDI 0x08 /* MIDI port */
#define IT8716F_GAME 0x09 /* GAME port */
#define IT8716F_IR   0x0a /* Consumer IR */

#if CONFIG_SUPERIO_ITE_IT8716F_OVERRIDE_FANCTL
/* Provided by mainboard, called by IT8716F superio.c. */
void init_ec(u16 base);
#endif

#if defined(__PRE_RAM__)
void it8716f_disable_dev(device_t dev);
void it8716f_enable_dev(device_t dev, u16 iobase);
void it8716f_enable_serial(device_t dev, u16 iobase);
#endif

#endif
