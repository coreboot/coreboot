/* SPDX-License-Identifier: GPL-2.0-only */

#include <option.h>
#include <soc/ramstage.h>

void mainboard_silicon_init_params(FSP_S_CONFIG *supd)
{
	/*
	 * FSP defaults to pins that are used for LPC; given that
	 * coreboot only supports eSPI, set these pins accordingly.
	 */
	supd->PchSerialIoI2cSdaPinMux[0]	= 0x1947c404;	// GPP_H4
	supd->PchSerialIoI2cSclPinMux[0]	= 0x1947a405;	// GPP_H5

	if (get_uint_option("thunderbolt", 1) == 0)
		supd->UsbTcPortEn = 0;
}
