/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Intel GM965 (Crestline) Northbridge - PEG Port Initialization
 * Reverse-engineered from ThinkPad X61 Phoenix BIOS
 *
 * BIOS references:
 *   bioscode_7.rom @ 0x03FC - MOV EAX, 0x80000800 (D1:F0 PCI config access)
 *   bioscode_5.rom - heavy PCI configuration (includes PEG setup)
 *
 * This file handles the PCI Express Graphics (PEG) port at D1:F0.
 * The X61 may or may not have PEG active (it has integrated graphics,
 * but the PEG port exists for potential docking station / external GPU).
 *
 * The init_pcie() and setup_aspm() PEG portions were previously
 * missing from dmi.c. This file completes the coreboot gm45_late_init()
 * equivalent by implementing the PEG-specific parts.
 *
 * Reference: coreboot GM45 pcie.c (init_pcie, setup_aspm PEG portion)
 */

#include <device/device.h>
#include <device/pci_ops.h>
#include <static.h>

#include "gm965.h"

/* ================================================================== */
/* PEG Port Initialization                                            */
/* ================================================================== */

/*
 * init_pcie() - Configure PEG port (D1:F0)
 *
 * Programs link width, slot capabilities, GPE enables, and
 * virtual channel configuration for the PEG port.
 *
 * On the X61, PEG is typically not used (integrated GPU only),
 * but the BIOS still touches D1:F0 if DEVEN bit 1 is set.
 *
 * coreboot equivalent: init_pcie() in gm45/pcie.c
 */
static void init_pcie(struct device *peg)
{
	/*
	 * Set negotiated link width.
	 * Register 0x224 bits [4:0] = max link width.
	 * Bit 0 = link valid.
	 * The X61 doesn't use SDVO over PEG, so if PEG is active
	 * it would be x16.
	 *
	 * coreboot: (peg_x16 ? 16 : 0) | 1
	 * For X61 with PEG enabled, assume x16 (no SDVO).
	 */
	pci_update_config8(peg, PEG_NEGWIDTH, ~0x1f, 16 | 1);  /* x16, link valid */

	/* Clear bit 8 of 0x224 (16-bit access) */
	pci_and_config16(peg, PEG_NEGWIDTH, ~(1 << 8));

	/*
	 * Slot configuration.
	 * coreboot uses devicetree to determine if PEG is a slot.
	 * The X61 PEG is not an exposed slot - it's either unused
	 * or connected to a fixed device (docking station GPU).
	 * Don't set slot-implemented bit.
	 */

	/*
	 * Slot power limits and slot number.
	 * Use slot 0 (NB PEG), power = 75W (val=75, exp=0).
	 * coreboot: slot 0 reserved for NB PEG, SB slots count from 1.
	 */
	{
		uint32_t slotcap;
		slotcap = (0 << SLOTCAP_SLOTNUM_SHIFT) |
			  (0 << SLOTCAP_POWER_SCALE_SHIFT) |
			  (75 << SLOTCAP_POWER_LIMIT_SHIFT);
		pci_write_config32(peg, PEG_SLOTCAP, slotcap);
	}

	/*
	 * Enable GPE (General Purpose Events) on PEG lane changes.
	 * Register 0xEC bits [2:0] = GPE enables.
	 * coreboot sets all 3 GPE bits.
	 */
	pci_or_config8(peg, PEG_PEGLC, PEGLC_GPE_EN_ALL);

	/*
	 * VC0: TC0 only.
	 * Clear all TC bits except TC0 in VC0 Resource Control.
	 */
	pci_and_config8(peg, D1F0_VC0RCTL, 1);

	/*
	 * Extended VC count: clear VC count field (VC0 only).
	 * Register 0x104 bits [2:0] = extended VC count.
	 */
	pci_and_config8(peg, D1F0_VCCAP, ~7);
}

/* ================================================================== */
/* PEG ASPM Setup                                                     */
/* ================================================================== */

/*
 * setup_peg_aspm() - Configure ASPM prerequisites on PEG port
 *
 * Programs numerous stepping-dependent registers on D1:F0 to
 * prepare the PEG port for Active State Power Management.
 *
 * The X61 shipped with B2+ stepping silicon, so we take the
 * B2+ code paths consistently.
 *
 * coreboot equivalent: setup_aspm() PEG portions in gm45/pcie.c
 */
static void setup_peg_aspm(struct device *peg)
{
	/* Set bits [14:13] in register 0x200 */
	pci_or_config32(peg, 0x200, 3 << 13);

	/* Clear bits [27:26] in register 0x0f0 */
	pci_and_config32(peg, 0x0f0, ~((1 << 27) | (1 << 26)));

	/* Set bits [25:24] in register 0x0f0 */
	pci_or_config32(peg, 0x0f0, 3 << 24);

	/* Clear bit 4 in register 0x0f4 */
	pci_and_config32(peg, 0x0f4, ~(1 << 4));

	/* Set bit 0 in register 0x0fc */
	pci_or_config32(peg, 0x0fc, 1 << 0);

	/* Set bit 1 in register 0x0fc */
	pci_or_config32(peg, 0x0fc, 1 << 1);

	/* Set bit 4 in register 0x0fc */
	pci_or_config32(peg, 0x0fc, 1 << 4);

	/* Clear bits [7:5] in register 0x0fc */
	pci_and_config32(peg, 0x0fc, ~(7 << 5));

	/* Enable L0s + L1 in LCTL (register 0x0b0) */
	pci_or_config32(peg, 0x0b0, 3 << 0);

	/* Set bits [25:24] in register 0x0f0 again (after LCTL) */
	pci_or_config32(peg, 0x0f0, 3 << 24);

	/*
	 * Stepping-dependent PEG ASPM (cf. GM45 setup_aspm).
	 * GM965 rev mapping: 0=A0, 1=A1, 2=B0, 3=C0.
	 * GM45 pattern: B0-B1 set bits, B2+ clear bits.
	 * GM965 C0 (rev 3) is equivalent to GM45 B2+.
	 */
	{
		uint8_t stepping = pci_read_config8(__pci_0_00_0, 0x08);

		/* 0x0f0 bit 31: B0 only (rev==2) set, B2+/C0 (rev>=3) clear */
		if (stepping == 0x02)
			pci_or_config32(peg, 0x0f0, 1 << 31);
		else if (stepping >= 0x03)
			pci_and_config32(peg, 0x0f0, ~(1 << 31));

		/* 0x0fc bit 10: B0 only (rev==2) set, B2+/C0 (rev>=3) clear */
		if (stepping == 0x02)
			pci_or_config32(peg, 0x0fc, 1 << 10);
		else if (stepping >= 0x03)
			pci_and_config32(peg, 0x0fc, ~(1 << 10));

		/* 0x0fc bit 14: B2+/C0 (rev>=3) set */
		if (stepping >= 0x03)
			pci_or_config32(peg, 0x0fc, 1 << 14);

		/* 0x0fc bit 13: B1+ (rev>=1) clear */
		if (stepping >= 0x01)
			pci_and_config32(peg, 0x0fc, ~(1 << 13));
	}

	/*
	 * PEG ASPM extended registers.
	 * These mirror the DMI ASPM config pattern.
	 */

	/* Clear bit 15 in register 0xa08 */
	pci_and_config32(peg, 0xa08, ~(1 << 15));

	/* Set bit 8 in register 0xa84 */
	pci_or_config32(peg, 0xa84, 1 << 8);

	/* Clear bit 17 in register 0xb14 */
	pci_and_config32(peg, 0xb14, ~(1 << 17));

	/* Set bits [9:8] in register 0xb00 */
	pci_or_config32(peg, 0xb00, 3 << 8);

	/* Set bits [5:3] in register 0xb00 */
	pci_or_config32(peg, 0xb00, 7 << 3);

	/* Clear then set bit 8 in register 0xa84 */
	pci_and_config32(peg, 0xa84, ~(1 << 8));
	pci_or_config32(peg, 0xa84, 1 << 8);

	/* Register 0xb04: set bits [27:23] = 0x0e, set bit 31 */
	pci_update_config32(peg, 0xb04, ~(0x1f << 23), (0x0e << 23) | (1 << 31));

	/* Register 0xb04: set bits [30:29] = 01b */
	pci_update_config32(peg, 0xb04, ~(0x03 << 29), 0x01 << 29);
}

/* ================================================================== */
/* Public Entry Point                                                 */
/* ================================================================== */

/*
 * gm965_pcie_init() - Full PEG port initialization
 *
 * Configures the PEG port and sets up ASPM prerequisites.
 * Called from chip ops .init when the PEG device is enabled.
 *
 * PEG RCRB registers are programmed by setup_rcrb() in dmi.c.
 *
 * coreboot equivalent: portions of gm45_late_init() in pcie.c
 */
void gm965_pcie_init(struct device *peg)
{
	init_pcie(peg);
	setup_peg_aspm(peg);
}
