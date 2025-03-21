/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <soc/mmu.h>
#include <soc/mmu_common.h>
#include <soc/symbols_common.h>
#include <soc/cpucp.h>
#include <soc/pcie.h>

static struct device_operations pci_domain_ops = {
	.read_resources = noop_read_resources,
	.set_resources = noop_set_resources,
};

static void soc_read_resources(struct device *dev)
{
	/* placeholder */
}

static void soc_init(struct device *dev)
{
	/* placeholder */
}

static struct device_operations soc_ops = {
	.read_resources = soc_read_resources,
	.set_resources = noop_set_resources,
	.init = soc_init,
};

static void enable_soc_dev(struct device *dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		if (mainboard_needs_pcie_init())
			dev->ops = &pci_domain_ops;
		else
			printk(BIOS_DEBUG, "Skip setting PCIe ops\n");
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER)
		dev->ops = &soc_ops;
}

struct chip_operations soc_qualcomm_x1p42100_ops = {
	.name = "SOC Qualcomm X1P-42-100",
	.enable_dev = enable_soc_dev,
};
