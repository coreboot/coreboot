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

#include <delay.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
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
	 * The IGD stays enabled until ramstage, as raminit programs its
	 * clocks. Boards with a discrete GPU also need the PEG port.
	 *
	 * BIOS sets DEVEN during PCI_INIT (FFF89000 region).
	 * Don't enable ME as somehow the handshake does not succeed.
	 */
	u32 deven = DEVEN_D0F0 | DEVEN_D2F0 | DEVEN_D2F1;
	if (is_devfn_enabled(PCI_DEVFN(1, 0)))
		deven |= DEVEN_D1F0;
	pci_write_config32(D0F0, D0F0_DEVEN, deven);

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

/*
 * gm965_peg_has_display() - Detect a discrete GPU on the PEG port
 *
 * Boards can be sold with either the IGD or a discrete GPU on PEG, like the
 * ThinkPad T61. Like i945, check the slot presence, wait for the link to
 * train and read the class of the device behind the port. If no display
 * controller is found, the PEG port is disabled again.
 */
bool gm965_peg_has_display(void)
{
	const pci_devfn_t gpu = PCI_DEV(1, 0, 0);
	bool found = false;
	int timeout;

	if (!(pci_read_config32(D0F0, D0F0_DEVEN) & DEVEN_D1F0))
		return false;

	if (!(pci_read_config16(D1F0, PEG_SLTSTS) & PCI_EXP_SLTSTA_PDS))
		goto disable_peg;

	/* Wait up to 100 ms for the link to train */
	for (timeout = 1000; timeout; timeout--) {
		const u16 lsts = pci_read_config16(D1F0, PEG_LSTS);
		/* Training done and a negotiated link width, bits [9:4] */
		if (!(lsts & PCI_EXP_LNKSTA_LT) && ((lsts >> 4) & 0x3f))
			break;
		udelay(100);
	}
	if (!timeout)
		goto disable_peg;

	/* Temporarily assign bus 1 to read the device behind the port */
	pci_write_config8(D1F0, PCI_SECONDARY_BUS, 1);
	pci_write_config8(D1F0, PCI_SUBORDINATE_BUS, 1);
	found = pci_read_config16(gpu, PCI_VENDOR_ID) != 0xffff &&
		(pci_read_config32(gpu, PCI_CLASS_REVISION) >> 24) == PCI_BASE_CLASS_DISPLAY;
	pci_write_config8(D1F0, PCI_SUBORDINATE_BUS, 0);
	pci_write_config8(D1F0, PCI_SECONDARY_BUS, 0);

	if (found)
		return true;

disable_peg:
	pci_and_config32(D0F0, D0F0_DEVEN, ~DEVEN_D1F0);
	return false;
}
