/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <soc/ramstage.h>

void board_silicon_USB2_override(SILICON_INIT_UPD *params)
{
	if (SocStepping() >= SocD0) {
		//D-Stepping
		//USB2[1] right external port
		params->Usb2Port1PerPortPeTxiSet = 7;
		params->Usb2Port1PerPortTxiSet = 3;
		params->Usb2Port1IUsbTxEmphasisEn = 2;
		params->Usb2Port1PerPortTxPeHalf = 1;

		//USB2[2] left external port
		params->Usb2Port2PerPortPeTxiSet = 7;
		params->Usb2Port2PerPortTxiSet = 0;
		params->Usb2Port2IUsbTxEmphasisEn = 2;
		params->Usb2Port2PerPortTxPeHalf = 1;

		//USB2[3] CCD
		params->Usb2Port3PerPortPeTxiSet = 7;
		params->Usb2Port3PerPortTxiSet = 0;
		params->Usb2Port3IUsbTxEmphasisEn = 2;
		params->Usb2Port3PerPortTxPeHalf = 1;
	}
}
