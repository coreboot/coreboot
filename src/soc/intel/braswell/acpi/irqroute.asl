/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

/* PCI Interrupt Routing */
Method(_PRT)
{
	If (PICM) {
		Return (Package() {
			#undef PIC_MODE
			#include <soc/intel/braswell/acpi/irq_helper.h>
			PCI_DEV_PIRQ_ROUTES
		})
	} Else {
		Return (Package() {
			#define PIC_MODE
			#include <soc/intel/braswell/acpi/irq_helper.h>
			PCI_DEV_PIRQ_ROUTES
		})
	}
}
