/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__

#include <cbmem.h>
#include <commonlib/helpers.h>
#include <arch/romstage.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <console/console.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/smm.h>
#include <northbridge/intel/x4x/x4x.h>
#include <program_loading.h>
#include <cpu/intel/smm_reloc.h>
#include <types.h>

/** Decodes used Graphics Mode Select (GMS) to kilobytes. */
u32 decode_igd_memory_size(const u32 gms)
{
	static const u16 ggc2uma[] = {0, 1, 4, 8, 16, 32, 48, 64, 128, 256, 96, 160, 224, 352};

	if (gms >= ARRAY_SIZE(ggc2uma))
		die("Bad Graphics Mode Select (GMS) setting.\n");

	return ggc2uma[gms] << 10;
}

/** Decodes used GTT Graphics Memory Size (GGMS) to kilobytes. */
u32 decode_igd_gtt_size(const u32 gsm)
{
	static const u8 ggc2gtt[] = {0, 1, 0, 2, 0, 0, 0, 0, 0, 2, 3, 4};

	if (gsm >= ARRAY_SIZE(ggc2gtt))
		die("Bad GTT Graphics Memory Size (GGMS) setting.\n");

	return ggc2gtt[gsm] << 10;
}

/** Decodes used TSEG size to bytes. */
u32 decode_tseg_size(const u32 esmramc)
{
	if (!(esmramc & 1))
		return 0;

	switch ((esmramc >> 1) & 3) {
	case 0:
		return 1 << 20;
	case 1:
		return 2 << 20;
	case 2:
		return 8 << 20;
	case 3:
	default:
		die("Bad TSEG setting.\n");
	}
}

static size_t northbridge_get_tseg_size(void)
{
	const u8 esmramc = pci_read_config8(HOST_BRIDGE, D0F0_ESMRAMC);
	return decode_tseg_size(esmramc);
}

static uintptr_t northbridge_get_tseg_base(void)
{
	return pci_read_config32(HOST_BRIDGE, D0F0_TSEG);
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
