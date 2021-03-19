/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <amdblocks/gpio_banks.h>
#include <amdblocks/i2c.h>
#include <baseboard/variants.h>
#include <console/console.h>
#include <security/vboot/vboot_common.h>
#include <soc/southbridge.h>

static void setup_gpio(void)
{
	const struct soc_amd_gpio *gpios;
	size_t num_gpios;

	printk(BIOS_DEBUG, "Setting GPIOs\n");
	gpios = variant_early_gpio_table(&num_gpios);
	program_gpios(gpios, num_gpios);
	printk(BIOS_DEBUG, "GPIOs setup\n");
}

static void setup_i2c(void)
{
	printk(BIOS_DEBUG, "Setting up i2c\n");
	i2c_soc_early_init();
	printk(BIOS_DEBUG, "i2c setup\n");
}

void verstage_mainboard_early_init(void)
{
	setup_gpio();
}

void verstage_mainboard_init(void)
{
	enable_aoac_devices();
	setup_i2c();
}
