/* SPDX-License-Identifier: GPL-2.0-only */

// Use simple device model for this file even in ramstage
#define __SIMPLE_DEVICE__

#include <arch/romstage.h>
#include <cbmem.h>
#include <cpu/intel/smm_reloc.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/smm.h>
#include <device/pci_ops.h>
#include <program_loading.h>
#include <stdint.h>

#include "e7505.h"

#define HOST_BRIDGE PCI_DEV(0, 0, 0)

static uintptr_t top_of_low_ram(void)
{
	uintptr_t tolm;

	/* This is at 128 MiB boundary. */
	tolm = pci_read_config16(HOST_BRIDGE, TOLM) >> 11;
	tolm <<= 27;
	return tolm;
}

size_t northbridge_get_tseg_size(void)
{
	const uint8_t esmramc = pci_read_config8(HOST_BRIDGE, ESMRAMC);

	if (!(esmramc & T_EN))
		return 0;

	switch ((esmramc & TSEG_SZ_MASK) >> 1) {
	case 0:
		return 128 * KiB;
	case 1:
		return 256 * KiB;
	case 2:
		return 512 * KiB;
	case 3:
	default:
		return 1 * MiB;
	}
}

uintptr_t northbridge_get_tseg_base(void)
{
	uintptr_t tolm = top_of_low_ram();

	/* subtract TSEG size */
	tolm -= northbridge_get_tseg_size();
	return tolm;
}

void smm_region(uintptr_t *start, size_t *size)
{
	*start = northbridge_get_tseg_base();
	*size = northbridge_get_tseg_size();
}

uintptr_t cbmem_top_chipset(void)
{
	return northbridge_get_tseg_base();
}

void smm_open(void)
{
	/* Set D_OPEN */
	pci_write_config8(HOST_BRIDGE, SMRAMC, D_OPEN | G_SMRAME | C_BASE_SEG);
}

void smm_close(void)
{
	/* Clear D_OPEN */
	pci_write_config8(HOST_BRIDGE, SMRAMC, G_SMRAME | C_BASE_SEG);
}

void smm_lock(void)
{
	/*
	 * LOCK the SMM memory window and enable normal SMM.
	 * After running this function, only a full reset can
	 * make the SMM registers writable again.
	 */
	printk(BIOS_DEBUG, "Locking SMM.\n");

	pci_write_config8(HOST_BRIDGE, SMRAMC, D_LCK | G_SMRAME | C_BASE_SEG);
}

void fill_postcar_frame(struct postcar_frame *pcf)
{
	/*
	 * Choose to NOT set ROM as WP cacheable here.
	 * Timestamps indicate the CPU this northbridge code is
	 * connected to, performs better for memcpy() and un-lzma
	 * operations when source is left as UC.
	 */

	pcf->skip_common_mtrr = 1;

	/* Cache RAM as WB from 0 -> TOLM. */
	postcar_frame_add_mtrr(pcf, top_of_low_ram(), CACHE_TMP_RAMTOP, MTRR_TYPE_WRBACK);
}
