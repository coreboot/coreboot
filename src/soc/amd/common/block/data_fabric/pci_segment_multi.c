/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/data_fabric.h>
#include <amdblocks/root_complex.h>
#include <console/console.h>
#include <device/device.h>
#include <types.h>

enum cb_err data_fabric_get_pci_bus_numbers(struct device *domain, uint8_t *segment_group,
					    uint8_t *first_bus, uint8_t *last_bus)
{
	const signed int iohc_dest_fabric_id = get_iohc_fabric_id(domain);
	union df_pci_cfg_base pci_bus_base;
	union df_pci_cfg_limit pci_bus_limit;

	for (unsigned int i = 0; i < DF_PCI_CFG_MAP_COUNT; i++) {
		pci_bus_base.raw = data_fabric_broadcast_read32(DF_PCI_CFG_BASE(i));
		pci_bus_limit.raw = data_fabric_broadcast_read32(DF_PCI_CFG_LIMIT(i));

		if (pci_bus_limit.dst_fabric_id != iohc_dest_fabric_id)
			continue;

		if (pci_bus_base.we && pci_bus_base.re) {
			*segment_group = pci_bus_base.segment_num;
			*first_bus = pci_bus_base.bus_num_base;
			*last_bus = pci_bus_limit.bus_num_limit;
			return CB_SUCCESS;
		}
	}

	printk(BIOS_ERR, "No valid DF PCI CFG register pair found for domain %x.\n",
	       dev_get_domain_id(domain));
	return CB_ERR;
}
