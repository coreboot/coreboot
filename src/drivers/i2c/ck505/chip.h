/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef DRIVERS_CK505_CHIP_H
#define DRIVERS_CK505_CHIP_H

struct drivers_i2c_ck505_config {
	const int nregs;
	const u8 regs[32];
	const u8 mask[32];
};

#endif
