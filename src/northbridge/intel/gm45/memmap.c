/* SPDX-License-Identifier: GPL-2.0-only */

// Use simple device model for this file even in ramstage
#define __SIMPLE_DEVICE__

#include <arch/romstage.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <console/console.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/smm.h>
#include <cbmem.h>
#include <program_loading.h>
#include <cpu/intel/smm_reloc.h>
#include <types.h>

#include "gm45.h"

/*
 * Decodes used Graphics Mode Select (GMS) to kilobytes.
 * The options for 1M, 4M, 8M and 16M preallocated igd memory are
 * undocumented but are verified to work.
 */
u32 decode_igd_memory_size(const u32 gms)
{
	static const u16 ggc2uma[] = { 0, 1, 4, 8, 16, 32, 48, 64, 128, 256,
				       96, 160, 224, 352 };

	if (gms >= ARRAY_SIZE(ggc2uma))
		die("Bad Graphics Mode Select (GMS) setting.\n");

	return ggc2uma[gms] << 10;
}

/** Decodes used Graphics Stolen Memory (GSM) to kilobytes. */
u32 decode_igd_gtt_size(const u32 gsm)
{
	switch (gsm) {
	case 0:
		return 0 << 10;
	case 1:
		return 1 << 10;
	case 3:
	case 9:
		return 2 << 10;
	case 10:
		return 3 << 10;
	case 11:
		return 4 << 10;
	default:
		die("Bad Graphics Stolen Memory (GSM) setting.\n");
		return 0;
	}
}

/* Decodes TSEG region size to kilobytes. */
u32 decode_tseg_size(u8 esmramc)
{
	if (!(esmramc & 1))
		return 0;
	switch ((esmramc >> 1) & 3) {
	case 0:
		return 1 << 10;
	case 1:
		return 2 << 10;
	case 2:
		return 8 << 10;
	case 3:
	default:
		die("Bad TSEG setting.\n");
	}
}

static uintptr_t northbridge_get_tseg_base(void)
{
	const pci_devfn_t dev = PCI_DEV(0, 0, 0);

	u32 tor;

	/* Top of Lower Usable DRAM */
	tor = (pci_read_config16(dev, D0F0_TOLUD) & 0xfff0) << 16;

	/* Graphics memory comes next */
	const u32 ggc = pci_read_config16(dev, D0F0_GGC);
	const u8 esmramc = pci_read_config8(dev, D0F0_ESMRAMC);
	if (!(ggc & 2)) {
		/* Graphics memory */
		tor -= decode_igd_memory_size((ggc >> 4) & 0xf) << 10;
		/* GTT Graphics Stolen Memory Size (GGMS) */
		tor -= decode_igd_gtt_size((ggc >> 8) & 0xf) << 10;
	}
	/* TSEG size */
	tor -= decode_tseg_size(esmramc) << 10;
	return tor;
}

static size_t northbridge_get_tseg_size(void)
{
	const u8 esmramc = pci_read_config8(PCI_DEV(0, 0, 0), D0F0_ESMRAMC);
	return decode_tseg_size(esmramc) << 10;
}

/* Depending of UMA and TSEG configuration, TSEG might start at any
 * 1 MiB alignment. As this may cause very greedy MTRR setup, push
 * CBMEM top downwards to 4 MiB boundary.
 */
uintptr_t cbmem_top_chipset(void)
{
	return ALIGN_DOWN(northbridge_get_tseg_base(), 4*MiB);
}

void smm_region(uintptr_t *start, size_t *size)
{
	*start = northbridge_get_tseg_base();
	*size = northbridge_get_tseg_size();
}

void fill_postcar_frame(struct postcar_frame *pcf)
{
	uintptr_t top_of_ram;

	/* Cache 8 MiB region below the top of RAM and 2 MiB above top of
	 * RAM to cover both cbmem as the TSEG region.
	 */
	top_of_ram = (uintptr_t)cbmem_top();
	postcar_frame_add_mtrr(pcf, top_of_ram - 8*MiB, 8*MiB,
			       MTRR_TYPE_WRBACK);
	postcar_frame_add_mtrr(pcf, northbridge_get_tseg_base(),
			       northbridge_get_tseg_size(), MTRR_TYPE_WRBACK);

}
