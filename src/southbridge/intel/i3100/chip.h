/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Arastra, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

struct southbridge_intel_i3100_config
{
#define I3100_GPIO_USE_MASK      0x03
#define I3100_GPIO_USE_DEFAULT   0x00
#define I3100_GPIO_USE_AS_NATIVE 0x01
#define I3100_GPIO_USE_AS_GPIO   0x02

#define I3100_GPIO_SEL_MASK      0x0c
#define I3100_GPIO_SEL_DEFAULT   0x00
#define I3100_GPIO_SEL_OUTPUT    0x04
#define I3100_GPIO_SEL_INPUT     0x08

#define I3100_GPIO_LVL_MASK      0x30
#define I3100_GPIO_LVL_DEFAULT   0x00
#define I3100_GPIO_LVL_LOW       0x10
#define I3100_GPIO_LVL_HIGH      0x20
#define I3100_GPIO_LVL_BLINK     0x30

#define I3100_GPIO_INV_MASK      0xc0
#define I3100_GPIO_INV_DEFAULT   0x00
#define I3100_GPIO_INV_OFF       0x40
#define I3100_GPIO_INV_ON        0x80

	/* GPIO use select */
	u8 gpio[64];
	int sata_ports_implemented;
	u32 pirq_a_d;
	u32 pirq_e_h;
};
