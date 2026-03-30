/* SPDX-License-Identifier: GPL-2.0-only */

// Use simple device model for this file even in ramstage
#define __SIMPLE_DEVICE__

#include <arch/romstage.h>
#include <console/console.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/smm.h>
#include <cbmem.h>
#include <program_loading.h>
#include <cpu/intel/smm_reloc.h>
#include <types.h>

#include "gm965.h"

/*
 * Decode the Graphics Mode Select (GMS) field of the GGC register to the
 * corresponding stolen memory size in kilobytes.
 *
 * GM965 uses the same GMS encoding as i945 (bits [6:4] of D0:F0 offset 0x52):
 *   0 = 0 MB (no stolen memory / IGD disabled)
 *   1 = 1 MB
 *   2 = 4 MB
 *   3 = 8 MB
 *   4 = 16 MB
 *   5 = 32 MB
 *   6 = 48 MB
 *   7 = 64 MB
 *
 * Unlike GM45, GM965 has no separately configurable GTT stolen region (GGMS).
 */
u32 decode_igd_memory_size(const u32 gms)
{
	static const u16 ggc2uma[] = { 0, 1, 4, 8, 16, 32, 48, 64 };

	if (gms >= ARRAY_SIZE(ggc2uma))
		die("Bad Graphics Mode Select (GMS) setting.\n");

	return ggc2uma[gms] << 10;
}

/* Decodes TSEG region size in bytes. */
u32 decode_tseg_size(u8 esmramc)
{
	if (!(esmramc & 1))
		return 0;
	switch ((esmramc >> 1) & 3) {
	case 0:
		return 1 * MiB;
	case 1:
		return 2 * MiB;
	case 2:
		return 8 * MiB;
	case 3:
	default:
		die("Bad TSEG setting.\n");
	}
}

static uintptr_t northbridge_get_tseg_base(void)
{
	const pci_devfn_t dev = D0F0;
	const u8 esmramc = pci_read_config8(dev, D0F0_ESMRAMC);
	u32 tor;

	/*
	 * Like i945, gm965 has no separate configurable GTT stolen region.
	 * BSM (Base of Stolen Memory) in the IGD device directly encodes
	 * the base of all UMA-stolen memory, so use it when the IGD is
	 * enabled.  Fall back to TOLUD when the IGD is disabled.
	 */
	if (pci_read_config32(dev, D0F0_DEVEN) & DEVEN_D2F0)
		tor = pci_read_config32(D2F0, D2F0_BSM);
	else
		tor = (pci_read_config16(dev, D0F0_TOLUD) & 0xfff0) << 16;

	tor -= decode_tseg_size(esmramc);
	return tor;
}

static size_t northbridge_get_tseg_size(void)
{
	const u8 esmramc = pci_read_config8(PCI_DEV(0, 0, 0), D0F0_ESMRAMC);
	return decode_tseg_size(esmramc);
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
	/* Cache 8 MiB region below the top of RAM and 2 MiB above top of
	 * RAM to cover both cbmem as the TSEG region.
	 */
	const uintptr_t top_of_ram = cbmem_top();
	postcar_frame_add_mtrr(pcf, top_of_ram - 8*MiB, 8*MiB,
			       MTRR_TYPE_WRBACK);
	postcar_frame_add_mtrr(pcf, northbridge_get_tseg_base(),
			       northbridge_get_tseg_size(), MTRR_TYPE_WRBACK);
}
