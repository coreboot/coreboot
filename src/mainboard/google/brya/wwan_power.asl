/* SPDX-License-Identifier: GPL-2.0-only */

#include <variant/gpio.h>

Method (MPTS, 1)
{
	\_SB.PCI0.CTXS(WWAN_PERST);
	Sleep(T1_OFF_MS)
	\_SB.PCI0.CTXS(WWAN_RST);
	Sleep(T2_OFF_MS)
	\_SB.PCI0.CTXS(WWAN_FCPO);
}
