/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi_device.h>
#include <baseboard/variants.h>
#include <ec/google/chromeec/ec.h>
#include <soc/gpio.h>

void variant_audio_update(void)
{
	struct soc_amd_picasso_config *cfg = config_of_soc();
	struct acpi_gpio *gpio = &cfg->dmic_select_gpio;

	if (variant_uses_v3_schematics())
		return;

	if (CONFIG(BOARD_GOOGLE_BASEBOARD_TREMBYLE))
		gpio->pins[0] = GPIO_13;
	else
		gpio->pins[0] = GPIO_6;
}
