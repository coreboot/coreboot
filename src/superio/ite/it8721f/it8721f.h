/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2011 QingPei Wang <wangqingpei@gmail.com>
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

#ifndef SUPERIO_ITE_IT8721F_IT8721F_H
#define SUPERIO_ITE_IT8721F_IT8721F_H

#define IT8721F_FDC  0x00 /* Floppy */
#define IT8721F_SP1  0x01 /* Com1 */
#define IT8721F_SP2  0x02 /* Com2 */
#define IT8721F_PP   0x03 /* Parallel port */
#define IT8721F_EC   0x04 /* Environment controller */
#define IT8721F_KBCK 0x05 /* PS/2 keyboard */
#define IT8721F_KBCM 0x06 /* PS/2 mouse */
#define IT8721F_GPIO 0x07 /* GPIO */
#define IT8721F_IR   0x0a /* Consumer IR */

#if defined(__PRE_RAM__)
void it8721f_24mhz_clkin(void);
void it8721f_disable_reboot(void);
void it8721f_enable_serial(device_t dev, u16 iobase);
#endif

#endif
