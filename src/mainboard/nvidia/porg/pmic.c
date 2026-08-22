/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <device/i2c_simple.h>
#include <reset.h>
#include <stdint.h>

#include "pmic.h"

#define MAX77620_I2C_ADDR 0x3c

static void pmic_write_reg(unsigned int bus, uint8_t chip, uint8_t reg, uint8_t val,
			   int delay)
{
	if (i2c_writeb(bus, chip, reg, val)) {
		printk(BIOS_ERR, "%s: reg = 0x%02X, value = 0x%02X failed!\n",
			__func__, reg, val);
		/* Reset the board on any PMIC write error */
		board_reset();
	} else {
		if (delay)
			udelay(500);
	}
}

void pmic_write_reg_77620(unsigned int bus, uint8_t reg, uint8_t val,
					int delay)
{
	pmic_write_reg(bus, MAX77620_I2C_ADDR, reg, val, delay);
}

void pmic_init(unsigned int bus)
{
	pmic_write_reg_77620(bus, MAX77620_GPIO5_REG, 0x09, 1);
	udelay(2000);
	printk(BIOS_DEBUG, "PMIC init done\n");
}
