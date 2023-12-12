/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/amd/common/acpi/pci_root.asl>
#include "globalnvs.asl"

Scope(\_SB) {
	/* global utility methods expected within the \_SB scope */
	#include "pci_int_defs.asl"

	#include "mmio.asl"

	ROOT_BRIDGE(S0B0)
	ROOT_BRIDGE(S0B1)
	ROOT_BRIDGE(S0B2)
	ROOT_BRIDGE(S0B3)
} /* End \_SB scope */
