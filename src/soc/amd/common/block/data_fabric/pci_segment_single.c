/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/data_fabric.h>
#include <device/device.h>
#include <types.h>

enum cb_err data_fabric_get_pci_bus_numbers(struct device *domain, uint8_t *first_bus,
					    uint8_t *last_bus)
{
	union df_pci_cfg_map pci_bus_map;

	for (unsigned int i = 0; i < DF_PCI_CFG_MAP_COUNT; i++) {
		pci_bus_map.raw = data_fabric_broadcast_read32(DF_PCI_CFG_MAP(i));

		/* TODO: Systems with more than one PCI root need to check to which PCI root
		   the PCI bus number range gets decoded to. */
		if (pci_bus_map.we && pci_bus_map.re) {
			*first_bus = pci_bus_map.bus_num_base;
			*last_bus = pci_bus_map.bus_num_limit;
			return CB_SUCCESS;
		}
	}

	return CB_ERR;
}
