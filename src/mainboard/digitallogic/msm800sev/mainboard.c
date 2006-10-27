#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <arch/io.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/lxdef.h>
#include "chip.h"

#define DIVIL_LBAR_GPIO		0x5140000c
static void init_gpio()
 {
	msr_t msr;
	printk_debug("Initializing GPIO module...\n");

	// initialize the GPIO LBAR
	msr.lo = GPIO_BASE;
	msr.hi = 0x0000f001;
	wrmsr(DIVIL_LBAR_GPIO, msr);
	msr = rdmsr(DIVIL_LBAR_GPIO);
	printk_debug("DIVIL_LBAR_GPIO set to 0x%08x 0x%08x\n", msr.hi, msr.lo);
}


static void init(struct device *dev)
{
	unsigned bus = 0;
	unsigned devNic = PCI_DEVFN(0xd, 0);	
	unsigned devUsb = PCI_DEVFN(0xf, 4);
	device_t usb = NULL, nic = NULL;
	unsigned char irqUsb = 0xa, irqNic = 0xb;

	printk_debug("DIGITALLOGIC MSM800SSEV  ENTER %s\n", __FUNCTION__);

#if 0
	// FIXME: do we need to initialize USB OHCI this way?
	printk_debug("%s (%x,%x) set USB PCI interrupt line to %d\n", 
		__FUNCTION__, bus, devUsb, irqUsb);

	// initialize the USB controller
	usb = dev_find_slot(bus, devUsb);
	if (!usb) printk_err("Could not find USB\n");
	else pci_write_config8(usb, PCI_INTERRUPT_LINE, irqUsb);

	printk_debug("%s (%x,%x) set NIC PCI interrupt line to %d\n", 
		__FUNCTION__, bus, devNic, irqNic);

	// initialize theEEPRO 100
	nic = dev_find_slot(bus, devNic);
	if (!nic) printk_err("Could not find USB\n");
	else pci_write_config8(nic, PCI_INTERRUPT_LINE, irqNic);
#endif
	init_gpio();

	printk_debug("DIGITALLOGIC MSM800SSEV EXIT %s\n", __FUNCTION__);
}

static void enable_dev(struct device *dev)
{
        dev->ops->init = init;
}

struct chip_operations mainboard_digitallogic_msm800sev_ops = {
	CHIP_NAME("DIGITAL-LOGIC MSM800SEV mainboard")
        .enable_dev = enable_dev,
};

