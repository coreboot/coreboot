/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <device/i2c_simple.h>
#include <reset.h>
#include <stdint.h>

#include "pmic.h"

enum {
	MAX77620_I2C_ADDR = 0x3c
};

struct max77620_init_reg {
	u8 reg;
	u8 val;
	u8 delay;
};

static void pmic_write_reg(unsigned int bus, uint8_t reg, uint8_t val, int delay)
{
	if (i2c_writeb(bus, MAX77620_I2C_ADDR, reg, val)) {
		printk(BIOS_ERR, "%s: reg = 0x%02X, value = 0x%02X failed!\n",
			__func__, reg, val);
		/* Reset the board on any PMIC write error */
		board_reset();
	} else {
		if (delay)
			udelay(500);
	}
}

void pmic_init(unsigned int bus)
{
	/* Setup/Enable GPIO5 - VDD_CPU_REG_EN */
	pmic_write_reg(bus, MAX77620_GPIO5_REG, 0x09, 1);

	/* Setup/Enable GPIO1 - VDD_HDMI_5V0_BST_EN -- ??? */
	pmic_write_reg(bus, MAX77620_GPIO1_REG, 0x09, 1);

	/* GPIO 0,1,5,6,7 = GPIO, 2,3,4 = alt mode */
	pmic_write_reg(bus, MAX77620_AME_GPIO, 0x1c, 1);

	/* Disable SD1 Remote Sense, Set SD1 for LPDDR4 to 1.125v? */
	pmic_write_reg(bus, MAX77620_CNFG2SD_REG, 0x04, 1);

	pmic_write_reg(bus, MAX77620_SD1_REG, 0x2a, 1);

	/* CNFG1_L2 = 0xF2 for 3.3v, enabled */
	pmic_write_reg(bus, MAX77620_CNFG1_L2_REG, 0xf2, 1);

	/* CNFG1_L1 = 0xCA for 1.05v, enabled */
	pmic_write_reg(bus, MAX77620_CNFG1_L1_REG, 0xca, 1);

	printk(BIOS_DEBUG, "PMIC init done\n");
}
