/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/pci_devs.h>
#include <soc/pcu.h>
#include <soc/ramstage.h>
#include <soc/util.h>

static void pcu6_final(struct device *dev)
{
	printk(BIOS_INFO, "%s: locking registers\n", dev_path(dev));
	pci_or_config32(dev, PCU_CR6_PLATFORM_RAPL_LIMIT_CFG_UPR,
			PLT_PWR_LIM_LOCK_UPR);
	pci_or_config32(dev, PCU_CR6_PLATFORM_POWER_INFO_CFG_UPR,
			PLT_PWR_INFO_LOCK_UPR);
}

static struct device_operations pcu6_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.final            = pcu6_final,
};

static const struct pci_driver pcu6_driver __pci_driver = {
	.ops = &pcu6_ops,
	.vendor = PCI_VID_INTEL,
	.device = PCU_CR6_DEVID,
};
