/* SPDX-License-Identifier: GPL-2.0-only */

// Use simple device model for this file even in ramstage
#define __SIMPLE_DEVICE__

#include <arch/romstage.h>
#include <cbmem.h>
#include <console/console.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/smm.h>
#include <device/pci_ops.h>
#include <types.h>

#include "i945.h"

/* Decodes TSEG region size to bytes. */
u32 decode_tseg_size(const u8 esmramc)
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

static uintptr_t northbridge_get_tseg_base(void)
{
	uintptr_t tom;

	if (pci_read_config8(HOST_BRIDGE, DEVEN) & (DEVEN_D2F0 | DEVEN_D2F1))
		/* IGD enabled, get top of Memory from BSM register */
		tom = pci_read_config32(IGD_DEV, BSM);
	else
		tom = (pci_read_config8(HOST_BRIDGE, TOLUD) & 0xf8) << 24;

	/* subtract TSEG size */
	tom -= decode_tseg_size(pci_read_config8(HOST_BRIDGE, ESMRAMC));
	return tom;
}

static size_t northbridge_get_tseg_size(void)
{
	const u8 esmramc = pci_read_config8(HOST_BRIDGE, ESMRAMC);
	return decode_tseg_size(esmramc);
}

/*
 * Depending of UMA and TSEG configuration, TSEG might start at any
 * 1 MiB alignment. As this may cause very greedy MTRR setup, push
 * CBMEM top downwards to 4 MiB boundary.
 */
uintptr_t cbmem_top_chipset(void)
{
	return ALIGN_DOWN(northbridge_get_tseg_base(), 4*MiB);
}

/* Decodes used Graphics Mode Select (GMS) to kilobytes. */
u32 decode_igd_memory_size(const u32 gms)
{
	static const u16 ggc2uma[] = { 0, 1, 4, 8, 16, 32, 48, 64 };

	if (gms >= ARRAY_SIZE(ggc2uma))
		die("Bad Graphics Mode Select (GMS) setting.\n");

	return ggc2uma[gms] << 10;
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
	postcar_frame_add_mtrr(pcf, top_of_ram - 8*MiB, 8*MiB, MTRR_TYPE_WRBACK);
	postcar_frame_add_mtrr(pcf, northbridge_get_tseg_base(),
			       northbridge_get_tseg_size(), MTRR_TYPE_WRBACK);
}
