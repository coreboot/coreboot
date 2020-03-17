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

Scope(\_GPE) {	/* Start Scope GPE */

	/*  General event 3  */
	Method(_L03) {
		/*  DBGO("\\_GPE\\_L03\n") */
		Notify(\_SB.PWRB, 0x02) /* SIO psin -> NOTIFY_DEVICE_WAKE */
	}

	/*  Power Button - PCIe Wake */
	Method(_L08) {
		/*  DBGO("\\_GPE\\_L08\n") */
		\_SB.SIO0.CPSI() /*  clear psin state in sio */
		Notify(\_SB.PCI0.PBR4, 0x02) /* NOTIFY_DEVICE_WAKE */
		Notify(\_SB.PCI0.PBR5, 0x02) /* NOTIFY_DEVICE_WAKE */
		Notify(\_SB.PCI0.PBR6, 0x02) /* NOTIFY_DEVICE_WAKE */
		Notify(\_SB.PCI0.PBR7, 0x02) /* NOTIFY_DEVICE_WAKE */
		Notify(\_SB.PCI0.PBR8, 0x02) /* NOTIFY_DEVICE_WAKE */
		Notify(\_SB.PCI0.AZHD, 0x02) /* NOTIFY_DEVICE_WAKE */
		Notify(\_SB.PWRB, 0x02) /* NOTIFY_DEVICE_WAKE */
	}
}	/* End Scope GPE */
