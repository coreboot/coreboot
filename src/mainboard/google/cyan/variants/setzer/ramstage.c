/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <soc/ramstage.h>

void board_silicon_USB2_override(SILICON_INIT_UPD *params)
{
	if (SocStepping() >= SocD0) {
		/* Left External Port*/
		params->Usb2Port1PerPortPeTxiSet = 7;
		params->Usb2Port1PerPortTxiSet = 6;
		params->Usb2Port1IUsbTxEmphasisEn = 3;
		params->Usb2Port1PerPortTxPeHalf = 1;

		/* Right External Port */
		params->Usb2Port2PerPortPeTxiSet = 7;
		params->Usb2Port2PerPortTxiSet = 6;
		params->Usb2Port2IUsbTxEmphasisEn = 3;
		params->Usb2Port2PerPortTxPeHalf = 1;

		/* Camera*/
		params->Usb2Port3PerPortPeTxiSet = 7;
		params->Usb2Port3PerPortTxiSet = 6;
		params->Usb2Port3IUsbTxEmphasisEn = 3;
		params->Usb2Port3PerPortTxPeHalf = 1;

		/* BT */
		params->Usb2Port4PerPortPeTxiSet = 7;
		params->Usb2Port4PerPortTxiSet = 6;
		params->Usb2Port4IUsbTxEmphasisEn = 3;
		params->Usb2Port4PerPortTxPeHalf = 1;
	}
}
