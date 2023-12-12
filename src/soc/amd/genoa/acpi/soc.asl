/* SPDX-License-Identifier: GPL-2.0-only */

#include "globalnvs.asl"

Scope(\_SB) {
	/* global utility methods expected within the \_SB scope */
	#include "pci_int_defs.asl"

	#include "mmio.asl"
} /* End \_SB scope */
