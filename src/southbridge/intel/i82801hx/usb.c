/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Intel ICH8-M USB Controller Initialization
 *
 * Reverse-engineered from ThinkPad X61 Phoenix BIOS (2B_0.rom module)
 * Reference: coreboot i82801ix/usb_ehci.c, i82801ix/i82801ix.c
 *
 * ICH8-M USB topology:
 *   D29:F0  UHCI #1  (PCI DID 0x2830)
 *   D29:F1  UHCI #2  (PCI DID 0x2831)
 *   D29:F2  UHCI #3  (PCI DID 0x2832)
 *   D29:F7  EHCI #1  (PCI DID 0x2836)
 *   D26:F0  UHCI #4  (PCI DID 0x2834)
 *   D26:F1  UHCI #5  (PCI DID 0x2835)
 *   D26:F7  EHCI #2  (PCI DID 0x283A)
 *
 * EHCI is initialized by usb_ehci.c as a PCI device after BAR/resource
 * assignment, matching the ICH7 coreboot model.
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <southbridge/intel/i82801hx/i82801hx.h>

/* ================================================================== */
/* UHCI Initialization                                                */
/* ================================================================== */

/*
 * i82801hx_uhci_init() - Initialize all UHCI (USB 1.1) controllers
 *
 * UHCI controllers are simple PCI devices that need minimal setup:
 *   - Enable Bus Master for DMA transfers
 *   - I/O BAR is assigned by PCI resource allocation
 *
 * The BIOS does not perform any special UHCI register programming
 * beyond standard PCI command bits.
 */
static void usb_uhci_init(struct device *dev)
{
	printk(BIOS_DEBUG, "UHCI: Setting up controller.. ");

	/* USB Specification says the device must be Bus Master. */
	pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER);

	/*
	 * Do not apply the ICH7 config 0xca workaround here.  It is not present
	 * in the X61 vendor USB sequence we found, and touching it on ICH8-M can
	 * wedge enumeration before the remaining UHCI functions are initialized.
	 */

	printk(BIOS_DEBUG, "done.\n");
}

static struct device_operations usb_uhci_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= usb_uhci_init,
	.ops_pci		= &pci_dev_ops_pci,
};

static const unsigned short pci_uhci_device_ids[] = {
	PCI_DID_INTEL_82801HB_USB1,
	PCI_DID_INTEL_82801HB_USB2,
	PCI_DID_INTEL_82801HB_USB3,
	PCI_DID_INTEL_82801HB_USB4,
	PCI_DID_INTEL_82801HB_USB5,
	0
};

static const struct pci_driver ich8_usb_uhci __pci_driver = {
	.ops		= &usb_uhci_ops,
	.vendor		= PCI_VID_INTEL,
	.devices	= pci_uhci_device_ids,
};
