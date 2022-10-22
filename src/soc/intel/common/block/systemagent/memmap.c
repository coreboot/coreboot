/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/romstage.h>
#include <cbmem.h>
#include <commonlib/bsd/helpers.h>
#include <console/console.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/smm.h>
#include <intelblocks/fast_spi.h>
#include <intelblocks/systemagent.h>
#include <types.h>

/*
 * Expected Host Memory Map (we don't know 100% and not all regions are present on all SoCs):
 *
 * +---------------------------+ TOUUD
 * |                           |
 * +---------------------------+ TOM (if mem > 4GB)
 * | CSME UMA (if mem > 4 GiB) |
 * +---------------------------+ TOUUD
 * |                           |
 * +---------------------------+ 4GiB
 * | PCI Address Space         |
 * +---------------------------+ TOM (if mem < 4GB)
 * | CSME UMA (if mem < 4 GiB) |
 * +---------------------------+ TOLUD (also maps into MC address space)
 * | iGD / DSM                 |
 * +---------------------------+ BDSM
 * | GTT / GSM                 |
 * +---------------------------+ TOLM
 * | TSEG                      |
 * +---------------------------+ TSEGMB
 * | DMA Protected Region      |
 * +---------------------------+ DPR
 * | PRM (C6DRAM/SGX)          |
 * +---------------------------+ PRMRR
 * | Probeless Trace           |
 * +---------------------------+ ME Stolen
 * | PTT                       |
 * +---------------------------+ TOLUM / top_of_ram / cbmem_top
 * | CBMEM Root                |
 * +---------------------------+
 * | FSP Reserved Memory       |
 * +---------------------------+
 * | various CBMEM entries     |
 * +---------------------------+ top_of_stack (8 byte aligned)
 * | stack (CBMEM entry)       |
 * +---------------------------+ FSP TOLUM
 * |                           |
 * +---------------------------+ 0
 */

void smm_region(uintptr_t *start, size_t *size)
{
	*start = sa_get_tseg_base();
	*size = sa_get_tseg_size();
}

void fill_postcar_frame(struct postcar_frame *pcf)
{
	/* FSP does not seem to bother w.r.t. alignment when asked to place cbmem_top() */
	uintptr_t top_of_ram = ALIGN_UP((uintptr_t)cbmem_top(), 8 * MiB);

	/*
	 * We need to make sure ramstage will be run cached. At this
	 * point exact location of ramstage in cbmem is not known.
	 * Instruct postcar to cache 16 megs below cbmem top which is
	 * a safe bet to cover ramstage.
	 */
	printk(BIOS_DEBUG, "top_of_ram = 0x%lx\n", top_of_ram);

	postcar_frame_add_mtrr(pcf, top_of_ram - 16 * MiB, 16 * MiB, MTRR_TYPE_WRBACK);

	/* Cache the TSEG region */
	postcar_enable_tseg_cache(pcf);

	/* Cache the extended BIOS region if it is supported */
	fast_spi_cache_ext_bios_postcar(pcf);
}
