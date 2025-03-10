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
	params->PchSerialIoI2cSdaPinMux[3] = 0x1A45CA06; // GPP_H6
	params->PchSerialIoI2cSclPinMux[3] = 0x1A45AA07; // GPP_H7

	params->PchSerialIoI2cSdaPinMux[4] = 0x8A44CC0C; // GPP_E12
	params->PchSerialIoI2cSclPinMux[4] = 0x8A44AC0D; // GPP_E13

	params->PchSerialIoI2cSdaPinMux[5] = 0x8A46CE0D; // GPP_F13
	params->PchSerialIoI2cSclPinMux[5] = 0x8A46AE0C; // GPP_F12

	params->CnviRfResetPinMux = 0x194CE404; // GPP_F04
	params->CnviClkreqPinMux = 0x394CE605;  // GPP_F05

	params->LidStatus = dasharo_ec_get_lid_state();

	params->PortResetMessageEnable[1] = 1;
	params->PortResetMessageEnable[5] = 1;
}
