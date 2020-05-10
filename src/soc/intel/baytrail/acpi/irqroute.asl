/* SPDX-License-Identifier: GPL-2.0-only */

// PCI Interrupt Routing
Method(_PRT)
{
	If (PICM) {
		Return (Package() {
			#undef PIC_MODE
			#include <soc/intel/baytrail/acpi/irq_helper.h>
			PCI_DEV_PIRQ_ROUTES
		})
	} Else {
		Return (Package() {
			#define PIC_MODE
			#include <soc/intel/baytrail/acpi/irq_helper.h>
			PCI_DEV_PIRQ_ROUTES
		})
	}
}
