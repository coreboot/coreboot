#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <arch/io.h>
#include "chip.h"



static void init(struct device *dev) {
/*
	unsigned bus = 0;
	unsigned devfn = PCI_DEVFN(0xf, 4);
	device_t usb = NULL;
	unsigned char usbirq = 0xa;
*/

	printk_debug("OLPC REVA ENTER %s\n", __FUNCTION__);

#if 0
	/* I can't think of any reason NOT to just set this. If it turns out we want this to be
	  * conditional we can make it a config variable later.
	  */

	printk_debug("%s (%x,%x)SET USB PCI interrupt line to %d\n", 
		__FUNCTION__, bus, devfn, usbirq);
	usb = dev_find_slot(bus, devfn);
	if (! usb){
		printk_err("Could not find USB\n");
	} else {
		pci_write_config8(usb, PCI_INTERRUPT_LINE, usbirq);
	}
#endif
	printk_debug("OLPC REVA EXIT %s\n", __FUNCTION__);
}

static void enable_dev(struct device *dev)
{
        dev->ops->init = init;
}

struct chip_operations mainboard_olpc_rev_a_ops = {
	CHIP_NAME("olpc rev_a mainboard ")
        .enable_dev = enable_dev,

};
