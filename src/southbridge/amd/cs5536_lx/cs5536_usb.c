#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>

#include <cpu/amd/lxdef.h>
#include <cpu/x86/msr.h>

#include "cs5536.h"

#define GLIU2_P2D_BM_1			0x51010021					// southbridge GLIU P2D base mask descriptor
#define USBMSRUOCB				0x5120000b					// USB option controller base address register
#define UOCMUX					(USBOC_BASE_ADDRESS + 4)	// option controller native multiplex register

#define USBOC_BASE_ADDRESS		0xe1017000		// USB option contoller base address, used only here
#define USBOC_PORT4_FUNCTION	2				// USB port 4 desired function (0 = disabled, 2 = host, 3 = device)

static void ohci_init(struct device *dev)
{
	uint32_t n;
	msr_t msr;

	printk_debug("USB: Setting up OHCI controller... ");

	// enable controller bus mastering
	n = pci_read_config32(dev, PCI_COMMAND);
	pci_write_config32(dev, PCI_COMMAND, 
		n | PCI_COMMAND_IO | PCI_COMMAND_MEMORY | 
		PCI_COMMAND_MASTER | PCI_COMMAND_INVALIDATE);

	// set the USB option controller base address
	msr.hi = 0x0000000a;
	msr.lo = USBOC_BASE_ADDRESS;
	wrmsr(USBMSRUOCB, msr);

	// create the GLIU memory mapping
	msr.hi = 0x40000000 | ((USBOC_BASE_ADDRESS >> 24) & 0x000000ff);
	msr.lo = ((USBOC_BASE_ADDRESS << 8) & 0xfff00000) | 0x000fffff;
	wrmsr(GLIU2_P2D_BM_1, msr);

	// set the multiplex register with port 4 function
	*((uint32_t*)UOCMUX) = USBOC_PORT4_FUNCTION;

	// finally, disable the USB option controller memory mapping
	msr.hi = 0x000000ff;
	msr.lo = 0xfff00000;
	wrmsr(GLIU2_P2D_BM_1, msr);
    
	printk_debug("done.\n");
}

static void ehci_init(struct device *dev)
{
	uint32_t cmd;
	printk_debug("USB: Setting up EHCI controller... ");
	
	// enable controller bus mastering
	cmd = pci_read_config32(dev, PCI_COMMAND);
	pci_write_config32(dev, PCI_COMMAND, 
		cmd | PCI_COMMAND_IO | PCI_COMMAND_MEMORY | 
		PCI_COMMAND_MASTER | PCI_COMMAND_INVALIDATE);

	printk_debug("done.\n");
}

static struct device_operations ohci_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = ohci_init,
	.scan_bus         = 0,
	.enable           = southbridge_enable,
};

static struct device_operations ehci_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = ehci_init,
	.scan_bus         = 0,
	.enable           = southbridge_enable,
};

static struct pci_driver cs5536_usb1_driver __pci_driver = {
	.ops    = &ohci_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_CS5536_OHCI
};

static struct pci_driver cs5536_usb2_driver __pci_driver = {
	.ops    = &ehci_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_CS5536_EHCI
};
