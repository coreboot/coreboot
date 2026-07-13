/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_gnvs.h>
#include <boot/coreboot_tables.h>
#include <commonlib/sdhci_nonpci_info.h>
#include <device/device.h>
#include <device/pci.h>
#include <soc/iosf.h>
#include <soc/device_nvs.h>
#include <soc/ramstage.h>

#define SCC_SDHCI_MMIO_SIZE	0x1000

static void scc_publish_sdhci_nonpci(int slot, uint32_t bar0)
{
	uint8_t flags = 0;

	if (!bar0)
		return;

	if (slot == SDHCI_NONPCI_SLOT_EMMC)
		flags = SDHCI_NONPCI_FLAG_EMBEDDED;

	lb_add_sdhci_nonpci(bar0, SCC_SDHCI_MMIO_SIZE, slot, flags);
}

void scc_enable_acpi_mode(struct device *dev, int iosf_reg, int nvs_index)
{
	struct resource *bar;
	struct device_nvs *dev_nvs = acpi_get_device_nvs();

	/* Save BAR0 and BAR1 to ACPI NVS */
	bar = probe_resource(dev, PCI_BASE_ADDRESS_0);
	if (bar)
		dev_nvs->scc_bar0[nvs_index] = bar->base;
	bar = probe_resource(dev, PCI_BASE_ADDRESS_2);
	if (bar)
		dev_nvs->scc_bar1[nvs_index] = bar->base;

	/* Device is enabled in ACPI mode */
	dev_nvs->scc_en[nvs_index] = 1;

	/* Publish BAR0 for non-PCI SDHCI discovery by payloads. */
	scc_publish_sdhci_nonpci(nvs_index, dev_nvs->scc_bar0[nvs_index]);
}
