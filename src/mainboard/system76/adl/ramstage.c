/* SPDX-License-Identifier: GPL-2.0-only */

#include <mainboard/gpio.h>
#include <soc/ramstage.h>
#include <smbios.h>

smbios_wakeup_type smbios_system_wakeup_type(void)
{
	return SMBIOS_WAKEUP_TYPE_POWER_SWITCH;
}

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	params->CnviRfResetPinMux = 0x194CE404; // GPP_F4
	params->CnviClkreqPinMux = 0x394CE605; // GPP_F5

	params->PchSerialIoI2cSdaPinMux[0] = 0x1947c404; // GPP_H4
	params->PchSerialIoI2cSclPinMux[0] = 0x1947a405; // GPP_H5
	params->PchSerialIoI2cSdaPinMux[1] = 0x1947c606; // GPP_H6
	params->PchSerialIoI2cSclPinMux[1] = 0x1947a607; // GPP_H7

	params->SataPortDevSlpPinMux[0] = 0x59673e0c; // GPP_H12
	params->SataPortDevSlpPinMux[1] = 0x5967400d; // GPP_H13

	params->SataPortsSolidStateDrive[1] = 1;
}

static void mainboard_init(void *chip_info)
{
	mainboard_configure_gpios();
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
};
