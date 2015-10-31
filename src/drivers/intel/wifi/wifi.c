/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Vladimir Serbinenko
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 or (at your option)
 * any later version of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <smbios.h>
#include <string.h>

static int smbios_write_wifi(struct device *dev, int *handle,
			     unsigned long *current)
{
	struct smbios_type_intel_wifi {
		u8 type;
		u8 length;
		u16 handle;
		u8 str;
		char eos[2];
	} __attribute__((packed));

	struct smbios_type_intel_wifi *t = (struct smbios_type_intel_wifi *)*current;
	int len = sizeof(struct smbios_type_intel_wifi);

	memset(t, 0, sizeof(struct smbios_type_intel_wifi));
	t->type = 0x85;
	t->length = len - 2;
	t->handle = *handle;
	/* Intel wifi driver expects this string to be in the table 0x85
	   with PCI IDs enumerated below.
	 */
	t->str = smbios_add_string(t->eos, "KHOIHGIUCCHHII");

	len = t->length + smbios_string_table_len(t->eos);
	*current += len;
	*handle += 1;
	return len;
}

static struct pci_operations pci_ops = {
	.set_subsystem = pci_dev_set_subsystem,
};

struct device_operations device_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = pci_dev_init,
	.scan_bus         = 0,
	.enable           = 0,
	.get_smbios_data  = smbios_write_wifi,
	.ops_pci          = &pci_ops,
};


static const unsigned short pci_device_ids[] = {
	0x0084, 0x0085, 0x0089, 0x008b, 0x008e, 0x0090,
	0x0886, 0x0888, 0x0891, 0x0893, 0x0895, 0x088f,
	0x4236, 0x4237, 0x4238, 0x4239, 0x423b, 0x423d,
	0 };

static const struct pci_driver pch_intel_wifi __pci_driver = {
	.ops	 = &device_ops,
	.vendor	 = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
