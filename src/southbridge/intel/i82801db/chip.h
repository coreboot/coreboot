/*
 * This file is part of the coreboot project.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
 
#ifndef I82801DB_CHIP_H
#define I82801DB_CHIP_H

struct southbridge_intel_i82801db_config 
{

#define ICH5R_GPIO_USE_MASK      0x03
#define ICH5R_GPIO_USE_DEFAULT   0x00
#define ICH5R_GPIO_USE_AS_NATIVE 0x01
#define ICH5R_GPIO_USE_AS_GPIO   0x02

#define ICH5R_GPIO_SEL_MASK      0x0c
#define ICH5R_GPIO_SEL_DEFAULT   0x00
#define ICH5R_GPIO_SEL_OUTPUT    0x04
#define ICH5R_GPIO_SEL_INPUT     0x08

#define ICH5R_GPIO_LVL_MASK      0x30
#define ICH5R_GPIO_LVL_DEFAULT   0x00
#define ICH5R_GPIO_LVL_LOW       0x10
#define ICH5R_GPIO_LVL_HIGH      0x20
#define ICH5R_GPIO_LVL_BLINK     0x30

#define ICH5R_GPIO_INV_MASK      0xc0
#define ICH5R_GPIO_INV_DEFAULT   0x00
#define ICH5R_GPIO_INV_OFF       0x40
#define ICH5R_GPIO_INV_ON        0x80

	/* GPIO use select */
	unsigned char gpio[64];
	unsigned int  pirq_a_d;
	unsigned int  pirq_e_h;
	int enable_native_ide;
};
extern struct chip_operations southbridge_intel_i82801db_ops;

#endif /* I82801DB_CHIP_H */

