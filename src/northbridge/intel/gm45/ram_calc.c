/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 secunet Security Networks AG
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

// Use simple device model for this file even in ramstage
#define __SIMPLE_DEVICE__

#include <stdint.h>
#include <arch/cpu.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <console/console.h>
#include <cpu/intel/romstage.h>
#include <cpu/x86/mtrr.h>
#include <cbmem.h>
#include <program_loading.h>
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

	if (gms > ARRAY_SIZE(ggc2uma))
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

uintptr_t smm_region_start(void)
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

/* Depending of UMA and TSEG configuration, TSEG might start at any
 * 1 MiB alignment. As this may cause very greedy MTRR setup, push
 * CBMEM top downwards to 4 MiB boundary.
 */
void *cbmem_top(void)
{
	uintptr_t top_of_ram = ALIGN_DOWN(smm_region_start(), 4*MiB);
	return (void *) top_of_ram;
}

#define ROMSTAGE_RAM_STACK_SIZE 0x5000

/* platform_enter_postcar() determines the stack to use after
 * cache-as-ram is torn down as well as the MTRR settings to use,
 * and continues execution in postcar stage. */
void platform_enter_postcar(void)
{
	struct postcar_frame pcf;
	uintptr_t top_of_ram;

	if (postcar_frame_init(&pcf, ROMSTAGE_RAM_STACK_SIZE))
		die("Unable to initialize postcar frame.\n");

	/* Cache the ROM as WP just below 4GiB. */
	postcar_frame_add_romcache(&pcf, MTRR_TYPE_WRPROT);

	/* Cache RAM as WB from 0 -> CACHE_TMP_RAMTOP. */
	postcar_frame_add_mtrr(&pcf, 0, CACHE_TMP_RAMTOP, MTRR_TYPE_WRBACK);

	/* Cache a 8 MiB region below the top of ram and 8 MiB above top of
	 * ram to cover both cbmem as the TSEG region.
	 */
	top_of_ram = (uintptr_t)cbmem_top();
	postcar_frame_add_mtrr(&pcf, top_of_ram - 8*MiB, 16*MiB,
			MTRR_TYPE_WRBACK);

	run_postcar_phase(&pcf);

	/* We do not return here. */
}
