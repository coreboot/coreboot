/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Intel GM965 (Crestline) Northbridge - Early Initialization
 * Reverse-engineered from ThinkPad X61 Phoenix BIOS
 *
 * BIOS functions:
 *   FFFE04F5 (early_bar_setup)     - Programs MCHBAR/DMIBAR/EPBAR BARs
 *   FFFE1111 (early_epbar_dmibar)  - EPBAR/DMIBAR register setup
 *   FFFE0F2D (sskpd_check)         - Checks SSKPD for 0xCAFE warm boot marker
 *   FFFE17DC (early_init_main)     - Orchestrates Phase 1
 *
 * Reference: coreboot GM45 early_init.c (nearly identical for GM965)
 */

#include <device/pci_ops.h>
#include <northbridge/intel/common/fixed_bars.h>
#include <northbridge/intel/gm965/gm965.h>

/* BAR base addresses from Kconfig */
#define MCHBAR_BASE   CONFIG_FIXED_MCHBAR_MMIO_BASE
#define DMIBAR_BASE   CONFIG_FIXED_DMIBAR_MMIO_BASE
#define EPBAR_BASE    CONFIG_FIXED_EPBAR_MMIO_BASE

/*
 * gm965_early_init() - Phase 1 early northbridge setup
 *
 * Programs the memory-mapped BAR registers into D0:F0 PCI config space
 * and opens PAM regions so the C0000-FFFFF range is accessible as RAM.
 *
 * Must be called very early, before any MCHBAR/DMIBAR/EPBAR access.
 *
 * BIOS equivalent: FFFE04F5 + FFFE17DC (16-bit early init)
 * coreboot equivalent: gm45_early_init() in early_init.c
 */
void gm965_early_init(void)
{
	/*
	 * Setup MCHBAR (D0:F0 offset 0x48).
	 * X61 BIOS uses 0xFED14000 (non-standard; GM45 default is 0xFED10000).
	 * Bit 0 = enable.
	 */
	pci_write_config32(D0F0, D0F0_MCHBAR_LO, MCHBAR_BASE | 1);
	pci_write_config32(D0F0, D0F0_MCHBAR_HI, 0);

	/* Setup DMIBAR (D0:F0 offset 0x68). */
	pci_write_config32(D0F0, D0F0_DMIBAR_LO, DMIBAR_BASE | 1);
	pci_write_config32(D0F0, D0F0_DMIBAR_HI, 0);

	/* Setup EPBAR (D0:F0 offset 0x40). */
	pci_write_config32(D0F0, D0F0_EPBAR_LO, EPBAR_BASE | 1);
	pci_write_config32(D0F0, D0F0_EPBAR_HI, 0);

	/*
	 * Open PAM regions - set C0000-FFFFF to access RAM on both
	 * reads and writes. This allows decompressed BIOS modules
	 * (or coreboot ramstage) to be copied into and executed from
	 * this address range.
	 *
	 * PAM0 (0xF0000-0xFFFFF): 0x30 = read/write RAM
	 * PAM1-6 (0xC0000-0xEFFFF): 0x33 = both halves read/write RAM
	 *
	 * BIOS equivalent: observed in early init and PCI_INIT regions.
	 */
	pci_write_config8(D0F0, D0F0_PAM(0), 0x30);
	pci_write_config8(D0F0, D0F0_PAM(1), 0x33);
	pci_write_config8(D0F0, D0F0_PAM(2), 0x33);
	pci_write_config8(D0F0, D0F0_PAM(3), 0x33);
	pci_write_config8(D0F0, D0F0_PAM(4), 0x33);
	pci_write_config8(D0F0, D0F0_PAM(5), 0x33);
	pci_write_config8(D0F0, D0F0_PAM(6), 0x33);

	/*
	 * Enable devices (DEVEN register, D0:F0 offset 0x54).
	 * Default has D0F0 + IGD enabled; ensure PEG is available
	 * if a discrete GPU is present (X61 has no PEG, but X61s does).
	 *
	 * BIOS sets DEVEN during PCI_INIT (FFF89000 region).
	 * Don't enable ME as somehow the handshake does not succeed.
	 */
	pci_write_config32(D0F0, D0F0_DEVEN,
		DEVEN_D0F0 | DEVEN_D2F0 | DEVEN_D2F1);

	/*
	 * MCHBAR+0x0094: Non-isoch arbitration / channel decode config.
	 *
	 * Vendor BIOS (FFFE11DA): reads MCHBAR+0x94, clears bits [20:19],
	 * then sets bit 20.  This is done conditionally in the vendor
	 * (guarded by a flag from PCI D0:F0 capability probing), but
	 * the X61 always takes this path.
	 *
	 * Bits [20:19] control the non-isoch arbiter decode mode:
	 *   00 = disabled
	 *   01 = mode A
	 *   10 = mode B (vendor setting)
	 *   11 = mode C
	 */
	mchbar_clrsetbits32(FSBPMC5, 3 << 19, 2 << 19);

	/*
	 * DMIBAR+0x0204: DMI link control 2.
	 *
	 * Vendor BIOS (FFFE11FE): unconditionally clears bits [11:10].
	 * These control DMI link de-emphasis / equalization settings
	 * that must be cleared before DRAM init begins.
	 */
	dmibar_clrbits32(DMILCTL2, 3 << 10);
}
