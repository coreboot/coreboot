/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/ramstage.h>

void board_silicon_USB2_override(SILICON_INIT_UPD *params)
{
	if (SocStepping() >= SocD0) {

		//Follow Intel recommendation to set
		//BSW D-stepping PERPORTRXISET 2 (low strength)
		params->Usb2Port0PerPortPeTxiSet = 7;
		params->Usb2Port0PerPortTxiSet = 0;
		params->Usb2Port0IUsbTxEmphasisEn = 3;
		params->Usb2Port0PerPortTxPeHalf = 1;
		params->D0Usb2Port0PerPortRXISet = 2;

		params->Usb2Port1PerPortPeTxiSet = 7;
		params->Usb2Port1PerPortTxiSet = 0;
		params->Usb2Port1IUsbTxEmphasisEn = 3;
		params->Usb2Port1PerPortTxPeHalf = 1;
		params->D0Usb2Port1PerPortRXISet = 2;

		params->Usb2Port2PerPortPeTxiSet = 7;
		params->Usb2Port2PerPortTxiSet = 6;
		params->Usb2Port2IUsbTxEmphasisEn = 3;
		params->Usb2Port2PerPortTxPeHalf = 1;
		params->D0Usb2Port2PerPortRXISet = 2;

		params->Usb2Port3PerPortPeTxiSet = 7;
		params->Usb2Port3PerPortTxiSet = 6;
		params->Usb2Port3IUsbTxEmphasisEn = 3;
		params->Usb2Port3PerPortTxPeHalf = 1;
		params->D0Usb2Port3PerPortRXISet = 2;

		params->Usb2Port4PerPortPeTxiSet = 7;
		params->Usb2Port4PerPortTxiSet = 6;
		params->Usb2Port4IUsbTxEmphasisEn = 3;
		params->Usb2Port4PerPortTxPeHalf = 1;
		params->D0Usb2Port4PerPortRXISet = 2;
	}
}
