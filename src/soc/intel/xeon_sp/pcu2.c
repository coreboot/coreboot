/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/pci_devs.h>
#include <soc/pcu.h>
#include <soc/ramstage.h>
#include <soc/util.h>

static void pcu2_init(struct device *dev)
{
	printk(BIOS_INFO, "%s: init registers\n", dev_path(dev));

	if (CONFIG(SOC_INTEL_SKYLAKE_SP)) {
		pci_or_config32(dev, PCU_CR2_PKG_CST_ENTRY_CRITERIA_MASK, 0xFFFFFF);
		pci_or_config32(dev, PCU_CR2_PKG_CST_ENTRY_CRITERIA_MASK2, 7);

		pci_write_config32(dev, PCU_CR2_PROCHOT_RESPONSE_RATIO_REG,
				   PROCHOT_RATIO);
		pci_or_config32(dev, PCU_CR2_DYNAMIC_PERF_POWER_CTL,
				UNOCRE_PLIMIT_OVERRIDE_SHIFT);
	}
}

static void pcu2_final(struct device *dev)
{
	printk(BIOS_INFO, "%s: locking registers\n", dev_path(dev));
	if (CONFIG(SOC_INTEL_SKYLAKE_SP) || CONFIG(SOC_INTEL_COOPERLAKE_SP)) {
		pci_or_config32(dev, PCU_CR2_DRAM_PLANE_POWER_LIMIT_LWR,
				PP_PWR_LIM_LOCK);
	} else {
		pci_or_config32(dev, PCU_CR2_DRAM_PLANE_POWER_LIMIT_UPR,
				PP_PWR_LIM_LOCK_UPR);
	}
	pci_or_config32(dev, PCU_CR2_DRAM_POWER_INFO_UPR,
			DRAM_POWER_INFO_LOCK_UPR);
}

static struct device_operations pcu2_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = pcu2_init,
	.final            = pcu2_final,
};

static const struct pci_driver pcu2_driver __pci_driver = {
	.ops = &pcu2_ops,
	.vendor = PCI_VID_INTEL,
	.device = PCU_CR2_DEVID,
};
