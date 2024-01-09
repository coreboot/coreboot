/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <amdblocks/ioapic.h>
#include <device/device.h>

unsigned long acpi_fill_madt(unsigned long current)
{
	struct device *dev = NULL;
	while ((dev = dev_find_path(dev, DEVICE_PATH_DOMAIN)) != NULL) {
		struct resource *res = probe_resource(dev, IOMMU_IOAPIC_IDX);
		if (!res)
			continue;

		current += acpi_create_madt_ioapic_from_hw((acpi_madt_ioapic_t *)current,
						   res->base);
	}

	return current;
}
