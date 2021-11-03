/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <bootmode.h>
#include <boot/coreboot_tables.h>
#include <gpio.h>
#include <vendorcode/google/chromeos/chromeos.h>

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		{-1, ACTIVE_HIGH, get_lid_switch(), "lid"},
		{-1, ACTIVE_HIGH, 0, "power"},
		{-1, ACTIVE_HIGH, gfx_get_init_done(), "oprom"},
		{GPIO_EC_IN_RW, ACTIVE_HIGH, gpio_get(GPIO_EC_IN_RW), "EC in RW"},
	};
	if (CONFIG(BOARD_INTEL_ADLRVP_P_EXT_EC) || CONFIG(BOARD_INTEL_ADLRVP_M_EXT_EC))
		lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
	else
		lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios) - 1);
}

#if !CONFIG(EC_GOOGLE_CHROMEEC_SWITCHES)
int get_lid_switch(void)
{
	/* Lid always open */
	return 1;
}

int get_recovery_mode_switch(void)
{
	return 0;
}
#endif /*!CONFIG(EC_GOOGLE_CHROMEEC_SWITCHES) */

int get_write_protect_state(void)
{
	/* No write protect */
	return 0;
}

void mainboard_chromeos_acpi_generate(void)
{
	const struct cros_gpio *gpios;
	size_t num;

	gpios = variant_cros_gpios(&num);
	chromeos_acpi_gpio_generate(gpios, num);
}
