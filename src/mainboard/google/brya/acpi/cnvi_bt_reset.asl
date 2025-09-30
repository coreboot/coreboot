/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <variant/gpio.h>

/* If variants use other GPIO for BT_RESET
   Define it at variant/gpio.h */
#ifndef BT_RESET_GPIO
#define BT_RESET_GPIO	GPP_D4
#endif

#define CNV_BTEN		GPP_VGPIO_0
#define CNV_BT_IF_SELECT	GPP_VGPIO_5

External (\_SB.PCI0.CTXS, MethodObj)
External (\_SB.PCI0.STXS, MethodObj)

Scope (\_SB.PCI0.XHCI.RHUB.HS10)
{
	PowerResource (CTTR, 0x00, 0x0000)
	{
		Method (_STA, 0, NotSerialized)  // _STA: Status
		{
			Return (One)
		}

		Method (_ON, 0, NotSerialized)  // _ON_: Power On
		{
			\_SB.PCI0.STXS(CNV_BTEN);
			\_SB.PCI0.STXS(CNV_BT_IF_SELECT);
			\_SB.PCI0.STXS(BT_RESET_GPIO);
			Sleep (105)
		}

		Method (_OFF, 0, NotSerialized)  // _OFF: Power Off
		{
			\_SB.PCI0.CTXS(CNV_BTEN);
			\_SB.PCI0.CTXS(CNV_BT_IF_SELECT);
			\_SB.PCI0.CTXS(BT_RESET_GPIO);
			Sleep (105)
		}

	}

	Name (_PRR, Package (0x01)  // _PRR: Power Resource for Reset
	{
		CTTR
	})

	Name (_PR0, Package (0x01)  // _PR0: Power Resources for D0
	{
		CTTR
	})
}
