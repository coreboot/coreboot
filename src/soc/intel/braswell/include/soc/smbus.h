/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef _SOC_SMBUS_H_
#define _SOC_SMBUS_H_

/* PCI Configuration Space SMBus */
#define HOSTC			0x40
#define HOSTC_I2C_EN		(1 << 2)

int smbus_i2c_block_write(u8 addr, u8 bytes, u8 *buf);
#endif /* _SOC_SMBUS_H_ */
