/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <variant/gpio.h>

/* If variants use other GPIO for BT_RESET
   Define it at variant/gpio.h */
#ifndef BT_RESET_GPIO
#define BT_RESET_GPIO	GPP_D4
#endif

/* Delay in milliseconds for BT_RESET */
#ifndef BT_RESET_DELAY_MS
#define BT_RESET_DELAY_MS	105
#endif

#define CNV_BTEN		GPP_VGPIO_0
#define CNV_BT_IF_SELECT	GPP_VGPIO_5

External (\_SB.PCI0.CTXS, MethodObj)
External (\_SB.PCI0.STXS, MethodObj)
External (\_SB.PCI0.GTXS, MethodObj)

Scope (\_SB.PCI0.XHCI.RHUB.HS10)
{
	PowerResource (CTTR, 0x05, 0x0000)
	{
		Method (_STA, 0, NotSerialized)  // _STA: Status
		{
			Return (One)
		}

		Method (_ON, 0, NotSerialized)  // _ON_: Power On
		{
		}

		Method (_OFF, 0, NotSerialized)  // _OFF: Power Off
		{
		}

		Method (_RST, 0, NotSerialized)  // _RST: Device Reset
		{
			\_SB.PCI0.CTXS(CNV_BTEN)
			\_SB.PCI0.CTXS(CNV_BT_IF_SELECT)
			\_SB.PCI0.CTXS(BT_RESET_GPIO)
			Sleep (BT_RESET_DELAY_MS)
			\_SB.PCI0.STXS(CNV_BTEN)
			\_SB.PCI0.STXS(CNV_BT_IF_SELECT)
			\_SB.PCI0.STXS(BT_RESET_GPIO)
			Sleep (BT_RESET_DELAY_MS)
		}
	}

	Method (_PRR, 0, NotSerialized)  // _PRR: Power Resource for Reset
	{
		Return (Package (0x01)
		{
			CTTR
		})
	}
}
