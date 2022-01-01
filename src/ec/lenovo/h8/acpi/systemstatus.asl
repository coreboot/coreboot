/* SPDX-License-Identifier: GPL-2.0-only */


Scope (\_SI)
{
	Method(_SST, 1, NotSerialized)
	{
		If (Arg0 == 0) {
			/* Indicator off */

			/* power TLED off */
			\_SB.PCI0.LPCB.EC.TLED(0x00)
			/* suspend TLED off */
			\_SB.PCI0.LPCB.EC.TLED(0x07)
		}

		If (Arg0 == 1) {
			/* working state */

			/* power TLED on */
			\_SB.PCI0.LPCB.EC.TLED(0x80)
			/* suspend TLED off */
			\_SB.PCI0.LPCB.EC.TLED(0x07)
		}

		If (Arg0 == 2) {
			/* waking state */

			/* power LED on */
			\_SB.PCI0.LPCB.EC.TLED(0x80)
			/* suspend LED blinking */
			\_SB.PCI0.LPCB.EC.TLED(0xc7)
		}

		If (Arg0 == 3) {
			/* sleep state */

			/* power TLED pulsing */
			\_SB.PCI0.LPCB.EC.TLED(0xa0)
			/* suspend TLED on */
			\_SB.PCI0.LPCB.EC.TLED(0x87)
		}
	}
}
