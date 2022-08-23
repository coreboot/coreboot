/* SPDX-License-Identifier: GPL-2.0-only */

Device(PCI0) {
	/* Describe the AMD Northbridge */
	#include "northbridge.asl"

	/* Describe the AMD Fusion Controller Hub */
	#include "sb_pci0_fch.asl"
}

/* Describe PCI INT[A-H] for the Southbridge */
#include <soc/amd/common/acpi/pci_int.asl>

/* Describe the devices in the Southbridge */
#include "sb_fch.asl"

/* Add GPIO library */
#include <soc/amd/common/acpi/gpio_bank_lib.asl>

#include <soc/amd/common/acpi/dptc.asl>
