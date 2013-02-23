/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000 AG Electronics Ltd.
 * Copyright (C) 2003-2004 Linux Networx
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

#ifndef SUPERIO_NSC_PC87351_PC87351_H
#define SUPERIO_NSC_PC87351_PC87351_H

#define PC87351_FDC  0x00 /* Floppy */
#define PC87351_PP   0x01 /* Parallel port */
#define PC87351_SP2  0x02 /* Com2 */
#define PC87351_SP1  0x03 /* Com1 */
#define PC87351_SWC  0x04 /* System wakeup control */
#define PC87351_KBCM 0x05 /* Mouse */
#define PC87351_KBCK 0x06 /* Keyboard */
#define PC87351_GPIO 0x07 /* General purpose IO */
#define PC87351_FSD  0x08 /* Fan speed device */

#endif
