/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/pci_ops.h>
#include <southbridge/intel/lynxpoint/hsio/hsio.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include <types.h>

/**
 * FIXME: Ask Intel whether all lanes need to be programmed as specified
 * in the PCH BWG. If not, make separate tables and only check this once.
 */
void hsio_sata_shared_update(const uint32_t addr, const uint32_t and, const uint32_t or)
{
	const uint8_t lane_owner = pci_read_config8(PCH_PCIE_DEV(0), 0x410);

	if ((addr & 0xfe00) == 0x2000 && (lane_owner & (1 << 4)))
		return;

	if ((addr & 0xfe00) == 0x2200 && (lane_owner & (1 << 5)))
		return;

	if (CONFIG(INTEL_LYNXPOINT_LP)) {
		if ((addr & 0xfe00) == 0x2400 && (lane_owner & (1 << 6)))
			return;

		if ((addr & 0xfe00) == 0x2600 && (lane_owner & (1 << 7)))
			return;
	}
	hsio_update(addr, and, or);
}

void hsio_xhci_shared_update(const uint32_t addr, const uint32_t and, const uint32_t or)
{
	const uint8_t lane_owner = pci_read_config8(PCH_PCIE_DEV(0), 0x410);
	if (CONFIG(INTEL_LYNXPOINT_LP)) {
		if ((addr & 0xfe00) == 0x2400 && ((lane_owner >> 0) & 3) != 2)
			return;

		if ((addr & 0xfe00) == 0x2600 && ((lane_owner >> 2) & 3) != 2)
			return;
	} else {
		if ((addr & 0xfe00) == 0x2c00 && ((lane_owner >> 2) & 3) != 2)
			return;

		if ((addr & 0xfe00) == 0x2e00 && ((lane_owner >> 0) & 3) != 2)
			return;
	}
	hsio_update(addr, and, or);
}
