/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Intel GM965 (Crestline) Northbridge - DMI/PCIe Link Initialization
 * Reverse-engineered from ThinkPad X61 Phoenix BIOS
 *
 * BIOS functions:
 *   FFF897DB (pci_init_dmibar) - DMI link setup from PCI_INIT region
 *   FFF8BF7A (bar_epbar_dmibar_setup) - EPBAR/DMIBAR register programming
 *   bioscode_7.rom - DMI/PCIe link init module
 *
 * This file handles the northbridge (MCH) side of DMI link setup.
 * The southbridge side is handled by i82801hx_dmi_setup() in the ICH8-M driver.
 *
 * Reference: coreboot GM45 pcie.c (init_egress, init_dmi, setup_rcrb)
 */

#include <device/pci_ops.h>
#include <northbridge/intel/gm965/gm965.h>

/* BAR base addresses from Kconfig */
#define MCHBAR_BASE   CONFIG_FIXED_MCHBAR_MMIO_BASE
#define DMIBAR_BASE   CONFIG_FIXED_DMIBAR_MMIO_BASE
#define EPBAR_BASE    CONFIG_FIXED_EPBAR_MMIO_BASE
#define RCBA_BASE     CONFIG_FIXED_RCBA_MMIO_BASE

/* ================================================================== */
/* Egress Port Setup                                                  */
/* ================================================================== */

/*
 * init_egress() - Configure the Egress Port (EPBAR)
 *
 * Sets up virtual channels on the EPBAR for DMI traffic routing.
 * VC0 handles normal traffic (TC0), VC1 handles isochronous (TC7).
 *
 * coreboot equivalent: init_egress() in pcie.c
 */
static void init_egress(void)
{
	printk(BIOS_DEBUG, "%s\n", __func__);
	/* VC0: TC0 only */
	epbar_clrbits8(EPVC0RCTL, ~1);
	epbar_clrsetbits8(EPPVCCAP1, 7, 1);

	/* VC1 isochronous: matching time slots */
	epbar_write32(EPVC1MTS, 0x0a0a0a0a);
	epbar_clrsetbits32(EPVC1RCAP, 127 << 16, 0x0a << 16);

	/* VC1: ID=1, TC7 */
	epbar_clrsetbits32(EPVC1RCTL, 7 << 24, 1 << 24);
	epbar_clrsetbits8(EPVC1RCTL, ~1, 1 << 7);

	/* VC1 ARB table: setup and enable */
	epbar_write32(EP_PORTARB(0), 0x55555555);
	epbar_write32(EP_PORTARB(1), 0x55555555);
	epbar_write32(EP_PORTARB(2), 0x55555555);
	epbar_write32(EP_PORTARB(3), 0x55555555);
	epbar_write32(EP_PORTARB(4), 0x55555555);
	epbar_write32(EP_PORTARB(5), 0x55555555);
	epbar_write32(EP_PORTARB(6), 0x55555555);
	epbar_write32(EP_PORTARB(7), 0x00005555);

	/* Load ARB table */
	epbar_setbits32(EPVC1RCTL, 1 << 16);

	/* Wait for ARB table load */
	while (epbar_read8(EPVC1RSTS) & 1)
		;

	/* Enable VC1 */
	epbar_setbits32(EPVC1RCTL, 1 << 31);

	/* Wait for VC1 negotiation */
	while (epbar_read8(EPVC1RSTS) & 2)
		;
}

/* ================================================================== */
/* DMI Link Setup (Northbridge Side)                                  */
/* ================================================================== */

/*
 * init_dmi() - Configure DMI link (MCH side)
 *
 * Sets up virtual channels, link parameters, and write-once registers
 * on the DMIBAR for the MCH-to-ICH link.
 *
 * coreboot equivalent: init_dmi() in pcie.c
 */
static void init_dmi(void)
{
	printk(BIOS_DEBUG, "%s\n", __func__);
	/* VC0: TC0 only */
	dmibar_clrbits8(DMIVC0RCTL, ~1);
	dmibar_clrsetbits8(DMIPVCCAP1, 7, 1);

	/* VC1: ID=1, TC7 */
	dmibar_clrsetbits32(DMIVC1RCTL, 7 << 24, 1 << 24);
	dmibar_clrsetbits8(DMIVC1RCTL, ~1, 1 << 7);

	/* Enable VC1 */
	dmibar_setbits32(DMIVC1RCTL, 1 << 31);

	/* Wait for VC1 negotiation */
	while (dmibar_read8(DMIVC1RSTS) & VC1NP)
		;

	/*
	 * Additional DMI configuration from BIOS.
	 * These magic register writes were observed in bioscode_7.rom
	 * and match coreboot GM45 init_dmi() exactly.
	 */
	dmibar_setbits32(0x200, 3 << 13);
	dmibar_clrbits32(0x200, 1 << 21);
	dmibar_clrsetbits32(0x200, 3 << 26, 2 << 26);
	dmibar_write32(0x2c, 0x86000040);

	/* Link flow control credits and configuration */
	dmibar_setbits32(0xfc, 1 << 0);
	dmibar_setbits32(0xfc, 1 << 1);
	dmibar_setbits32(0xfc, 1 << 4);

	/*
	 * Stepping-dependent config (cf. GM45 init_dmi).
	 * Pre-B0 steppings (A0/A1, rev < 2): set bit 11.
	 * B0+ steppings (rev >= 2, including C0 rev 3): clear bit 11.
	 * Vendor inteltool confirms 0xFC=0x31 (bit 11 clear) on C0.
	 */
	if (northbridge_stepping() < 0x02)
		dmibar_setbits32(0xfc, 1 << 11);
	else
		dmibar_clrbits32(0xfc, 1 << 11);

	dmibar_clrbits32(0x204, 3 << 10);
	dmibar_clrbits32(0xf4, 1 << 4);
	dmibar_setbits32(0xf0, 3 << 24);

	/* Per-lane equalization settings */
	dmibar_write32(0xf04, 0x07050880);
	dmibar_write32(0xf44, 0x07050880);
	dmibar_write32(0xf84, 0x07050880);
	dmibar_write32(0xfc4, 0x07050880);

	/* Lock write-once registers by performing a read-modify-write (OR 0) */
	dmibar_setbits32(0x308, 0);
	dmibar_setbits32(0x314, 0);
	dmibar_setbits32(0x324, 0);
	dmibar_setbits32(0x328, 0);
	dmibar_setbits32(0x334, 0);
	dmibar_setbits32(0x338, 0);
}

/* ================================================================== */
/* RCRB Setup                                                         */
/* ================================================================== */

/*
 * setup_rcrb() - Configure Root Complex Register Block routing
 *
 * Programs the EPBAR and DMIBAR with component IDs and link targets
 * so the root complex can route transactions between MCH (component 1)
 * and ICH (component 2).
 *
 * coreboot equivalent: setup_rcrb() in pcie.c
 */
static void setup_rcrb(void)
{
	int peg_present;

	printk(BIOS_DEBUG, "%s\n", __func__);
	/*
	 * Egress Port RCRB:
	 * - Component ID = 1 (MCH)
	 * - Link1: target = DMIBAR (component 1, link valid)
	 */
	epbar_clrsetbits32(EPESD, 0xff << 16, 1 << 16);

	epbar_clrsetbits32(EPLE1D, 0xff << 16, (1 << 16) | (1 << 0));
	epbar_write32(EPLE1A, DMIBAR_BASE);

	/*
	 * PEG Link2 setup: check if D1:F0 (PEG) is present by reading
	 * its vendor ID.  The vendor BIOS reads D1:F0 reg 0x00 and
	 * skips if 0xFF (device absent).
	 *
	 * Vendor BIOS: bioscode_7.rom @ 0x06ac-0x072a
	 */
	peg_present = (pci_read_config8(D1F0, 0x00) != 0xff);

	if (peg_present) {
		/*
		 * Egress Port Link2 -> PEG:
		 * - Component ID = 1 (MCH), link valid
		 */
		epbar_clrsetbits32(EPLE2D, 0xff << 16, (1 << 16) | (1 << 0));

		/*
		 * PEG device RCRB (D1:F0 extended config space):
		 * Configure the PEG endpoint's own link registers to
		 * point back to the egress port (EPBAR).
		 *
		 * 0x144: component ID = 1
		 * 0x158: target = EPBAR
		 * 0x150: component 1, link valid
		 */
		pci_update_config32(D1F0, 0x144,
			~(0xff << 16), 1 << 16);
		pci_write_config32(D1F0, 0x158,
			CONFIG_FIXED_EPBAR_MMIO_BASE);
		pci_update_config32(D1F0, 0x150,
			~(0xff << 16), (1 << 16) | (1 << 0));
	}

	/*
	 * DMI Port RCRB:
	 * - Component ID = 1 (MCH)
	 * - Link1: target port 0, component 2 (ICH), link valid -> RCBA
	 * - Link2: component 1 (MCH), link valid -> EPBAR
	 */
	dmibar_clrsetbits32(DMIESD, 0xff << 16, 1 << 16);

	dmibar_write32(DMILE1A, RCBA_BASE);
	dmibar_clrsetbits32(DMILE1D, 0xffff << 16,
		(0 << 24) | (2 << 16) | (1 << 0));

	dmibar_write32(DMILE2A, CONFIG_FIXED_EPBAR_MMIO_BASE);
	dmibar_clrsetbits32(DMILE2D, 0xff << 16, (1 << 16) | (1 << 0));
}

/* ================================================================== */
/* ASPM Setup                                                         */
/* ================================================================== */

/*
 * setup_aspm() - Configure Active State Power Management on DMI
 *
 * Enables L0s and L1 for mobile platforms (X61 is always mobile).
 *
 * coreboot equivalent: setup_aspm() in pcie.c (DMI portion)
 */
static void setup_aspm(void)
{
	printk(BIOS_DEBUG, "%s\n", __func__);
	/* DMI ASPM prerequisites */
	dmibar_setbits8(0x0e1c, 1 << 0);
	dmibar_setbits16(0x0f00, 3 << 8);
	dmibar_setbits16(0x0f00, 7 << 3);
	dmibar_clrbits32(0x0f14, 1 << 17);
	dmibar_clrbits16(0x0e1c, 1 << 8);

	/*
	 * Stepping-dependent adjustments (cf. GM45 setup_aspm).
	 * B0+ steppings (rev >= 2): program 0xe2c lane equalization.
	 */
	if (northbridge_stepping() >= 0x02) {
		dmibar_clrsetbits32(0x0e28 + 4, 0xfU << (52 - 32), 0xdU << (52 - 32));
		dmibar_write32(0x0e2c, 0x88d07333);
	}

	/* Enable ASPM L0s + L1 on DMI (write-once) - X61 is mobile */
	dmibar_setbits8(DMILCTL, 3 << 0);

	/* Exit latency timing */
	dmibar_clrsetbits32(DMILCAP, 63 << 12, (2 << 12) | (2 << 15));
	dmibar_write8(0x208 + 3, 0);
	dmibar_clrbits32(0x208, 3 << 20);
}

/* ================================================================== */
/* Public Entry Point                                                 */
/* ================================================================== */

/*
 * gm965_dmi_init() - Full DMI/PCIe link initialization
 *
 * Orchestrates egress port, DMI link, RCRB routing, and ASPM setup.
 * Should be called after gm965_early_init() and raminit, but before
 * PCI device enumeration.
 *
 * The southbridge side (i82801hx_dmi_setup) must be called first or
 * concurrently. After both sides are configured, call
 * i82801hx_dmi_poll_vc1() to wait for negotiation.
 *
 * BIOS equivalent: FFF897DB + FFF8BF7A + bioscode_7.rom
 * coreboot equivalent: gm45_late_init() in pcie.c
 */
void gm965_dmi_init(void)
{
	init_egress();
	init_dmi();
	setup_rcrb();
	setup_aspm();
}
