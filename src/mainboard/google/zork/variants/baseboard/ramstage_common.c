/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi_device.h>
#include <baseboard/variants.h>
#include <ec/google/chromeec/ec.h>
#include <soc/gpio.h>

void variant_audio_update(void)
{
	struct soc_amd_picasso_config *cfg = config_of_soc();
	uint32_t board_version;
	struct acpi_gpio *gpio = &cfg->dmic_select_gpio;

	if (!google_chromeec_cbi_get_board_version(&board_version) &&
	    (board_version >= CONFIG_VARIANT_MIN_BOARD_ID_V3_SCHEMATICS))
		return;

	if (CONFIG(BOARD_GOOGLE_BASEBOARD_TREMBYLE))
		gpio->pins[0] = GPIO_13;
	else
		gpio->pins[0] = GPIO_6;
}
