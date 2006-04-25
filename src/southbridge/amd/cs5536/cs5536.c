
#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <console/console.h>
#include <cpu/amd/gx2def.h>
#include <cpu/x86/msr.h>
#include "chip.h"

static void southbridge_init(struct device *dev)
{
	printk_err("cs5536: %s\n", __FUNCTION__);
	setup_i8259(); 
}

static void southbridge_enable(struct device *dev)
{
	struct southbridge_amd_cs5536_config  *sb = (struct southbridge_amd_cs5536_config *)dev->chip_info;
	msr_t msr;
	/*
	 * struct device *gpiodev;
	 * unsigned short gpiobase = MDD_GPIO;
	 */

	printk_err("%s: dev is %p\n", __FUNCTION__, dev);
	if (sb->lpc_serirq_enable) {
		msr.lo = sb->lpc_serirq_enable;
		msr.hi  = 0;
		wrmsr(MDD_LPC_SIRQ, msr);
	}
	if (sb->lpc_irq) {
		msr.lo = sb->lpc_irq;
		msr.hi = 0;
		wrmsr(MDD_IRQM_LPC, msr);
	}

	if (sb->enable_gpio0_inta){
	  msr = rdmsr(MDD_IRQM_ZHIGH);
		msr.lo |= 0x10;
		wrmsr(MDD_IRQM_ZHIGH, msr);
		/* todo: look the device up. But we know that gpiobase is 0x6100 */
		/* oh gosh, all the defines from AMD assume 6100. Don't bother looking up! */
		outl(GPIOL_0_SET|GPIOL_1_SET|GPIOL_3_SET, GPIOL_INPUT_ENABLE);
		outl(GPIOL_0_SET,GPIOL_EVENTS_ENABLE);
		/* magic stuff */
		outl(0x3081, GPIOL_INPUT_INVERT_ENABLE);
		outl(GPIOL_0_SET, GPIO_MAPPER_X);
	}
		
	
}

static void cs5536_pci_dev_enable_resources(device_t dev)
{
	printk_err("cs5536.c: %s()\n", __FUNCTION__);
	pci_dev_enable_resources(dev);
	enable_childrens_resources(dev);
}

static struct device_operations southbridge_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = cs5536_pci_dev_enable_resources,
	.init             = southbridge_init,
	.enable           = southbridge_enable,
	.scan_bus         = scan_static_bus,
};

static struct pci_driver cs5536_pci_driver __pci_driver = {
	.ops 	= &southbridge_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_CS5536_ISA
};

struct chip_operations southbridge_amd_cs5536_ops = {
	CHIP_NAME("AMD cs5536")
	/* This only called when this device is listed in the 
	* static device tree.
	*/
	.enable_dev = southbridge_enable,
};
