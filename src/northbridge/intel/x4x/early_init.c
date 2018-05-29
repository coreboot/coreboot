/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 secunet Security Networks AG
 * Copyright (C) 2015 Damien Zammit <damien@zamaudio.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdint.h>
#include <arch/io.h>
#include "iomap.h"
#if IS_ENABLED(CONFIG_SOUTHBRIDGE_INTEL_I82801GX)
#include <southbridge/intel/i82801gx/i82801gx.h> /* DEFAULT_PMBASE */
#else
#include <southbridge/intel/i82801jx/i82801jx.h> /* DEFAULT_PMBASE */
#endif
#include <pc80/mc146818rtc.h>
#include "x4x.h"
#include <cbmem.h>
#include <console/console.h>
#include <halt.h>
#include <romstage_handoff.h>

void x4x_early_init(void)
{
	const pci_devfn_t d0f0 = PCI_DEV(0, 0, 0);

	/* Setup MCHBAR. */
	pci_write_config32(d0f0, D0F0_MCHBAR_LO, (uintptr_t)DEFAULT_MCHBAR | 1);

	/* Setup DMIBAR. */
	pci_write_config32(d0f0, D0F0_DMIBAR_LO, (uintptr_t)DEFAULT_DMIBAR | 1);

	/* Setup EPBAR. */
	pci_write_config32(d0f0, D0F0_EPBAR_LO, DEFAULT_EPBAR | 1);

	/* Setup PMBASE */
	pci_write_config32(PCI_DEV(0, 0x1f, 0), PMBASE, DEFAULT_PMBASE | 1);
	pci_write_config8(PCI_DEV(0, 0x1f, 0), ACPI_CNTL, 0x80);

	/* Setup HECIBAR */
	pci_write_config32(PCI_DEV(0, 3, 0), 0x10, DEFAULT_HECIBAR);

	/* Set C0000-FFFFF to access RAM on both reads and writes */
	pci_write_config8(d0f0, D0F0_PAM(0), 0x30);
	pci_write_config8(d0f0, D0F0_PAM(1), 0x33);
	pci_write_config8(d0f0, D0F0_PAM(2), 0x33);
	pci_write_config8(d0f0, D0F0_PAM(3), 0x33);
	pci_write_config8(d0f0, D0F0_PAM(4), 0x33);
	pci_write_config8(d0f0, D0F0_PAM(5), 0x33);
	pci_write_config8(d0f0, D0F0_PAM(6), 0x33);

	printk(BIOS_DEBUG, "Disabling Watchdog reboot...");
	RCBA32(GCS) = RCBA32(GCS) | (1 << 5);	/* No reset */
	outw(1 << 11, DEFAULT_PMBASE + 0x60 + 0x08);	/* halt timer */
	outw(1 <<  3, DEFAULT_PMBASE + 0x60 + 0x04);	/* clear timeout */
	outw(1 <<  1, DEFAULT_PMBASE + 0x60 + 0x06);	/* clear 2nd timeout */
	printk(BIOS_DEBUG, " done.\n");

	if (!(pci_read_config32(d0f0, D0F0_CAPID0 + 4) & (1 << (46 - 32)))) {
		/* Enable internal GFX */
		pci_write_config32(d0f0, D0F0_DEVEN, BOARD_DEVEN);

		/* Set preallocated IGD size from cmos */
		u8 gfxsize = 6; /* 6 for 64MiB, default if not set in cmos */
		get_option(&gfxsize, "gfx_uma_size");
		if (gfxsize > 12)
			gfxsize = 6;
		/* Need at least 4M for cbmem_top alignment */
		else if (gfxsize < 1)
			gfxsize = 1;
		/* Set GTT size to 2+2M */
		pci_write_config16(d0f0, D0F0_GGC, 0x0b00 | (gfxsize + 1) << 4);
	} else { /* Does not feature internal graphics */
		pci_write_config32(d0f0, D0F0_DEVEN, D0EN | D1EN | PEG1EN);
		pci_write_config16(d0f0, D0F0_GGC, (1 << 1));
	}
}

static void init_egress(void)
{
	u32 reg32;

	/* VC0: TC0 only */
	EPBAR8(0x14) = 1;
	EPBAR8(0x4) = 1;

	switch (MCHBAR32(0xc00) & 0x7) {
	case 0x0:
		/* FSB 1066 */
		EPBAR32(0x2c) = 0x0001a6db;
		break;
	case 0x2:
		/* FSB 800 */
		EPBAR32(0x2c) = 0x00014514;
		break;
	default:
	case 0x4:
		/* FSB 1333 */
		EPBAR32(0x2c) = 0x00022861;
		break;
	}
	EPBAR32(0x28) = 0x0a0a0a0a;
	EPBAR8(0xc) = (EPBAR8(0xc) & ~0xe) | 2;
	EPBAR32(0x1c) = (EPBAR32(0x1c) & ~0x7f0000) | 0x0a0000;
	MCHBAR8(0x3c) = MCHBAR8(0x3c) | 0x7;

	/* VC1: ID1, TC7 */
	reg32 = (EPBAR32(0x20) & ~(7 << 24)) | (1 << 24);
	reg32 = (reg32 & ~0xfe) | (1 << 7);
	EPBAR32(0x20) = reg32;

	/* Init VC1 port arbitration table */
	EPBAR32(0x100) = 0x001000001;
	EPBAR32(0x104) = 0x000040000;
	EPBAR32(0x108) = 0x000001000;
	EPBAR32(0x10c) = 0x000000040;
	EPBAR32(0x110) = 0x001000001;
	EPBAR32(0x114) = 0x000040000;
	EPBAR32(0x118) = 0x000001000;
	EPBAR32(0x11c) = 0x000000040;

	/* Load table */
	reg32 = EPBAR32(0x20) | (1 << 16);
	EPBAR32(0x20) = reg32;
	asm("nop");
	EPBAR32(0x20) = reg32;

	/* Wait for table load */
	while ((EPBAR8(0x26) & (1 << 0)) != 0)
		;

	/* VC1: enable */
	EPBAR32(0x20) |= 1 << 31;

	/* Wait for VC1 */
	while ((EPBAR8(0x26) & (1 << 1)) != 0)
		;

	printk(BIOS_DEBUG, "Done Egress Port\n");
}

static void init_dmi(void)
{
	u32 reg32;
	u16 reg16;

	/* Assume IGD present */

	/* Clear error status */
	DMIBAR32(0x1c4) = 0xffffffff;
	DMIBAR32(0x1d0) = 0xffffffff;

	/* VC0: TC0 only */
	DMIBAR8(DMIVC0RCTL) = 1;
	DMIBAR8(0x4) = 1;

	/* VC1: ID1, TC7 */
	reg32 = (DMIBAR32(DMIVC1RCTL) & ~(7 << 24)) | (1 << 24);
	reg32 = (reg32 & ~0xff) | 1 << 7;

	/* VC1: enable */
	reg32 |= 1 << 31;
	reg32 = (reg32 & ~(0x7 << 17)) | (0x4 << 17);

	DMIBAR32(DMIVC1RCTL) = reg32;

	/* Set up VCs in southbridge RCBA */
	RCBA8(0x3022) &= ~1;

	reg32 = (0x5 << 28) | (1 << 6); /* PCIe x4 */
	RCBA32(0x2020) = (RCBA32(0x2020) & ~((0xf << 28) | (0x7 << 6))) | reg32;

	/* Assign VC1 id 1 */
	RCBA32(0x20) = (RCBA32(0x20) & ~(0x7 << 24)) | (1 << 24);

	/* Map TC7 to VC1 */
	RCBA8(0x20) &= 1;
	RCBA8(0x20) |= 1 << 7;

	/* Map TC0 to VC0 */
	RCBA8(0x14) &= 1;

	/* Init DMI VC1 port arbitration table */
	RCBA32(0x20) &= 0xfff1ffff;
	RCBA32(0x20) |= 1 << 19;

	RCBA32(0x30) = 0x0000000f;
	RCBA32(0x34) = 0x000f0000;
	RCBA32(0x38) = 0;
	RCBA32(0x3c) = 0x000000f0;
	RCBA32(0x40) = 0x0f000000;
	RCBA32(0x44) = 0;
	RCBA32(0x48) = 0x0000f000;
	RCBA32(0x4c) = 0;
	RCBA32(0x50) = 0x0000000f;
	RCBA32(0x54) = 0x000f0000;
	RCBA32(0x58) = 0;
	RCBA32(0x5c) = 0x000000f0;
	RCBA32(0x60) = 0x0f000000;
	RCBA32(0x64) = 0;
	RCBA32(0x68) = 0x0000f000;
	RCBA32(0x6c) = 0;

	RCBA32(0x20) |= 1 << 16;

	/* Enable VC1 */
	RCBA32(0x20) |= 1 << 31;

	/* Wait for VC1 */
	while ((RCBA8(0x26) & (1 << 1)) != 0)
		;

	/* Wait for table load */
	while ((RCBA8(0x26) & (1 << 0)) != 0)
		;

	/* ASPM on DMI link */
	RCBA16(0x1a8) &= ~0x3;
	reg16 = RCBA16(0x1a8);
	RCBA32(0x2010) = (RCBA32(0x2010) & ~(0x3 << 10)) | (1 << 10);
	reg32 = RCBA32(0x2010);

	/* Set up VC1 max time */
	RCBA32(0x1c) = (RCBA32(0x1c) & ~0x7f0000) | 0x120000;

	while ((DMIBAR32(0x26) & (1 << 1)) != 0)
		;
	printk(BIOS_DEBUG, "Done DMI setup\n");

	/* ASPM on DMI */
	DMIBAR32(0x200) &= ~(0x3 << 26);
	DMIBAR16(0x210) = (DMIBAR16(0x210) & ~(0xff7)) | 0x101;
	DMIBAR32(0x88) &= ~0x3;
	DMIBAR32(0x88) |= 0x3;
	reg16 = DMIBAR16(0x88);
}

static void x4x_prepare_resume(int s3resume)
{
	romstage_handoff_init(s3resume);
}

void x4x_late_init(int s3resume)
{
	init_egress();
	init_dmi();
	x4x_prepare_resume(s3resume);
}
