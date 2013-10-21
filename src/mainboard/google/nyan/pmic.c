/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
 * Copyright (c) 2013, NVIDIA CORPORATION.  All rights reserved.
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

#include <delay.h>
#include <device/i2c.h>
#include <stdint.h>
#include <stdlib.h>

#include "pmic.h"

struct pmic_write
{
	uint8_t reg; // Register to write.
	uint8_t val; // Value to write.
};

enum {
	AS3722_I2C_ADDR = 0x40
};

static struct pmic_write pmic_writes[] =
{
	/* Don't need to set up VDD_CORE - already done - by OTP */

	/* First set VDD_CPU to 1.0V, then enable the VDD_CPU regulator. */
	{ 0x00, 0x28 },

	/* Don't write SDCONTROL - it's already 0x7F, i.e. all SDs enabled. */

	/* First set VDD_GPU to 1.0V, then enable the VDD_GPU regulator. */
	{ 0x06, 0x28 },

	/* Don't write SDCONTROL - it's already 0x7F, i.e. all SDs enabled. */

	/* First set VPP_FUSE to 1.2V, then enable the VPP_FUSE regulator. */
	{ 0x12, 0x10 },

	/* Don't write LDCONTROL - it's already 0xFF, i.e. all LDOs enabled. */

	/*
	 * Bring up VDD_SDMMC via the AS3722 PMIC on the PWR I2C bus.
	 * First set it to bypass 3.3V straight thru, then enable the regulator
	 *
	 * NOTE: We do this early because doing it later seems to hose the CPU
	 * power rail/partition startup. Need to debug.
	 */
	{ 0x16, 0x3f },

	/* Don't write LDCONTROL - it's already 0xFF, i.e. all LDOs enabled. */
	/* panel power GPIO O4. Set mode for GPIO4 (0x0c to 7), then set
	 * the value (register 0x20 bit 4)
	 */
	{ 0x0c, 0x07 },
	{ 0x20, 0x10 },
};

void pmic_init(unsigned bus)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(pmic_writes); i++) {
		i2c_write(bus, AS3722_I2C_ADDR, pmic_writes[i].reg, 1,
			  &pmic_writes[i].val, 1);
		udelay(10 * 1000);
	}
}
