/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci_ids.h>
#include <smbios.h>
#include <string.h>

#include "wifi_private.h"

static int smbios_write_intel_wifi(struct device *dev, int *handle, unsigned long *current)
{
	struct smbios_type_intel_wifi {
		u8 type;
		u8 length;
		u16 handle;
		u8 str;
		u8 eos[2];
	} __packed;

	struct smbios_type_intel_wifi *t = (struct smbios_type_intel_wifi *)*current;
	int len = sizeof(struct smbios_type_intel_wifi);

	memset(t, 0, sizeof(struct smbios_type_intel_wifi));
	t->type = 0x85;
	t->length = len - 2;
	t->handle = *handle;
	/* Intel wifi driver expects this string to be in the table 0x85. */
	t->str = smbios_add_string(t->eos, "KHOIHGIUCCHHII");

	len = t->length + smbios_string_table_len(t->eos);
	*current += len;
	*handle += 1;
	return len;
}

int smbios_write_wifi_pcie(struct device *dev, int *handle, unsigned long *current)
{
	if (dev->vendor == PCI_VENDOR_ID_INTEL)
		return smbios_write_intel_wifi(dev, handle, current);

	return 0;
}

int smbios_write_wifi_cnvi(struct device *dev, int *handle, unsigned long *current)
{
	return smbios_write_wifi_pcie(dev->bus->dev, handle, current);
}
