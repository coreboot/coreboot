/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <device/i2c_simple.h>
#include <stdint.h>
#include <reset.h>

#include "pmic.h"

enum {
	AS3722_I2C_ADDR = 0x40
};

struct as3722_init_reg {
	u8 reg;
	u8 val;
	u8 delay;
};

static struct as3722_init_reg init_list[] = {
	{AS3722_SDO0, 0x3C, 1},
	{AS3722_SDO1, 0x32, 0},
	{AS3722_LDO3, 0x59, 0},
	{AS3722_SDO2, 0x3C, 0},
	{AS3722_SDO3, 0x00, 0},
	{AS3722_SDO4, 0x00, 0},
	{AS3722_SDO5, 0x50, 0},
	{AS3722_SDO6, 0x28, 1},
	{AS3722_LDO0, 0x8A, 0},
	{AS3722_LDO1, 0x00, 0},
	{AS3722_LDO2, 0x10, 0},
	{AS3722_LDO4, 0x00, 0},
	{AS3722_LDO5, 0x00, 0},
	{AS3722_LDO6, 0x00, 0},
	{AS3722_LDO7, 0x00, 0},
	{AS3722_LDO9, 0x00, 0},
	{AS3722_LDO10, 0x00, 0},
	{AS3722_LDO11, 0x00, 1},
};

static void pmic_write_reg(unsigned int bus, uint8_t reg, uint8_t val, int do_delay)
{
	if (i2c_writeb(bus, AS3722_I2C_ADDR, reg, val)) {
		printk(BIOS_ERR, "%s: reg = 0x%02X, value = 0x%02X failed!\n",
			__func__, reg, val);
		/* Reset the SoC on any PMIC write error */
		board_reset();
	} else {
		if (do_delay)
			udelay(500);
	}
}

static void pmic_slam_defaults(unsigned int bus)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(init_list); i++) {
		struct as3722_init_reg *reg = &init_list[i];
		pmic_write_reg(bus, reg->reg, reg->val, reg->delay);
	}
}

void pmic_init(unsigned int bus)
{
	/*
	 * Don't need to set up VDD_CORE - already done - by OTP
	 * Don't write SDCONTROL - it's already 0x7F, i.e. all SDs enabled.
	 * Don't write LDCONTROL - it's already 0xFF, i.e. all LDOs enabled.
	 */

	/* Restore PMIC POR defaults, in case kernel changed 'em */
	pmic_slam_defaults(bus);

	/* First set VDD_CPU to 1.2V, then enable the VDD_CPU regulator. */
	pmic_write_reg(bus, 0x00, 0x50, 1);

	/* First set VDD_GPU to 1.0V, then enable the VDD_GPU regulator. */
	pmic_write_reg(bus, 0x06, 0x28, 1);

	/*
	 * First set +1.2V_GEN_AVDD to 1.2V, then enable the +1.2V_GEN_AVDD
	 * regulator.
	 */
	pmic_write_reg(bus, 0x12, 0x10, 1);

	/*
	 * Panel power GPIO O4. Set mode for GPIO4 (0x0c to 7), then set
	 * the value (register 0x20 bit 4)
	 */
	pmic_write_reg(bus, 0x0c, 0x07, 0);
	pmic_write_reg(bus, 0x20, 0x10, 1);
}
