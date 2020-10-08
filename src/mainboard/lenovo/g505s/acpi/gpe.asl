/* SPDX-License-Identifier: GPL-2.0-only */

Scope(\_GPE) {	/* Start Scope GPE */

	/* Legacy PM event */
	Method(_L08) {
		/* DBGO("\\_GPE\\_L08\n") */
	}

	/* Temp warning (TWarn) event */
	Method(_L09) {
		/* DBGO("\\_GPE\\_L09\n") */
		/* Notify (\_TZ.TZ00, 0x80) */
	}

	/* USB controller PME# */
	Method(_L0B) {
		Debug = "USB PME"
		/* Notify devices of wake event */
		Notify(\_SB.PCI0.UOH1, 0x02)
		Notify(\_SB.PCI0.UOH2, 0x02)
		Notify(\_SB.PCI0.UOH3, 0x02)
		Notify(\_SB.PCI0.UOH4, 0x02)
		Notify(\_SB.PCI0.XHC0, 0x02)
		Notify(\_SB.PCI0.UEH1, 0x02)
	}

	/* ExtEvent0 SCI event */
	Method(_L10) {
		/* DBGO("\\_GPE\\_L10\n") */
	}

	/* ExtEvent1 SCI event */
	Method(_L11) {
		/* DBGO("\\_GPE\\_L11\n") */
	}

	/* Lid switch opened or closed */
	Method(_L16) {
		Debug = "Lid status changed"
		/* Flip trigger polarity */
		LPOL = ~LPOL
		/* Notify lid object of status change */
		Notify(\_SB.LID, 0x80)
	}

	/* GPIO0 or GEvent8 event */
	Method(_L18) {
		Debug = "PCI bridge wake event"
		/* Notify PCI bridges of wake event */
		Notify(\_SB.PCI0.PBR4, 0x02)
		Notify(\_SB.PCI0.PBR5, 0x02)
	}

	/* Azalia SCI event */
	Method(_L1B) {
		/* DBGO("\\_GPE\\_L1B\n") */
		Notify(\_SB.PCI0.AZHD, 0x02) /* NOTIFY_DEVICE_WAKE */
	}
}	/* End Scope GPE */
