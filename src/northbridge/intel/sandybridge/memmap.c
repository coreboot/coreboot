/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__

#include <arch/romstage.h>
#include <device/pci_ops.h>
#include <cbmem.h>
#include <cpu/intel/smm_reloc.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/smm.h>
#include <program_loading.h>
#include "sandybridge.h"
#include <security/intel/txt/txt_platform.h>
#include <stddef.h>
#include <stdint.h>

static uintptr_t northbridge_get_tseg_base(void)
{
	/* TSEG has 1 MiB granularity, and bit 0 is a lock */
	return ALIGN_DOWN(pci_read_config32(HOST_BRIDGE, TSEGMB), 1 * MiB);
}

static size_t northbridge_get_tseg_size(void)
{
	return CONFIG_SMM_TSEG_SIZE;
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
	 * may not always be enabled. Unlike most memory map registers,
	 * the DPR register stores top of DPR instead of its base address.
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
	*size  = northbridge_get_tseg_size();
}

void fill_postcar_frame(struct postcar_frame *pcf)
{
	uintptr_t top_of_ram = (uintptr_t)cbmem_top();

	/*
	 * Cache 8MiB below the top of ram. On sandybridge systems the top of
	 * RAM under 4GiB is the start of the TSEG region. It is required to
	 * be 8MiB aligned. Set this area as cacheable so it can be used later
	 * for ramstage before setting up the entire RAM as cacheable.
	 */
	postcar_frame_add_mtrr(pcf, top_of_ram - 8 * MiB, 8 * MiB, MTRR_TYPE_WRBACK);

	/*
	 * Cache 8MiB at the top of ram. Top of RAM on sandybridge systems
	 * is where the TSEG region resides. However, it is not restricted
	 * to SMM mode until SMM has been relocated. By setting the region
	 * to cacheable it provides faster access when relocating the SMM
	 * handler as well as using the TSEG region for other purposes.
	 */
	postcar_frame_add_mtrr(pcf, top_of_ram, 8 * MiB, MTRR_TYPE_WRBACK);
}
