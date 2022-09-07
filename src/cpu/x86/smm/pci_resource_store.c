/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/x86/smm.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>

void smm_pci_get_stored_resources(const volatile struct smm_pci_resource_info **out_slots,
				  size_t *out_size)
{
	*out_slots = smm_get_pci_resource_store();
	*out_size = CONFIG_SMM_PCI_RESOURCE_STORE_NUM_SLOTS;
}

bool smm_pci_resource_store_fill_resources(struct smm_pci_resource_info *slots, size_t num_slots,
					   const struct device **devices, size_t num_devices)
{
	size_t i_slot = 0;

	for (size_t i_dev = 0; i_dev < num_devices; i_dev++) {
		if (i_slot >= num_slots) {
			printk(BIOS_ERR, "Failed to store all PCI resources, number of devices exceeds %zd slots\n",
			       num_slots);
			return false;
		}

		if (!is_pci(devices[i_dev])) {
			printk(BIOS_WARNING, "Skipping storing PCI resources for device at index %zd, not a PCI device\n",
			       i_dev);
			continue;
		}

		pci_devfn_t pci_addr = PCI_BDF(devices[i_dev]);
		slots[i_slot].pci_addr = pci_addr;
		slots[i_slot].class_device = PCI_CLASS_GET_DEVICE(devices[i_dev]->class);
		slots[i_slot].class_prog = PCI_CLASS_GET_PROG(devices[i_dev]->class);

		/* Use the resource list to get our BARs. */
		if (!devices[i_dev]->resource_list)
			continue;

		size_t i_res = 0;
		for (const struct resource *res = devices[i_dev]->resource_list; res != NULL;
			res = res->next) {
			slots[i_slot].resources[i_res] = *res;
			slots[i_slot].resources[i_res].next = NULL;

			if (i_res > 0)
				slots[i_slot].resources[i_res - 1].next = (struct resource *)&slots[i_slot].resources[i_res];

			if (++i_res >= SMM_PCI_RESOURCE_STORE_NUM_RESOURCES) {
				if (res->next)
					printk(BIOS_WARNING, "Number of PCI resources exceeds supported storage count\n");
				break;
			}
		}

		i_slot++;
	}

	return true;
}

void __weak smm_mainboard_pci_resource_store_init(struct smm_pci_resource_info *slots,
						  size_t size)
{
}

void smm_pci_resource_store_init(struct smm_runtime *smm_runtime)
{
	smm_mainboard_pci_resource_store_init(&smm_runtime->pci_resources[0],
					 CONFIG_SMM_PCI_RESOURCE_STORE_NUM_SLOTS);
}
