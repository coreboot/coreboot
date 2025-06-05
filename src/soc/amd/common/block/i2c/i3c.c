/* SPDX-License-Identifier: GPL-2.0-only */
#include <device/mmio.h>
#include <acpi/acpigen.h>
#include <amdblocks/aoac.h>
#include <amdblocks/i2c.h>
#include <console/console.h>
#include <device/device.h>
#include <types.h>

static int get_i3c_bus_for_dev(const struct device *dev,
			       const struct soc_i3c_ctrlr_info *ctrlr,
			       const size_t num_ctrlrs)
{
	if (!dev || !ctrlr)
		return -1;

	if (!(uintptr_t)dev->path.mmio.addr) {
		printk(BIOS_ERR, "NULL MMIO address at %s\n", __func__);
		return -1;
	}

	for (int i = 0; i < num_ctrlrs; i++) {
		if (dev->path.mmio.addr != ctrlr[i].bar)
			continue;
		return i;
	}
	printk(BIOS_ERR, "%s: Could not find i3c bus for %s\n", __func__, dev_path(dev));

	return -1;
}

static const struct soc_i3c_ctrlr_info *
get_i3c_info_for_dev(const struct device *dev)
{
	size_t num_ctrlrs;
	const struct soc_i3c_ctrlr_info *ctrlr = soc_get_i3c_ctrlr_info(&num_ctrlrs);
	const int bus = get_i3c_bus_for_dev(dev, ctrlr, num_ctrlrs);

	if (ctrlr && bus >= 0 && bus < num_ctrlrs)
		return &ctrlr[bus];

	printk(BIOS_ERR, "%s: Could not find soc_i3c_ctrlr_info for %s\n", __func__, dev_path(dev));

	return NULL;
}

#if CONFIG(HAVE_ACPI_TABLES)
static const char *i3c_acpi_name(const struct device *dev)
{
	const struct soc_i3c_ctrlr_info *info = get_i3c_info_for_dev(dev);
	if (info)
		return info->acpi_name;

	printk(BIOS_ERR, "%s: Could not find soc_i3c_ctrlr_info for %s\n", __func__, dev_path(dev));
	return NULL;
}

static void i3c_acpi_fill_ssdt(const struct device *dev)
{
	acpigen_write_scope(acpi_device_path(dev));
	acpigen_write_store_int_to_namestr(acpi_device_status(dev), "STAT");
	acpigen_pop_len(); /* Scope */
}
#endif

/* Even though this is called enable, it gets called for both enabled and disabled devices. */
static void i3c_enable(struct device *dev)
{
	const struct soc_i3c_ctrlr_info *info = get_i3c_info_for_dev(dev);
	if (!info)
		return;

	if (dev->enabled) {
		power_on_aoac_device(info->aoac_device);
		wait_for_aoac_enabled(info->aoac_device);
	} else {
		power_off_aoac_device(info->aoac_device);
	}
}

static void i3c_init(struct device *dev)
{
	size_t num_ctrlrs, num_buses;
	const struct soc_i3c_ctrlr_info *ctrlr = soc_get_i3c_ctrlr_info(&num_ctrlrs);
	const int bus = get_i3c_bus_for_dev(dev, ctrlr, num_ctrlrs);
	if (bus < 0)
		return;

	const struct dw_i2c_bus_config *cfg = soc_get_i2c_bus_config(&num_buses);

	if (cfg)
		soc_i2c_misc_init(bus, cfg);
}

static void i3c_read_resources(struct device *dev)
{
	mmio_range(dev, 0, dev->path.mmio.addr, 4 * KiB);
}

/*
 * Currently there's no I3C driver, thus the I3C hardware cannot be used pre ramstage.
 * It's sufficient to power switch the I3C controllers in the enable method.
 */
struct device_operations soc_amd_i3c_mmio_ops = {
	.enable = i3c_enable,
	.init = i3c_init,
	.read_resources = i3c_read_resources,
	.set_resources = noop_set_resources,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name = i3c_acpi_name,
	.acpi_fill_ssdt = i3c_acpi_fill_ssdt,
#endif
};
