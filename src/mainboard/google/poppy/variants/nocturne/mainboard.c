/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <chip.h>
#include <device/device.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <intelblocks/power_limit.h>

/* PL2 limit in watts for AML and KBL */
#define PL2_AML	18
#define PL2_KBL	15

static uint32_t get_pl2(void)
{
	struct device *igd_dev = pcidev_path_on_root(SA_DEVFN_IGD);
	uint16_t id;

	id = pci_read_config16(igd_dev, PCI_DEVICE_ID);
	/* Assume we only have KLB-Y and AML-Y SKUs */
	if (id == PCI_DID_INTEL_KBL_GT2_SULXM)
		return PL2_KBL;

	return PL2_AML;
}

/* Override dev tree settings per board */
void variant_devtree_update(void)
{
	struct soc_power_limits_config *soc_conf;
	config_t *cfg = config_of_soc();

	soc_conf = &cfg->power_limits_config;
	/* Update PL2 based on CPU */
	soc_conf->tdp_pl2_override = get_pl2();
}
