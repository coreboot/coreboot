/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/data_fabric.h>
#include <console/console.h>
#include <device/device.h>
#include <types.h>

enum cb_err data_fabric_get_pci_bus_numbers(struct device *domain, uint8_t *first_bus,
					    uint8_t *last_bus)
{
	union df_pci_cfg_base pci_bus_base;
	union df_pci_cfg_limit pci_bus_limit;

	for (unsigned int i = 0; i < DF_PCI_CFG_MAP_COUNT; i++) {
		pci_bus_base.raw = data_fabric_broadcast_read32(DF_PCI_CFG_BASE(i));
		pci_bus_limit.raw = data_fabric_broadcast_read32(DF_PCI_CFG_LIMIT(i));

		/* TODO: Systems with more than one PCI root need to check to which PCI root
		   the PCI bus number range gets decoded to. */
		if (pci_bus_base.we && pci_bus_base.re) {
			/* TODO: Implement support for multiple PCI segments in coreboot */
			if (pci_bus_base.segment_num) {
				printk(BIOS_ERR, "DF PCI CFG register pair %d uses bus "
						 "segment != 0.\n", i);
				return CB_ERR;
			}

			*first_bus = pci_bus_base.bus_num_base;
			*last_bus = pci_bus_limit.bus_num_limit;
			return CB_SUCCESS;
		}
	}

	return CB_ERR;
}
