/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/romstage.h>
#include <cbmem.h>
#include <console/console.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/smm.h>
#include <intelblocks/systemagent.h>

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
	uintptr_t top_of_ram;

	/*
	 * We need to make sure ramstage will be run cached. At this
	 * point exact location of ramstage in cbmem is not known.
	 * Instruct postcar to cache 16 megs under cbmem top which is
	 * a safe bet to cover ramstage.
	 */
	top_of_ram = (uintptr_t) cbmem_top();
	printk(BIOS_DEBUG, "top_of_ram = 0x%lx\n", top_of_ram);
	top_of_ram -= 16*MiB;
	postcar_frame_add_mtrr(pcf, top_of_ram, 16*MiB, MTRR_TYPE_WRBACK);

	/* Cache the TSEG region */
	postcar_enable_tseg_cache(pcf);
}
