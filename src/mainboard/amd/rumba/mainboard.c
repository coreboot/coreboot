#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <arch/io.h>
#include "chip.h"

static void init(struct device *dev) {
	struct mainboard_config  *mainboard = (struct mainboard_config*)dev->chip_info;
	device_t nic = NULL;
	unsigned bus = 0;
	unsigned devfn = PCI_DEVFN(0xd, 0);
	int nicirq = 1;

	if (mainboard->nicirq)
		nicirq = mainboard->nicirq;

	printk(BIOS_DEBUG, "AMD RUMBA ENTER %s\n", __func__);

	if (nicirq) {
		printk(BIOS_DEBUG, "%s (%x,%x)SET PCI interrupt line to %d\n",
			__func__, bus, devfn, nicirq);
		nic = dev_find_slot(bus, devfn);
		if (! nic){
			printk(BIOS_ERR, "Could not find NIC\n");
		} else {
			pci_write_config8(nic, PCI_INTERRUPT_LINE, nicirq);
		}
	}
	printk(BIOS_DEBUG, "AMD RUMBA EXIT %s\n", __func__);
}

static void enable_dev(struct device *dev)
{
        dev->ops->init = init;
}

struct chip_operations mainboard_ops = {
	CHIP_NAME("AMD Rumba Mainboard")
        .enable_dev = enable_dev,
};

