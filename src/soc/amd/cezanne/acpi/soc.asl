/* SPDX-License-Identifier: GPL-2.0-only */

Scope(\_SB) {
	/* global utility methods expected within the \_SB scope */
	#include <arch/x86/acpi/globutil.asl>

	#include <soc/amd/common/acpi/gpio_bank_lib.asl>
} /* End \_SB scope */
