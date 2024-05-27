/* SPDX-License-Identifier: GPL-2.0-only */

#include <ec/dasharo/ec/acpi.h>
#include <mainboard/gpio.h>
#include <smbios.h>
#include <soc/ramstage.h>

const char *smbios_system_sku(void)
{
	return "Not Applicable";
}

smbios_enclosure_type smbios_mainboard_enclosure_type(void)
{
	return SMBIOS_ENCLOSURE_NOTEBOOK;
}

smbios_wakeup_type smbios_system_wakeup_type(void)
{
	return SMBIOS_WAKEUP_TYPE_POWER_SWITCH;
}

static void mainboard_init(void *chip_info)
{
	mainboard_configure_gpios();
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
};

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	// Enable reporting CPU C10 state over eSPI
	params->PchEspiHostC10ReportEnable = 1;

	// Pinmux configuration
	params->CnviRfResetPinMux = 0x194CE404; // GPP_F04
	params->CnviClkreqPinMux = 0x394CE605;  // GPP_F05

	params->LidStatus = dasharo_ec_get_lid_state();
}
