/* SPDX-License-Identifier: GPL-2.0-only */

Scope(\_GPE) {	/* Start Scope GPE */

	/*  General event 3  */
	Method(_L03) {
		/* DBGO("\\_GPE\\_L00\n") */
	}

	/*  Legacy PM event  */
	Method(_L08) {
		/* DBGO("\\_GPE\\_L08\n") */
	}

	/*  Temp warning (TWarn) event  */
	Method(_L09) {
		/* DBGO("\\_GPE\\_L09\n") */
		/* Notify (\_TZ.TZ00, 0x80) */
	}

	/*  USB controller PME#  */
	Method(_L0B) {
		/* DBGO("\\_GPE\\_L0B\n") */
		Notify(\_SB.PCI0.UOH1, 0x02) /* NOTIFY_DEVICE_WAKE */
		Notify(\_SB.PCI0.UOH2, 0x02) /* NOTIFY_DEVICE_WAKE */
		Notify(\_SB.PCI0.UOH3, 0x02) /* NOTIFY_DEVICE_WAKE */
		Notify(\_SB.PCI0.UOH4, 0x02) /* NOTIFY_DEVICE_WAKE */
		Notify(\_SB.PCI0.UOH5, 0x02) /* NOTIFY_DEVICE_WAKE */
		Notify(\_SB.PCI0.UOH6, 0x02) /* NOTIFY_DEVICE_WAKE */
		Notify(\_SB.PCI0.UEH1, 0x02) /* NOTIFY_DEVICE_WAKE */
	}

	/*  ExtEvent0 SCI event  */
	Method(_L10) {
		/* DBGO("\\_GPE\\_L10\n") */
	}

	/*  ExtEvent1 SCI event  */
	Method(_L11) {
		/* DBGO("\\_GPE\\_L11\n") */
	}

	/*  GPIO0 or GEvent8 event  */
	Method(_L18) {
		/* DBGO("\\_GPE\\_L18\n") */
		Notify(\_SB.PCI0.PBR4, 0x02) /* NOTIFY_DEVICE_WAKE */
		Notify(\_SB.PCI0.PBR5, 0x02) /* NOTIFY_DEVICE_WAKE */
		Notify(\_SB.PCI0.PBR6, 0x02) /* NOTIFY_DEVICE_WAKE */
		Notify(\_SB.PCI0.PBR7, 0x02) /* NOTIFY_DEVICE_WAKE */
	}

	/*  Azalia SCI event  */
	Method(_L1B) {
		/* DBGO("\\_GPE\\_L1B\n") */
		Notify(\_SB.PCI0.AZHD, 0x02) /* NOTIFY_DEVICE_WAKE */
	}
}	/* End Scope GPE */
