/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci_ids.h>
#include <smbios.h>

#include "wifi_private.h"

static int smbios_write_intel_wifi(struct device *dev, int *handle, unsigned long *current)
{
	if (dev->vendor != PCI_VID_INTEL)
		return 0;

	struct smbios_type_intel_wifi {
		struct smbios_header header;
		u8 str;
		u8 eos[2];
	} __packed;

	struct smbios_type_intel_wifi *t = smbios_carve_table(*current, 0x85,
							      sizeof(*t), *handle);

	/* Intel wifi driver expects this string to be in the table 0x85. */
	t->str = smbios_add_string(t->eos, "KHOIHGIUCCHHII");

	const int len = smbios_full_table_len(&t->header, t->eos);
	*current += len;
	*handle += 1;
	return len;
}

int smbios_write_wifi_pcie(struct device *dev, int *handle, unsigned long *current)
{
	int len = smbios_write_intel_wifi(dev, handle, current);
	len += get_smbios_data(dev, handle, current);
	return len;
}

int smbios_write_wifi_cnvi(struct device *dev, int *handle, unsigned long *current)
{
	return smbios_write_wifi_pcie(dev->bus->dev, handle, current);
}
