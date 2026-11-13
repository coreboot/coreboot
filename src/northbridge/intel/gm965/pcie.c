/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Intel GM965 (Crestline) Northbridge - PEG Port Initialization
 *
 * The PEG port (D1:F0) is only used on boards with a discrete GPU. The
 * values below match what the ThinkPad T61 (NVIDIA Quadro NVS 140M,
 * GM965 rev 0x0c) vendor BIOS leaves in the PEG config space.
 *
 * PEG RCRB registers are programmed by setup_rcrb() in dmi.c.
 */

#include <device/device.h>
#include <device/pci_ops.h>

#include "gm965.h"

static void init_pcie(struct device *peg)
{
	/* Negotiated link width: x16, link valid. Clear bit 8. */
	pci_update_config8(peg, PEG_NEGWIDTH, ~0x1f, 16 | 1);
	pci_and_config16(peg, PEG_NEGWIDTH, ~(1 << 8));

	/*
	 * Slot 1, 75 W power limit. The port is hot-plug capable, but it doesn't
	 * report command completion, so tell the OS not to wait for it.
	 */
	pci_write_config32(peg, PEG_SLOTCAP,
			   (1 << SLOTCAP_SLOTNUM_SHIFT) |
			   SLOTCAP_NO_CMD_COMPLETED |
			   (0 << SLOTCAP_POWER_SCALE_SHIFT) |
			   (75 << SLOTCAP_POWER_LIMIT_SHIFT));

	/*
	 * No extended VCs. Keep the default TC/VC map of VC0 (all TCs), which
	 * matches the default of the device behind the port: when only the port
	 * is limited to TC0, it drops the DMA reads of the NVIDIA GPU of the
	 * ThinkPad T61 using another TC.
	 */
	pci_and_config8(peg, D1F0_VCCAP, ~7);
}

static void setup_peg_pm(struct device *peg)
{
	pci_or_config32(peg, 0x200, 3 << 13);

	/* Vendor value: bits 31, 26 and 5 set, bits 27 and 25:24 clear */
	pci_update_config32(peg, 0x0f0, ~((1 << 31) | (0xf << 24) | (1 << 5)),
			    (1 << 31) | (1 << 26) | (1 << 5));

	/* Vendor value: bit 4 clear */
	pci_and_config32(peg, 0x0f4, ~(1 << 4));

	/* Vendor value: bits 0, 4 and 5 set */
	pci_update_config32(peg, 0x0fc, ~((1 << 14) | (1 << 13) | (1 << 10) | (0x7 << 5) |
					  (1 << 4) | (1 << 1) | (1 << 0)),
			    (1 << 5) | (1 << 4) | (1 << 0));
}

void gm965_pcie_init(struct device *peg)
{
	init_pcie(peg);
	setup_peg_pm(peg);
}
