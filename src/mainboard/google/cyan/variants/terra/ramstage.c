/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <soc/ramstage.h>
#include <boardid.h>
#include <variant/onboard.h>

void board_silicon_USB2_override(SILICON_INIT_UPD *params)
{
	uint8_t	boardid = 0;
	uint8_t	projectid = 0;

	boardid = board_id();
	projectid = (boardid >> 3) & 0x01;

	if (projectid == TERRA2_PROJECT_ID) {
		params->Usb2Port0PerPortPeTxiSet = 7;
		params->Usb2Port0PerPortTxiSet = 6;
		params->Usb2Port0IUsbTxEmphasisEn = 3;
		params->Usb2Port0PerPortTxPeHalf = 1;
		params->Usb2Port1PerPortPeTxiSet = 7;
		params->Usb2Port1PerPortTxiSet = 6;
		params->Usb2Port1IUsbTxEmphasisEn = 3;
		params->Usb2Port1PerPortTxPeHalf = 1;
		params->Usb2Port2PerPortPeTxiSet = 7;
		params->Usb2Port2PerPortTxiSet = 6;
		params->Usb2Port2IUsbTxEmphasisEn = 3;
		params->Usb2Port2PerPortTxPeHalf = 1;
		params->Usb2Port3PerPortPeTxiSet = 7;
		params->Usb2Port3PerPortTxiSet = 6;
		params->Usb2Port3IUsbTxEmphasisEn = 3;
		params->Usb2Port3PerPortTxPeHalf = 1;
		params->Usb2Port4PerPortPeTxiSet = 7;
		params->Usb2Port4PerPortTxiSet = 6;
		params->Usb2Port4IUsbTxEmphasisEn = 3;
		params->Usb2Port4PerPortTxPeHalf = 1;
	} else {
		params->Usb2Port0PerPortPeTxiSet = 7;
		params->Usb2Port0PerPortTxiSet = 6;
		params->Usb2Port0IUsbTxEmphasisEn = 3;
		params->Usb2Port0PerPortTxPeHalf = 1;
		params->Usb2Port1PerPortPeTxiSet = 7;
		params->Usb2Port1PerPortTxiSet = 6;
		params->Usb2Port1IUsbTxEmphasisEn = 3;
		params->Usb2Port1PerPortTxPeHalf = 1;
		params->Usb2Port2PerPortPeTxiSet = 7;
		params->Usb2Port2PerPortTxiSet = 3;
		params->Usb2Port2IUsbTxEmphasisEn = 2;
		params->Usb2Port2PerPortTxPeHalf = 1;
		params->Usb2Port3PerPortPeTxiSet = 7;
		params->Usb2Port3PerPortTxiSet = 6;
		params->Usb2Port3IUsbTxEmphasisEn = 3;
		params->Usb2Port3PerPortTxPeHalf = 1;
		params->Usb2Port4PerPortPeTxiSet = 7;
		params->Usb2Port4PerPortTxiSet = 3;
		params->Usb2Port4IUsbTxEmphasisEn = 2;
		params->Usb2Port4PerPortTxPeHalf = 1;
	}
}
