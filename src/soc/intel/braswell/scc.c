/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_gnvs.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <soc/iosf.h>
#include <soc/nvs.h>
#include <soc/ramstage.h>

void scc_enable_acpi_mode(struct device *dev, int iosf_reg, int nvs_index)
{
	struct resource *bar;
	struct global_nvs *gnvs;

	/* Find ACPI NVS to update BARs */
	gnvs = acpi_get_gnvs();
	if (!gnvs)
		return;

	/* Save BAR0 and BAR1 to ACPI NVS */
	bar = find_resource(dev, PCI_BASE_ADDRESS_0);
	if (bar)
		gnvs->dev.scc_bar0[nvs_index] = bar->base;
	bar = find_resource(dev, PCI_BASE_ADDRESS_2);
	if (bar)
		gnvs->dev.scc_bar1[nvs_index] = bar->base;

	/* Device is enabled in ACPI mode */
	gnvs->dev.scc_en[nvs_index] = 1;
}
