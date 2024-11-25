/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/pci_devs.h>
#include <soc/pcu.h>
#include <soc/ramstage.h>
#include <soc/util.h>

static void pcu3_final(struct device *dev)
{
	printk(BIOS_INFO, "%s: locking registers\n", dev_path(dev));
	pci_or_config32(dev, PCU_CR3_CONFIG_TDP_CONTROL, TDP_LOCK);
	if (CONFIG(SOC_INTEL_SKYLAKE_SP) || CONFIG(SOC_INTEL_COOPERLAKE_SP)) {
		pci_or_config32(dev, PCU_CR3_FLEX_RATIO, OC_LOCK);
	}
}

static struct device_operations pcu3_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.final            = pcu3_final,
};

static const struct pci_driver pcu3_driver __pci_driver = {
	.ops = &pcu3_ops,
	.vendor = PCI_VID_INTEL,
	.device = PCU_CR3_DEVID,
};
