#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <arch/io.h>
#include "chip.h"


static void init(struct device *dev)
{
	unsigned bus = 0;
	unsigned devNic = PCI_DEVFN(0xd, 0);	
	unsigned devUsb = PCI_DEVFN(0xf, 4);
	device_t usb = NULL, nic = NULL;
	unsigned char irqUsb = 0xa, irqNic = 0xb;

	printk_debug("ARTECGROUP DBE61 ENTER %s\n", __FUNCTION__);

	// FIXME: do we need to initialize USB OHCI this way?
	printk_debug("%s (%x,%x) set USB PCI interrupt line to %d\n", 
		__FUNCTION__, bus, devUsb, irqUsb);

	// initialize the USB controller
	usb = dev_find_slot(bus, devUsb);
	if (!usb) printk_err("Could not find USB\n");
	else pci_write_config8(usb, PCI_INTERRUPT_LINE, irqUsb);

	printk_debug("%s (%x,%x) set NIC PCI interrupt line to %d\n", 
		__FUNCTION__, bus, devNic, irqNic);

	// initialize the Realtek NIC
	nic = dev_find_slot(bus, devNic);
	if (!nic) printk_err("Could not find USB\n");
	else pci_write_config8(nic, PCI_INTERRUPT_LINE, irqNic);

	printk_debug("ARTECGROUP DBE61 EXIT %s\n", __FUNCTION__);
}

static void enable_dev(struct device *dev)
{
        dev->ops->init = init;
}

struct chip_operations mainboard_artecgroup_dbe61_ops = {
	CHIP_NAME("artecgroup dbe61 mainboard ")
        .enable_dev = enable_dev,

};
