/* SPDX-License-Identifier: GPL-2.0-only */

/* Use simple device model for this file even in ramstage */
#define __SIMPLE_DEVICE__

#include <arch/romstage.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/smm.h>
#include <device/pci_ops.h>
#include <cbmem.h>
#include <security/intel/txt/txt_platform.h>
#include <security/intel/txt/txt_register.h>
#include <types.h>

#include "haswell.h"

static uintptr_t northbridge_get_tseg_base(void)
{
	return ALIGN_DOWN(pci_read_config32(HOST_BRIDGE, TSEG), 1 * MiB);
}

static uintptr_t northbridge_get_tseg_limit(void)
{
	return ALIGN_DOWN(pci_read_config32(HOST_BRIDGE, BGSM), 1 * MiB);
}

union dpr_register txt_get_chipset_dpr(void)
{
	return (union dpr_register) { .raw = pci_read_config32(HOST_BRIDGE, DPR) };
}

/*
 * Return the topmost memory address below 4 GiB available for general
 * use, from software's view of memory. Do not confuse this with TOLUD,
 * which applies to the DRAM as viewed by the memory controller itself.
 */
static uintptr_t top_of_low_usable_memory(void)
{
	/*
	 * Base of DPR is top of usable DRAM below 4 GiB. However, DPR
	 * isn't always enabled. Unlike most memory map registers, the
	 * DPR register stores top of DPR instead of its base address.
	 * Unless binary-patched, Haswell MRC.bin does not enable DPR.
	 * Top of DPR is R/O, and mirrored from TSEG base by hardware.
	 */
	uintptr_t tolum = northbridge_get_tseg_base();

	const union dpr_register dpr = txt_get_chipset_dpr();

	/* Subtract DMA Protected Range size if enabled */
	if (dpr.epm)
		tolum -= dpr.size * MiB;

	return tolum;
}

uintptr_t cbmem_top_chipset(void)
{
	return top_of_low_usable_memory();
}

void smm_region(uintptr_t *start, size_t *size)
{
	*start = northbridge_get_tseg_base();
	*size = northbridge_get_tseg_limit();

	*size -= *start;
}

void fill_postcar_frame(struct postcar_frame *pcf)
{
	uintptr_t top_of_ram;

	/* Cache at least 8 MiB below the top of ram, and at most 8 MiB
	 * above top of the ram. This satisfies MTRR alignment requirement
	 * with different TSEG size configurations.
	 */
	top_of_ram = ALIGN_DOWN((uintptr_t)cbmem_top(), 8 * MiB);
	postcar_frame_add_mtrr(pcf, top_of_ram - 8 * MiB, 16 * MiB, MTRR_TYPE_WRBACK);
}
