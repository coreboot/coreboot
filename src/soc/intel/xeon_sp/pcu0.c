/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/pci_devs.h>
#include <soc/pcu.h>
#include <soc/ramstage.h>
#include <soc/util.h>

static void pcu0_final(struct device *dev)
{
	printk(BIOS_INFO, "%s: locking registers\n", dev_path(dev));
	pci_or_config32(dev, PCU_CR0_P_STATE_LIMITS, P_STATE_LIMITS_LOCK);
	pci_or_config32(dev, PCU_CR0_PACKAGE_RAPL_LIMIT_UPR,
			PKG_PWR_LIM_LOCK_UPR);
	pci_or_config32(dev, PCU_CR0_TURBO_ACTIVATION_RATIO,
			TURBO_ACTIVATION_RATIO_LOCK);

	/* Set PMAX_LOCK - must be set before RESET CPL4 */
	pci_or_config32(dev, PCU_CR0_PMAX, PMAX_LOCK);
}

static struct device_operations pcu0_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.final            = pcu0_final,
};

static const struct pci_driver pcu0_driver __pci_driver = {
	.ops = &pcu0_ops,
	.vendor = PCI_VID_INTEL,
	.device = PCU_CR0_DEVID,
};
