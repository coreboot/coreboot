/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SOC_SMBUS_H_
#define _SOC_SMBUS_H_

/* PCI Configuration Space SMBus */
#define HOSTC			0x40
#define HOSTC_I2C_EN		(1 << 2)

int smbus_i2c_block_write(u8 addr, u8 bytes, u8 *buf);
#endif /* _SOC_SMBUS_H_ */
