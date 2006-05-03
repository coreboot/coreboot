#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <arch/io.h>
#include "chip.h"

static void init(struct device *dev) {
	struct mainboard_amd_rumba_config  *mainboard = (struct mainboard_amd_rumba_config*)dev->chip_info;
	device_t nic = NULL;
	unsigned bus = 0;
	unsigned devfn = PCI_DEVFN(0xd, 0);
	int nicirq = 1;

	if (mainboard->nicirq)
		nicirq = mainboard->nicirq;

	printk_debug("AMD RUMBA ENTER %s\n", __FUNCTION__);

	if (nicirq) {
		printk_debug("%s (%x,%x)SET PCI interrupt line to %d\n", 
			__FUNCTION__, bus, devfn, nicirq);
		nic = dev_find_slot(bus, devfn);
		if (! nic){
			printk_err("Could not find NIC\n");
		} else {
			pci_write_config8(nic, PCI_INTERRUPT_LINE, nicirq);
		}
	}
	printk_debug("AMD RUMBA EXIT %s\n", __FUNCTION__);
}

static void enable_dev(struct device *dev)
{
        dev->ops->init = init;
}

struct chip_operations mainboard_amd_rumba_ops = {
	CHIP_NAME("AMD Rumba mainboard ")
        .enable_dev = enable_dev,
};

