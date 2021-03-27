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
	epbar_write8(EPVC0RCTL, 1);
	epbar_write8(EPPVCCAP1, 1);

	switch (mchbar_read32(CLKCFG_MCHBAR) & CLKCFG_FSBCLK_MASK) {
	case 0x0:
		/* FSB 1066 */
		epbar_write32(EPVC1ITC, 0x0001a6db);
		break;
	case 0x2:
		/* FSB 800 */
		epbar_write32(EPVC1ITC, 0x00014514);
		break;
	default:
	case 0x4:
		/* FSB 1333 */
		epbar_write32(EPVC1ITC, 0x00022861);
		break;
	}
	epbar_write32(EPVC1MTS, 0x0a0a0a0a);
	epbar_clrsetbits8(EPPVCCTL, 7 << 1, 1 << 1);
	epbar_clrsetbits32(EPVC1RCAP, 0x7f << 16, 0x0a << 16);
	mchbar_setbits8(0x3c, 7);

	/* VC1: ID1, TC7 */
	reg32 = (epbar_read32(EPVC1RCTL) & ~(7 << 24)) | (1 << 24);
	reg32 = (reg32 & ~0xfe) | (1 << 7);
	epbar_write32(EPVC1RCTL, reg32);

	/* Init VC1 port arbitration table */
	epbar_write32(EP_PORTARB(0), 0x001000001);
	epbar_write32(EP_PORTARB(1), 0x000040000);
	epbar_write32(EP_PORTARB(2), 0x000001000);
	epbar_write32(EP_PORTARB(3), 0x000000040);
	epbar_write32(EP_PORTARB(4), 0x001000001);
	epbar_write32(EP_PORTARB(5), 0x000040000);
	epbar_write32(EP_PORTARB(6), 0x000001000);
	epbar_write32(EP_PORTARB(7), 0x000000040);

	/* Load table */
	reg32 = epbar_read32(EPVC1RCTL) | (1 << 16);
	epbar_write32(EPVC1RCTL, reg32);
	asm("nop");
	epbar_write32(EPVC1RCTL, reg32);

	/* Wait for table load */
	while ((epbar_read8(EPVC1RSTS) & (1 << 0)) != 0)
		;

	/* VC1: enable */
	epbar_setbits32(EPVC1RCTL, 1 << 31);

	/* Wait for VC1 */
	while ((epbar_read8(EPVC1RSTS) & (1 << 1)) != 0)
		;

	printk(BIOS_DEBUG, "Done Egress Port\n");
}

static void init_dmi(void)
{
	u32 reg32;

	/* Assume IGD present */

	/* Clear error status */
	dmibar_write32(DMIUESTS, 0xffffffff);
	dmibar_write32(DMICESTS, 0xffffffff);

	/* VC0: TC0 only */
	dmibar_write8(DMIVC0RCTL, 1);
	dmibar_write8(DMIPVCCAP1, 1);

	/* VC1: ID1, TC7 */
	reg32 = (dmibar_read32(DMIVC1RCTL) & ~(7 << 24)) | (1 << 24);
	reg32 = (reg32 & ~0xff) | 1 << 7;

	/* VC1: enable */
	reg32 |= 1 << 31;
	reg32 = (reg32 & ~(0x7 << 17)) | (0x4 << 17);

	dmibar_write32(DMIVC1RCTL, reg32);

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

	while ((dmibar_read32(DMIVC1RSTS) & VC1NP) != 0)
		;
	printk(BIOS_DEBUG, "Done DMI setup\n");

	/* ASPM on DMI */
	dmibar_clrbits32(0x200, 3 << 26);
	dmibar_clrsetbits16(0x210, 0xff7, 0x101);
	dmibar_clrbits32(DMILCTL, 3);
	dmibar_setbits32(DMILCTL, 3);
	/* FIXME: Do we need to read RCBA16(DMILCTL)? Probably not. */
	dmibar_read16(DMILCTL);
}

void x4x_late_init(void)
{
	init_egress();
	init_dmi();
}
