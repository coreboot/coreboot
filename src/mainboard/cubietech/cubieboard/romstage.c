/*
 * Placeholder for Cubieboard romstage
 *
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#include <console/console.h>
#include <cpu/allwinner/a10/clock.h>
#include <cpu/allwinner/a10/gpio.h>
#include <cpu/allwinner/a10/twi.h>
#define __SIMPLE_DEVICE__
#include <device/device.h>
#include <drivers/xpowers/axp209/axp209.h>
#include <drivers/xpowers/axp209/chip.h>


#define GPB_TWI0_FUNC			2
#define GPB_TWI0_PINS			((1 << 0) | (1 << 1))

#define AXP209_BUS	0

static enum cb_err cubieboard_setup_power(void)
{
	enum cb_err err;
	const struct device * pmu;
	const struct drivers_xpowers_axp209_config *cfg;

	/* Find the AXP209 in devicetree */
	pmu = dev_find_slot_on_smbus(AXP209_BUS, AXP209_I2C_ADDR);
	if (!pmu) {
		printk(BIOS_ERR, "AXP209 not found in devicetree.cb\n");
		return CB_ERR;
	}

	cfg = pmu->chip_info;

	/* Mux TWI0 pins */
	gpio_set_multipin_func(GPB, GPB_TWI0_PINS, GPB_TWI0_FUNC);
	/* Enable TWI0 */
	a1x_periph_clock_enable(A1X_CLKEN_TWI0);
	a1x_twi_init(AXP209_BUS, 400000);

	if ((err = axp209_init(AXP209_BUS)) != CB_SUCCESS) {
		printk(BIOS_ERR, "PMU initialization failed\n");
		return err;
	}

	if ((err = axp209_set_voltages(AXP209_BUS, cfg)) != CB_SUCCESS) {
		printk(BIOS_WARNING, "Power setup incomplete: "
				     "CPU may hang when increasing clock\n");
		return err;
	}

	printk(BIOS_SPEW, "VDD CPU (DCDC2): %imv\n", cfg->dcdc2_voltage_mv);
	printk(BIOS_SPEW, "VDD DLL (DCDC3): %imv\n", cfg->dcdc3_voltage_mv);
	printk(BIOS_SPEW, "AVCC    (LDO2) : %imv\n", cfg->ldo2_voltage_mv);
	printk(BIOS_SPEW, "CSI1-IO (LDO4) : %imv\n", cfg->ldo4_voltage_mv);
	printk(BIOS_SPEW, "(LDO3) : %imv\n", cfg->ldo3_voltage_mv);

	return CB_SUCCESS;
}

void main(void)
{
	console_init();
	printk(BIOS_INFO, "You have managed to succesfully load romstage.\n");

	/* Configure power rails */
	cubieboard_setup_power();

}
