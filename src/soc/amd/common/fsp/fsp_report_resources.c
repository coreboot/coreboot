/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/root_complex.h>
#include <cbmem.h>
#include <console/console.h>
#include <device/device.h>
#include <fsp/util.h>
#include <types.h>

void read_fsp_resources(struct device *dev, unsigned long *idx)
{
	const uint32_t mem_usable = cbmem_top();
	const struct hob_header *hob_iterator;
	const struct hob_resource *res;

	if (fsp_hob_iterator_init(&hob_iterator) != CB_SUCCESS) {
		printk(BIOS_ERR, "%s incomplete because no HOB list was found\n", __func__);
		return;
	}

	while (fsp_hob_iterator_get_next_resource(&hob_iterator, &res) == CB_SUCCESS) {
		if (res->type == EFI_RESOURCE_SYSTEM_MEMORY && res->addr < mem_usable)
			/* 0 through low usable is already reported by the root complex code */
			continue;
		if (res->type == EFI_RESOURCE_MEMORY_MAPPED_IO)
			continue; /* Done separately */

		if (res->type == EFI_RESOURCE_SYSTEM_MEMORY)
			ram_range(dev, (*idx)++, res->addr, res->length);
		else if (res->type == EFI_RESOURCE_MEMORY_RESERVED)
			reserved_ram_range(dev, (*idx)++, res->addr, res->length);
		else
			printk(BIOS_ERR, "failed to set resources for type %d\n", res->type);
	}
}
