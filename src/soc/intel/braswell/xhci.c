/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <reg_script.h>

#include <soc/iomap.h>
#include <soc/iosf.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/ramstage.h>
#include <soc/xhci.h>

#include "chip.h"

static void xhci_init(struct device *dev)
{
	struct soc_intel_braswell_config *config = config_of(dev);

	if (config->usb_comp_bg) {
		struct reg_script ops[] = {
			REG_IOSF_WRITE(IOSF_PORT_USBPHY, USBPHY_COMPBG, config->usb_comp_bg),
			REG_SCRIPT_END
		};
		printk(BIOS_INFO, "Override USB2_COMPBG to: 0x%X\n", config->usb_comp_bg);
		reg_script_run(ops);
	}
}

static struct device_operations xhci_device_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= xhci_init,
	.ops_pci		= &soc_pci_ops,
};

static const struct pci_driver soc_xhci __pci_driver = {
	.ops	= &xhci_device_ops,
	.vendor	= PCI_VID_INTEL,
	.device	= XHCI_DEVID
};
