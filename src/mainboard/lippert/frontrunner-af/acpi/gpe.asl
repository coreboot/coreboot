/* SPDX-License-Identifier: GPL-2.0-only */

	Scope(\_GPE) {	/* Start Scope GPE */

		/*  General event 3  */
		Method(_L03) {
			Notify(\_SB.PWRB, 0x02) /* NOTIFY_DEVICE_WAKE */
		}

		/*  Legacy PM event  */
		Method(_L08) {
		}

		/*  Temp warning (TWarn) event  */
		Method(_L09) {
		}

		/*  USB controller PME#  */
		Method(_L0B) {
			Notify(\_SB.PCI0.UOH1, 0x02) /* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PCI0.UOH2, 0x02) /* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PCI0.UOH3, 0x02) /* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PCI0.UOH4, 0x02) /* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PCI0.UOH5, 0x02) /* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PCI0.UOH6, 0x02) /* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PCI0.UEH1, 0x02) /* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PWRB, 0x02) /* NOTIFY_DEVICE_WAKE */
		}

		/*  ExtEvent0 SCI event  */
		Method(_L10) {
		}

		/*  ExtEvent1 SCI event  */
		Method(_L11) {
		}

		/*  GPIO0 or GEvent8 event  */
		Method(_L18) {
			Notify(\_SB.PCI0.PBR2, 0x02) /* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PCI0.PBR4, 0x02) /* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PCI0.PBR5, 0x02) /* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PCI0.PBR6, 0x02) /* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PCI0.PBR7, 0x02) /* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PWRB, 0x02) /* NOTIFY_DEVICE_WAKE */
		}

		/*  Azalia SCI event  */
		Method(_L1B) {
			Notify(\_SB.PCI0.AZHD, 0x02) /* NOTIFY_DEVICE_WAKE */
			Notify(\_SB.PWRB, 0x02) /* NOTIFY_DEVICE_WAKE */
		}
	}	/* End Scope GPE */
