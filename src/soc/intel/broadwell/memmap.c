/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__

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
	uintptr_t dpr = pci_read_config32(SA_DEV_ROOT, DPR);
	uintptr_t tom = ALIGN_DOWN(dpr, 1 * MiB);

	/* Subtract DMA Protected Range size if enabled */
	if (dpr & DPR_EPM)
		tom -= (dpr & DPR_SIZE_MASK) << 16;

	return tom;
}

void *cbmem_top_chipset(void)
{
	return (void *) dpr_region_start();
}

void smm_region(uintptr_t *start, size_t *size)
{
	uintptr_t tseg = pci_read_config32(PCI_DEV(0, 0, 0), TSEG);
	uintptr_t bgsm = pci_read_config32(PCI_DEV(0, 0, 0), BGSM);

	tseg = ALIGN_DOWN(tseg, 1 * MiB);
	bgsm = ALIGN_DOWN(bgsm, 1 * MiB);
	*start = tseg;
	*size = bgsm - tseg;
}
