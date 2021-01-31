/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <device/i2c_simple.h>
#include <reset.h>
#include <stdint.h>

#include "pmic.h"

enum {
	MAX77620_I2C_ADDR = 0x3c,
	MAX77621_CPU_I2C_ADDR = 0x1B,
	MAX77621_GPU_I2C_ADDR = 0x1C,
};

struct max77620_init_reg {
	u8 reg;
	u8 val;
	u8 delay;
};

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

static inline void pmic_write_reg_77621(unsigned int bus, uint8_t reg, uint8_t val,
					int delay)
{
	pmic_write_reg(bus, MAX77621_CPU_I2C_ADDR, reg, val, delay);
}

void pmic_init(unsigned int bus)
{
	/* MAX77620: Set SD0 to 1.0V - VDD_CORE */
	pmic_write_reg_77620(bus, MAX77620_SD0_REG, 0x20, 1);
	pmic_write_reg_77620(bus, MAX77620_VDVSSD0_REG, 0x20, 1);

	/* MAX77620: GPIO 0,1,2,5,6,7 = GPIO, 3,4 = alt mode */
	pmic_write_reg_77620(bus, MAX77620_AME_GPIO, 0x18, 1);

	/* MAX77620: Disable SD1 Remote Sense, Set SD1 for LPDDR4 to 1.125V */
	pmic_write_reg_77620(bus, MAX77620_CNFG2SD_REG, 0x04, 1);
	pmic_write_reg_77620(bus, MAX77620_SD1_REG, 0x2a, 1);

	/*
	 * MAX77620: Set LDO2 output to 1.8V. LDO2 is used as always-on
	 * reference for the droop alert circuit. Match this setting with what
	 * the kernel expects.
	 */
	pmic_write_reg_77620(bus, MAX77620_CNFG1_L2_REG, 0x14, 1);

	/* MAX77621: Set VOUT_REG to 1.0V - CPU VREG */
	pmic_write_reg_77621(bus, MAX77621_VOUT_REG, 0xBF, 1);

	/* MAX77621: Set VOUT_DVC_REG to 1.0V - CPU VREG DVC */
	pmic_write_reg_77621(bus, MAX77621_VOUT_DVC_REG, 0xBF, 1);

	/* MAX77621: Set CONTROL1 to 0x38 */
	pmic_write_reg_77621(bus, MAX77621_CONTROL1_REG, 0x38, 1);

	/* MAX77621: Set CONTROL2 to 0xD2 */
	pmic_write_reg_77621(bus, MAX77621_CONTROL2_REG, 0xD2, 1);

	/* MAX77620: Setup/Enable GPIO5 - EN_VDD_CPU */
	pmic_write_reg_77620(bus, MAX77620_GPIO5_REG, 0x09, 1);

	/* Required delay of 2msec */
	udelay(2000);

	printk(BIOS_DEBUG, "PMIC init done\n");
}
