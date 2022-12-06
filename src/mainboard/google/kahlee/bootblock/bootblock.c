/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <bootblock_common.h>
#include <gpio.h>
#include <soc/southbridge.h>
#include <amdblocks/lpc.h>
#include <variant/ec.h>
#include <variant/gpio.h>

void bootblock_mainboard_early_init(void)
{
	size_t num_gpios;
	const struct soc_amd_gpio *gpios;

	/* Enable the EC as soon as we have visibility */
	mainboard_ec_init();

	gpios = variant_wlan_rst_early_gpio_table(&num_gpios);
	gpio_configure_pads(gpios, num_gpios);

	gpios = variant_early_gpio_table(&num_gpios);
	gpio_configure_pads(gpios, num_gpios);
}

void bootblock_mainboard_init(void)
{
	/* Setup TPM decode before verstage */
	lpc_tpm_decode_spi();
}
