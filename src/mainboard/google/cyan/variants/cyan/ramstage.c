/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/ramstage.h>

void board_silicon_USB2_override(SILICON_INIT_UPD *params)
{
	if (SocStepping() >= SocD0) {
		params->Usb2Port0PerPortTxiSet = 3;
		params->Usb2Port0IUsbTxEmphasisEn = 3;
		params->Usb2Port1PerPortTxiSet = 3;
		params->Usb2Port1IUsbTxEmphasisEn = 2;
		params->Usb2Port2PerPortTxiSet = 3;
		params->Usb2Port2IUsbTxEmphasisEn = 3;
		params->Usb2Port3PerPortTxiSet = 3;
		params->Usb2Port3IUsbTxEmphasisEn = 3;
		params->Usb2Port4PerPortTxiSet = 3;
		params->Usb2Port4IUsbTxEmphasisEn = 2;
	}
}
