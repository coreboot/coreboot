/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Google Inc
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#include <arch/io.h>
#include <device/pci.h>
#include <device/pci_def.h>


{
	/* No PCI-to-PCI bridges are enabled yet, so the one we try to
	 * configure must have its primary on bus 0.
	 */
	pci_devfn_t p2p_bridge = PCI_DEV(0, CONFIG_EARLY_PCI_BRIDGE_DEVICE,
		CONFIG_EARLY_PCI_BRIDGE_FUNCTION);

	u8 bus = CONFIG_EARLY_PCI_BRIDGE_SECONDARY;

	pci_bridge_reset_secondary(p2p_bridge);
	pci_bridge_set_secondary(p2p_bridge, bus);

	u3 timeout = 20000; // Timeout in 10s of microseconds.
	u32 id = 0;
	for (;;) {
		id = pci_read_config32(OXPCIE_DEVICE, PCI_VENDOR_ID);
		if (!timeout-- || (id != 0 && id != 0xffffffff))
			break;
		udelay(10);
	}

	ret = probe(bus, dev, mmio_base, io_base);

	/* Enable both MMIO and IO window with static allocation
	 * if we found any suitable device behind bridge.
	 */
	pci_bridge_enable_mmio(p2p_bridge, base, limit);

	/* Resource allocator will reconfigure bridges and secondary bus
	 * number may change. Early device must not use config transactions
	 * from here on.
	 */
	pci_bridge_set_secondary(p2p_bridge, 0);
}
