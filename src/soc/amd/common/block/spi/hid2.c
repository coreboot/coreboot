/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <amdblocks/aoac.h>
#include <device/device.h>
#include <device/mmio.h>
#include <soc/aoac_defs.h>
#include <types.h>

static void hid2_enable(struct device *dev)
{
	if (dev->enabled) {
		power_on_aoac_device(FCH_AOAC_DEV_HID2);
		wait_for_aoac_enabled(FCH_AOAC_DEV_HID2);
	} else {
		power_off_aoac_device(FCH_AOAC_DEV_HID2);
	}
}

static void hid2_read_resources(struct device *dev)
{
	mmio_range(dev, 0, dev->path.mmio.addr, 4 * KiB);
}

#if CONFIG(HAVE_ACPI_TABLES)
static const char *hid2_acpi_name(const struct device *dev)
{
	return "HID2";
}

static void hid2_acpi_fill_ssdt(const struct device *dev)
{
	acpigen_write_scope(acpi_device_path(dev));
	acpigen_write_store_int_to_namestr(acpi_device_status(dev), "STAT");
	acpigen_pop_len(); /* Scope */
}
#endif

struct device_operations soc_amd_hid2_ops = {
	.enable = hid2_enable,
	.read_resources = hid2_read_resources,
	.set_resources = noop_set_resources,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_name = hid2_acpi_name,
	.acpi_fill_ssdt = hid2_acpi_fill_ssdt,
#endif
};
