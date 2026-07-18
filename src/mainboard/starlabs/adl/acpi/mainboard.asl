/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB) {
	#include <mainboard/starlabs/common/acpi/sleep.asl>
}

#if CONFIG(BOARD_STARLABS_LITE_ADL)
External (\_SB.PCI0.LPCB.EC.VBTN.UPDK, MethodObj)

Scope (_GPE)
{
	Method (_E0F, 0, NotSerialized)
	{
		\_SB.PCI0.LPCB.EC.VBTN.UPDK()
	}
}
#endif
