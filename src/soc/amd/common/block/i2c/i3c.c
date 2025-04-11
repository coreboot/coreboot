/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <amdblocks/i2c.h>
#include <console/console.h>
#include <device/device.h>
#include <types.h>

#if CONFIG(HAVE_ACPI_TABLES)
static const char *i3c_acpi_name(const struct device *dev)
{
	size_t i;
	size_t num_ctrlrs;
	const struct soc_i3c_ctrlr_info *ctrlr = soc_get_i3c_ctrlr_info(&num_ctrlrs);

	if (!(uintptr_t)dev->path.mmio.addr) {
		printk(BIOS_ERR, "NULL MMIO address at %s\n", __func__);
		return NULL;
	}

	for (i = 0; i < num_ctrlrs; i++) {
		if ((uintptr_t)dev->path.mmio.addr == ctrlr[i].bar)
			return ctrlr[i].acpi_name;
	}
	printk(BIOS_ERR, "%s: Could not find %lu\n", __func__, (uintptr_t)dev->path.mmio.addr);
	return NULL;
}

static void i3c_acpi_fill_ssdt(const struct device *dev)
{
	acpigen_write_scope(acpi_device_path(dev));
	acpigen_write_store_int_to_namestr(acpi_device_status(dev), "STAT");
	acpigen_pop_len(); /* Scope */
}
#endif

static void i3c_read_resources(struct device *dev)
{
	mmio_range(dev, 0, dev->path.mmio.addr, 4 * KiB);
}

struct device_operations soc_amd_i3c_mmio_ops = {
	.read_resources = i3c_read_resources,
	.set_resources = noop_set_resources,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name = i3c_acpi_name,
	.acpi_fill_ssdt = i3c_acpi_fill_ssdt,
#endif
};
