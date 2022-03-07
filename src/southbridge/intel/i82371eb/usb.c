/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "i82371eb.h"

/**
 * Initialize the USB (UHCI) controller.
 *
 * Depending on the configuration variable 'usb_enable', enable or
 * disable the USB (UHCI) controller.
 *
 * @param dev The device to use.
 */
static void usb_init(struct device *dev)
{
	/* TODO: No special init needed? */
}

static const struct device_operations usb_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= usb_init,
	.ops_pci		= 0, /* No subsystem IDs on 82371EB! */
};

/* Note: No USB on 82371FB/MX (PIIX/MPIIX) and 82437MX. */

/* Intel 82371SB (PIIX3) */
static const struct pci_driver usb_driver_sb __pci_driver = {
	.ops	= &usb_ops,
	.vendor	= PCI_VID_INTEL,
	.device	= PCI_DID_INTEL_82371SB_USB,
};

/* Intel 82371AB/EB/MB (PIIX4/PIIX4E/PIIX4M) */
/* The 440MX (82443MX) consists of 82443BX + 82371EB (uses same PCI IDs). */
static const struct pci_driver usb_driver_ab_eb_mb __pci_driver = {
	.ops	= &usb_ops,
	.vendor	= PCI_VID_INTEL,
	.device	= PCI_DID_INTEL_82371AB_USB,
};
