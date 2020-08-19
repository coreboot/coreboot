/* SPDX-License-Identifier: GPL-2.0-only */

/*
Scope (_SB) {
	Device(PCI0) {
		Device(SATA) {
			Name(_ADR, 0x00110000)
			#include "sata.asl"
		}
	}
}
*/

Name(STTM, Buffer(20) {
	0x78, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00,
	0x78, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00,
	0x1f, 0x00, 0x00, 0x00
})

/* Start by clearing the PhyRdyChg bits */
Method(_INI) {
	\_GPE._L1F()
}

Device(PMRY)
{
	Name(_ADR, 0)
	Method(_GTM, 0x0, NotSerialized) {
		Return(STTM)
	}
	Method(_STM, 0x3, NotSerialized) {}

	Device(PMST) {
		Name(_ADR, 0)
		Method(_STA,0) {
			if (LGreater(P0IS,0)) {
				return (0x0F) /* sata is visible */
			}
			else {
				return  (0x00) /* sata is missing */
			}
		}
	}/* end of PMST */

	Device(PSLA)
	{
		Name(_ADR, 1)
		Method(_STA,0) {
			if (LGreater(P1IS,0)) {
				return (0x0F) /* sata is visible */
			}
			else {
				return (0x00) /* sata is missing */
			}
		}
	}	/* end of PSLA */
}   /* end of PMRY */

Device(SEDY)
{
	Name(_ADR, 1)		/* IDE Scondary Channel */
	Method(_GTM, 0x0, NotSerialized) {
		Return(STTM)
	}
	Method(_STM, 0x3, NotSerialized) {}

	Device(SMST)
	{
		Name(_ADR, 0)
		Method(_STA,0) {
			if (LGreater(P2IS,0)) {
				return (0x0F) /* sata is visible */
			}
			else {
				return (0x00) /* sata is missing */
			}
		}
	} /* end of SMST */

	Device(SSLA)
	{
		Name(_ADR, 1)
		Method(_STA,0) {
			if (LGreater(P3IS,0)) {
				return (0x0F) /* sata is visible */
			}
			else {
				return (0x00) /* sata is missing */
			}
		}
	} /* end of SSLA */
}   /* end of SEDY */

/* SATA Hot Plug Support */
Scope(\_GPE) {
	Method(_L1F,0x0,NotSerialized) {
		if (\_SB.P0PR) {
			if (LGreater(\_SB.P0IS,0)) {
				sleep(32)
			}
			Notify(\_SB.PCI0.STCR.PMRY.PMST, 0x01) /* NOTIFY_DEVICE_CHECK */
			store(one, \_SB.P0PR)
		}

		if (\_SB.P1PR) {
			if (LGreater(\_SB.P1IS,0)) {
				sleep(32)
			}
			Notify(\_SB.PCI0.STCR.PMRY.PSLA, 0x01) /* NOTIFY_DEVICE_CHECK */
			store(one, \_SB.P1PR)
		}

		if (\_SB.P2PR) {
			if (LGreater(\_SB.P2IS,0)) {
				sleep(32)
			}
			Notify(\_SB.PCI0.STCR.SEDY.SMST, 0x01) /* NOTIFY_DEVICE_CHECK */
			store(one, \_SB.P2PR)
		}

		if (\_SB.P3PR) {
			if (LGreater(\_SB.P3IS,0)) {
				sleep(32)
			}
			Notify(\_SB.PCI0.STCR.SEDY.SSLA, 0x01) /* NOTIFY_DEVICE_CHECK */
			store(one, \_SB.P3PR)
		}
	}
}
