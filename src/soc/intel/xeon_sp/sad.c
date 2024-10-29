/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/pci.h>
#include <device/pci_ids.h>
#include <intelblocks/cfg.h>
#include <intelpch/lockdown.h>
#include <soc/pci_devs.h>

/*
 * Driver for SAD device (SKX and CPX only), now called CHA dev.
 * SKX/CPX [B(31), D:29, F:0/F:1]
 * SPR/GNR [B(31), D:29, F:0/F:1]
 */

static void sad_enable_resources(struct device *dev)
{
	pci_dev_enable_resources(dev);

	if (!CONFIG(SOC_INTEL_SKYLAKE_SP) && !CONFIG(SOC_INTEL_COOPERLAKE_SP))
		return;

	/*
	 * Enables the C-F segment as DRAM. Only necessary on SKX and CPX, that do
	 * not unconditionally enable those segments in FSP.
	 * The assumption here is that FSP-S does not need to access the C-F segment,
	 * and nothing in coreboot should access those segments before writing tables.
	 */

	uint32_t pam0123_unlock_dram = 0x33333330;
	uint32_t pam456_unlock_dram = 0x00333333;

	pci_write_config32(dev, SAD_ALL_PAM0123_CSR, pam0123_unlock_dram);
	pci_write_config32(dev, SAD_ALL_PAM456_CSR, pam456_unlock_dram);

	uint32_t reg1 = pci_read_config32(dev, SAD_ALL_PAM0123_CSR);
	uint32_t reg2 = pci_read_config32(dev, SAD_ALL_PAM456_CSR);
	printk(BIOS_DEBUG, "%s:%s pam0123_csr: 0x%x, pam456_csr: 0x%x\n",
		__FILE__, __func__, reg1, reg2);
}

static void sad_final(struct device *dev)
{
	if (!CONFIG(SOC_INTEL_COMMON_PCH_LOCKDOWN))
		return;

	if (get_lockdown_config() != CHIPSET_LOCKDOWN_COREBOOT)
		return;

	pci_or_config32(dev, SAD_ALL_PAM0123_CSR, PAM_LOCK);
}

static const unsigned short sad_ids[] = {
	SAD_ALL_DEVID,
	0
};

static struct device_operations sad_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= sad_enable_resources,
	.final			= sad_final,
};

static const struct pci_driver sad_driver __pci_driver = {
	.ops = &sad_ops,
	.vendor = PCI_VID_INTEL,
	.devices = sad_ids,
};
