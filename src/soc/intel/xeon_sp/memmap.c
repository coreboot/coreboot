/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_ops.h>
#include <cpu/x86/smm.h>
#include <soc/pci_devs.h>

void smm_region(uintptr_t *start, size_t *size)
{
	uintptr_t tseg_base = pci_read_config32(VTD_DEV(0), VTD_TSEG_BASE_CSR);
	uintptr_t tseg_limit = pci_read_config32(VTD_DEV(0), VTD_TSEG_LIMIT_CSR);

	tseg_base = ALIGN_DOWN(tseg_base, 1 * MiB);
	tseg_limit = ALIGN_DOWN(tseg_limit, 1 * MiB);
	/* Only the upper [31:20] bits of an address are checked against
	 * VTD_TSEG_LIMIT_CSR[31:20] which must be below or equal, so this
	 * effectively means +1MiB for the limit.
	 */
	tseg_limit += 1 * MiB;

	*start = tseg_base;
	*size = tseg_limit - tseg_base;
}
