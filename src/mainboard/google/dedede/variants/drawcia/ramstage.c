/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <boardid.h>
#include <device/device.h>
#include <drivers/usb/acpi/chip.h>
#include <fw_config.h>
#include <soc/soc_chip.h>
#include <static.h>

static void ext_vr_update(void)
{
	struct soc_intel_jasperlake_config *cfg = config_of_soc();
	if (fw_config_probe(FW_CONFIG(EXT_VR, EXT_VR_ABSENT)))
		cfg->disable_external_bypass_vr = 1;
}

static void update_lte_device_drawcia(void)
{
	struct device *lte_usb2 = DEV_PTR(lte_usb2);
	struct drivers_usb_acpi_config *config;
	struct acpi_gpio lte_reset_gpio = ACPI_GPIO_OUTPUT_ACTIVE_LOW(GPP_H0);

	if (lte_usb2 == NULL)
		return;

	config = config_of(lte_usb2);
	config->reset_gpio = lte_reset_gpio;
}

void variant_devtree_update(void)
{
	uint32_t board_version = board_id();

	/* Remove power IC after board version 0xb for Draw* and Oscino units */
	if (board_version > 0xb)
		ext_vr_update();
	/* Board version 0xa, 0xb and 0xf are for Drawper units */
	if (board_version != 0xa && board_version != 0xb && board_version != 0xf)
		update_lte_device_drawcia();
}
