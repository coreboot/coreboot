/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__

#include <arch/romstage.h>
#include <device/pci_ops.h>
#include <cbmem.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/smm.h>
#include <types.h>

#include "ironlake.h"

static uintptr_t northbridge_get_tseg_base(void)
{
	/* Base of TSEG is top of usable DRAM */
	return pci_read_config32(PCI_DEV(0, 0, 0), TSEG);
}

static size_t northbridge_get_tseg_size(void)
{
	return CONFIG_SMM_TSEG_SIZE;
}

uintptr_t cbmem_top_chipset(void)
{
	return northbridge_get_tseg_base();
}

void smm_region(uintptr_t *start, size_t *size)
{
	*start = northbridge_get_tseg_base();
	*size = northbridge_get_tseg_size();
}

void fill_postcar_frame(struct postcar_frame *pcf)
{
	uintptr_t top_of_ram;

	/* Cache at least 8 MiB below the top of ram, and at most 8 MiB
	 * above top of the ram. This satisfies MTRR alignment requirement
	 * with different TSEG size configurations.
	 */
	top_of_ram = ALIGN_DOWN((uintptr_t)cbmem_top(), 8*MiB);
	postcar_frame_add_mtrr(pcf, top_of_ram - 8*MiB, 8*MiB, MTRR_TYPE_WRBACK);
	postcar_frame_add_mtrr(pcf, top_of_ram, 8*MiB, MTRR_TYPE_WRBACK);
}
