/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <amdblocks/aoac.h>
#include <boot/coreboot_tables.h>
#include <commonlib/sdhci_nonpci_info.h>
#include <device/device.h>
#include <soc/aoac_defs.h>

#define EMMC_SDHCI_MMIO_SIZE	(4 * KiB)

static void emmc_publish_sdhci_nonpci(uint32_t bar0)
{
	if (!bar0)
		return;

	lb_add_sdhci_nonpci(bar0, EMMC_SDHCI_MMIO_SIZE, SDHCI_NONPCI_SLOT_EMMC,
			    SDHCI_NONPCI_FLAG_EMBEDDED);
}

static void emmc_read_resources(struct device *dev)
{
	mmio_range(dev, 0, dev->path.mmio.addr, EMMC_SDHCI_MMIO_SIZE);
}

static void emmc_enable(struct device *dev)
{
	if (!dev->enabled && !CONFIG(SOC_AMD_COMMON_BLOCK_EMMC_SKIP_POWEROFF))
		power_off_aoac_device(FCH_AOAC_DEV_EMMC);
}

static void emmc_init(struct device *dev)
{
	if (!dev->enabled || acpi_is_wakeup_s3())
		return;

	emmc_publish_sdhci_nonpci(dev->path.mmio.addr);
}

static const char *emmc_acpi_name(const struct device *dev)
{
	return "MMC0";
}

static void emmc_acpi_fill_ssdt(const struct device *dev)
{
	acpigen_write_scope(acpi_device_path(dev));
	acpigen_write_store_int_to_namestr(acpi_device_status(dev), "STAT");
	acpigen_pop_len(); /* Scope */
}

struct device_operations amd_emmc_mmio_ops = {
	.read_resources = emmc_read_resources,
	.set_resources = noop_set_resources,
	.scan_bus = scan_static_bus,
	.enable = emmc_enable,
	.init = emmc_init,
	.acpi_name = emmc_acpi_name,
	.acpi_fill_ssdt = emmc_acpi_fill_ssdt,
};
