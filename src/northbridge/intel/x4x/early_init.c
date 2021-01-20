/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <device/pci_ops.h>
#if CONFIG(SOUTHBRIDGE_INTEL_I82801GX)
#include <southbridge/intel/i82801gx/i82801gx.h> /* DEFAULT_PMBASE */
#else
#include <southbridge/intel/i82801jx/i82801jx.h> /* DEFAULT_PMBASE */
#endif
#include <option.h>
#include "x4x.h"
#include <console/console.h>
#include <romstage_handoff.h>

void x4x_early_init(void)
{
	/* Setup MCHBAR. */
	pci_write_config32(HOST_BRIDGE, D0F0_MCHBAR_LO, CONFIG_FIXED_MCHBAR_MMIO_BASE | 1);

	/* Setup DMIBAR. */
	pci_write_config32(HOST_BRIDGE, D0F0_DMIBAR_LO, CONFIG_FIXED_DMIBAR_MMIO_BASE | 1);

	/* Setup EPBAR. */
	pci_write_config32(HOST_BRIDGE, D0F0_EPBAR_LO, CONFIG_FIXED_EPBAR_MMIO_BASE | 1);

	/* Setup HECIBAR */
	pci_write_config32(PCI_DEV(0, 3, 0), 0x10, DEFAULT_HECIBAR);

	/* Set C0000-FFFFF to access RAM on both reads and writes */
	pci_write_config8(HOST_BRIDGE, D0F0_PAM(0), 0x30);
	pci_write_config8(HOST_BRIDGE, D0F0_PAM(1), 0x33);
	pci_write_config8(HOST_BRIDGE, D0F0_PAM(2), 0x33);
	pci_write_config8(HOST_BRIDGE, D0F0_PAM(3), 0x33);
	pci_write_config8(HOST_BRIDGE, D0F0_PAM(4), 0x33);
	pci_write_config8(HOST_BRIDGE, D0F0_PAM(5), 0x33);
	pci_write_config8(HOST_BRIDGE, D0F0_PAM(6), 0x33);

	if (!(pci_read_config32(HOST_BRIDGE, D0F0_CAPID0 + 4) & (1 << (46 - 32)))) {
		/* Enable internal GFX */
		pci_write_config32(HOST_BRIDGE, D0F0_DEVEN, BOARD_DEVEN);

		/* Set preallocated IGD size from CMOS */
		u8 gfxsize = 6; /* 6 for 64MiB, default if not set in CMOS */
		get_option(&gfxsize, "gfx_uma_size");
		if (gfxsize > 12)
			gfxsize = 6;
		/* Need at least 4M for cbmem_top alignment */
		else if (gfxsize < 1)
			gfxsize = 1;
		/* Set GTT size to 2+2M */
		pci_write_config16(HOST_BRIDGE, D0F0_GGC, 0x0b00 | (gfxsize + 1) << 4);
	} else { /* Does not feature internal graphics */
		pci_write_config32(HOST_BRIDGE, D0F0_DEVEN, D0EN | D1EN | PEG1EN);
		pci_write_config16(HOST_BRIDGE, D0F0_GGC, (1 << 1));
	}
}

static void init_egress(void)
{
	u32 reg32;

	/* VC0: TC0 only */
	EPBAR8(EPVC0RCTL) = 1;
	EPBAR8(EPPVCCAP1) = 1;

	switch (MCHBAR32(0xc00) & 0x7) {
	case 0x0:
		/* FSB 1066 */
		EPBAR32(EPVC1ITC) = 0x0001a6db;
		break;
	case 0x2:
		/* FSB 800 */
		EPBAR32(EPVC1ITC) = 0x00014514;
		break;
	default:
	case 0x4:
		/* FSB 1333 */
		EPBAR32(EPVC1ITC) = 0x00022861;
		break;
	}
	EPBAR32(EPVC1MTS) = 0x0a0a0a0a;
	EPBAR8(EPPVCCTL) = (EPBAR8(EPPVCCTL) & ~0xe) | 2;
	EPBAR32(EPVC1RCAP) = (EPBAR32(EPVC1RCAP) & ~0x7f0000) | 0x0a0000;
	MCHBAR8(0x3c) = MCHBAR8(0x3c) | 0x7;

	/* VC1: ID1, TC7 */
	reg32 = (EPBAR32(EPVC1RCTL) & ~(7 << 24)) | (1 << 24);
	reg32 = (reg32 & ~0xfe) | (1 << 7);
	EPBAR32(EPVC1RCTL) = reg32;

	/* Init VC1 port arbitration table */
	EPBAR32(EP_PORTARB(0)) = 0x001000001;
	EPBAR32(EP_PORTARB(1)) = 0x000040000;
	EPBAR32(EP_PORTARB(2)) = 0x000001000;
	EPBAR32(EP_PORTARB(3)) = 0x000000040;
	EPBAR32(EP_PORTARB(4)) = 0x001000001;
	EPBAR32(EP_PORTARB(5)) = 0x000040000;
	EPBAR32(EP_PORTARB(6)) = 0x000001000;
	EPBAR32(EP_PORTARB(7)) = 0x000000040;

	/* Load table */
	reg32 = EPBAR32(EPVC1RCTL) | (1 << 16);
	EPBAR32(EPVC1RCTL) = reg32;
	asm("nop");
	EPBAR32(EPVC1RCTL) = reg32;

	/* Wait for table load */
	while ((EPBAR8(EPVC1RSTS) & (1 << 0)) != 0)
		;

	/* VC1: enable */
	EPBAR32(EPVC1RCTL) |= 1 << 31;

	/* Wait for VC1 */
	while ((EPBAR8(EPVC1RSTS) & (1 << 1)) != 0)
		;

	printk(BIOS_DEBUG, "Done Egress Port\n");
}

static void init_dmi(void)
{
	u32 reg32;

	/* Assume IGD present */

	/* Clear error status */
	DMIBAR32(DMIUESTS) = 0xffffffff;
	DMIBAR32(DMICESTS) = 0xffffffff;

	/* VC0: TC0 only */
	DMIBAR8(DMIVC0RCTL) = 1;
	DMIBAR8(DMIPVCCAP1) = 1;

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
	/* FIXME: Do we need to read RCBA16(0x1a8)? */
	RCBA16(0x1a8);
	RCBA32(0x2010) = (RCBA32(0x2010) & ~(0x3 << 10)) | (1 << 10);
	/* FIXME: Do we need to read RCBA32(0x2010)? */
	RCBA32(0x2010);

	/* Set up VC1 max time */
	RCBA32(0x1c) = (RCBA32(0x1c) & ~0x7f0000) | 0x120000;

	while ((DMIBAR32(DMIVC1RSTS) & VC1NP) != 0)
		;
	printk(BIOS_DEBUG, "Done DMI setup\n");

	/* ASPM on DMI */
	DMIBAR32(0x200) &= ~(0x3 << 26);
	DMIBAR16(0x210) = (DMIBAR16(0x210) & ~(0xff7)) | 0x101;
	DMIBAR32(DMILCTL) &= ~0x3;
	DMIBAR32(DMILCTL) |= 0x3;
	/* FIXME: Do we need to read RCBA16(DMILCTL)? Probably not. */
	DMIBAR16(DMILCTL);
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
