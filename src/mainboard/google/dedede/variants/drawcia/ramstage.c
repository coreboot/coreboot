/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <boardid.h>
#include <device/device.h>
#include <drivers/usb/acpi/chip.h>
#include <fw_config.h>
#include <option.h>
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

#define TS_TYPE_AUTO_SELECT 0
#define TS_TYPE_ELAN0001 0
#define TS_TYPE_GTCH7503 1
#define TS_TYPE_GDIX0000 2
#define TS_TYPE_ELAN2513 3
#define TS_TYPE_WDHT0002 4

void variant_devtree_update(void)
{
	uint32_t board_version = board_id();
	struct device *ts_elan0001 = DEV_PTR(elan0001);
	struct device *ts_gtch7503 = DEV_PTR(gtch7503);
	struct device *ts_gdix0000 = DEV_PTR(gdix0000);
	struct device *ts_elan2513 = DEV_PTR(elan2513);
	struct device *ts_wdht0002 = DEV_PTR(wdht0002);

	/* Remove power IC after board version 0xb for Draw* and Oscino units */
	if (board_version > 0xb)
		ext_vr_update();
	/* Board version 0xa, 0xb and 0xf are for Drawper units */
	if (board_version != 0xa && board_version != 0xb && board_version != 0xf)
		update_lte_device_drawcia();

	/* Update touchscreen device */
	switch (get_uint_option("touchscreen", TS_TYPE_AUTO_SELECT)) {
	case TS_TYPE_ELAN0001:
		ts_gtch7503->enabled = 0;
		ts_gdix0000->enabled = 0;
		ts_elan2513->enabled = 0;
		ts_wdht0002->enabled = 0;
		break;
	case TS_TYPE_GTCH7503:
		ts_elan0001->enabled = 0;
		ts_gdix0000->enabled = 0;
		ts_elan2513->enabled = 0;
		ts_wdht0002->enabled = 0;
		break;
	case TS_TYPE_GDIX0000:
		ts_elan0001->enabled = 0;
		ts_gtch7503->enabled = 0;
		ts_elan2513->enabled = 0;
		ts_wdht0002->enabled = 0;
		break;
	case TS_TYPE_ELAN2513:
		ts_elan0001->enabled = 0;
		ts_gtch7503->enabled = 0;
		ts_gdix0000->enabled = 0;
		ts_wdht0002->enabled = 0;
		break;
	case TS_TYPE_WDHT0002:
		ts_elan0001->enabled = 0;
		ts_gtch7503->enabled = 0;
		ts_gdix0000->enabled = 0;
		ts_elan2513->enabled = 0;
		break;
	}
}
