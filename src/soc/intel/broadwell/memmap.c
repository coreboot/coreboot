/* SPDX-License-Identifier: GPL-2.0-only */

/* Use simple device model for this file even in ramstage */
#define __SIMPLE_DEVICE__

#include <arch/romstage.h>
#include <cbmem.h>
#include <cpu/x86/smm.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <soc/pci_devs.h>
#include <soc/systemagent.h>
#include <stdint.h>

static uintptr_t dpr_region_start(void)
{
	/*
	 * Base of DPR is top of usable DRAM below 4GiB. The register has
	 * 1 MiB alignment and reports the TOP of the range, the base
	 * must be calculated from the size in MiB in bits 11:4.
	 */
	uintptr_t dpr = pci_read_config32(HOST_BRIDGE, DPR);
	uintptr_t tom = ALIGN_DOWN(dpr, 1 * MiB);

	/* Subtract DMA Protected Range size if enabled */
	if (dpr & DPR_EPM)
		tom -= (dpr & DPR_SIZE_MASK) << 16;

	return tom;
}

uintptr_t cbmem_top_chipset(void)
{
	return dpr_region_start();
}

void smm_region(uintptr_t *start, size_t *size)
{
	uintptr_t tseg = pci_read_config32(HOST_BRIDGE, TSEG);
	uintptr_t bgsm = pci_read_config32(HOST_BRIDGE, BGSM);

	tseg = ALIGN_DOWN(tseg, 1 * MiB);
	bgsm = ALIGN_DOWN(bgsm, 1 * MiB);
	*start = tseg;
	*size = bgsm - tseg;
}

void fill_postcar_frame(struct postcar_frame *pcf)
{
	uintptr_t top_of_ram;

	/* Cache at least 8 MiB below the top of ram, and at most 8 MiB
	 * above top of the ram. This satisfies MTRR alignment requirement
	 * with different TSEG size configurations.
	 */
	top_of_ram = ALIGN_DOWN((uintptr_t)cbmem_top(), 8*MiB);
	postcar_frame_add_mtrr(pcf, top_of_ram - 8*MiB, 16*MiB,
			MTRR_TYPE_WRBACK);
}
