/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
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

#ifndef _MAINBOARD_M3885_H
#define _MAINBOARD_M3885_H

#define M3885_CMCMD		0x04
#define M3885_CMDAT1		0x05
#define M3885_CMDAT2		0x06
#define M3885_CMDAT3		0x07

#define M3885_GPIO_LEVEL	(0<<7)
#define M3885_GPIO_PULSE	(1<<7)

#define M3885_GPIO_READ		(0<<5)
#define M3885_GPIO_SET		(1<<5)
#define M3885_GPIO_CLEAR	(2<<5)
#define M3885_GPIO_TOGGLE	(3<<5)

#define M3885_GPIO_P14		(0x00<<0)
#define M3885_GPIO_P15		(0x01<<0)
#define M3885_GPIO_P16		(0x02<<0)
#define M3885_GPIO_P17		(0x03<<0)

#define M3885_GPIO_P54		(0x04<<0)
#define M3885_GPIO_P55		(0x05<<0)
#define M3885_GPIO_P56		(0x06<<0)
#define M3885_GPIO_P57		(0x07<<0)

#define M3885_GPIO_P20		(0x08<<0)
#define M3885_GPIO_P21		(0x09<<0)
#define M3885_GPIO_P22		(0x0a<<0)
#define M3885_GPIO_P23		(0x0b<<0)
#define M3885_GPIO_P24		(0x0c<<0)
#define M3885_GPIO_P25		(0x0d<<0)
#define M3885_GPIO_P26		(0x0e<<0)
#define M3885_GPIO_P27		(0x0f<<0)

#define M3885_GPIO_P40		(0x10<<0)
#define M3885_GPIO_P41		(0x11<<0)
#define M3885_GPIO_P42		(0x12<<0)
#define M3885_GPIO_P43		(0x13<<0)
#define M3885_GPIO_P44		(0x14<<0)
#define M3885_GPIO_P45		(0x15<<0)
#define M3885_GPIO_P46		(0x16<<0)
#define M3885_GPIO_P47		(0x17<<0)

#define M3885_GPIO_P60		(0x18<<0)
#define M3885_GPIO_P61		(0x19<<0)
#define M3885_GPIO_P62		(0x1a<<0)
#define M3885_GPIO_P63		(0x1b<<0)
#define M3885_GPIO_P64		(0x1c<<0)
#define M3885_GPIO_P65		(0x1d<<0)
#define M3885_GPIO_P66		(0x1e<<0)
#define M3885_GPIO_P67		(0x1f<<0)

void m3885_configure_multikey(void);
u8 m3885_gpio(u8 value);

#endif
