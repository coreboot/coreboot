/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <boot/coreboot_tables.h>
#include <gpio.h>
#include <security/tpm/tis.h>

#include "board.h"

static const uint32_t wp_polarity = CONFIG(GRU_BASEBOARD_SCARLET) ?
				    ACTIVE_LOW : ACTIVE_HIGH;

int get_write_protect_state(void)
{
	return gpio_get(GPIO_WP) ^ !wp_polarity;
}

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
#if CONFIG(GRU_BASEBOARD_SCARLET)
		{GPIO_BACKLIGHT.raw, ACTIVE_HIGH, -1, "backlight"},
#endif
		{GPIO_EC_IN_RW.raw, ACTIVE_HIGH, -1, "EC in RW"},
		{GPIO_EC_IRQ.raw, ACTIVE_LOW, -1, "EC interrupt"},
		{GPIO_RESET.raw, ACTIVE_HIGH, -1, "reset"},
		{GPIO_SPK_PA_EN.raw, ACTIVE_HIGH, -1, "speaker enable"},
#if CONFIG(GRU_HAS_TPM2)
		{GPIO_TPM_IRQ.raw, ACTIVE_HIGH, -1, "TPM interrupt"},
#endif
	};

	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

void setup_chromeos_gpios(void)
{
	if (CONFIG(GRU_BASEBOARD_SCARLET))
		gpio_input(GPIO_WP);
	else
		gpio_input_pullup(GPIO_WP);
	gpio_input_pullup(GPIO_EC_IN_RW);
	gpio_input_pullup(GPIO_EC_IRQ);
}

#if CONFIG(GRU_HAS_TPM2)
int tis_plat_irq_status(void)
{
	return gpio_irq_status(GPIO_TPM_IRQ);
}
#endif

int get_ec_is_trusted(void)
{
	/* EC is trusted if not in RW. */
	return !gpio_get(GPIO_EC_IN_RW);
}
