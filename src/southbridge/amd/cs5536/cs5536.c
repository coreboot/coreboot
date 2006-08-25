
#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <console/console.h>
#include <cpu/amd/gx2def.h>
#include <cpu/x86/msr.h>
#include "chip.h"


#define PIN_OPT_IDE		(1ULL<<0)	/* 0 for flash, 1 for IDE */

/* Intended value for LBAR_FLSH0: 4KiB, enabled, MMIO, NAND, @0x20000000 */
/* NOTE: no longer used, prune at some point */
/* OOPS: steve's changes don't work, so we have to keep this */
msr_t flsh0 = { .hi=0xFFFFF007, .lo=0x20000000};

static void
enable_ide_nand_flash(){
	msr_t msr;
	printk_err("cs5536: %s\n", __FUNCTION__);
#if 1
	printk_err("WARNING: using deprecated flash enable mechanism\n");
	/* steve took this one out ... not sure if needed or not */
	msr = rdmsr(MDD_LBAR_FLSH0);

	if ( ((msr.hi) & 7) != 7) {
		printk_err("MDD_LBAR_FLSH0 was 0x%08x%08x\n", msr.hi,msr.lo);
		wrmsr(MDD_LBAR_FLSH0, flsh0);
	}
	msr = rdmsr(MDD_LBAR_FLSH0);
	printk_err("MDD_LBAR_FLSH0 is 0x%08x%08x\n", msr.hi,msr.lo);
#endif
	msr = rdmsr(MDD_PIN_OPT);
	if (msr.lo & PIN_OPT_IDE) {
		printk_err("MDD_PIN_OPT was 0x%08x%08x\n", msr.hi,msr.lo);
		msr.lo &= ~PIN_OPT_IDE;
		wrmsr(MDD_PIN_OPT, msr);
	}
	msr = rdmsr(MDD_PIN_OPT);
	printk_err("MDD_PIN_OPT is 0x%08x%08x\n", msr.hi,msr.lo);

	msr = rdmsr(MDD_NANDF_DATA);
	if (msr.lo != 0x00100010) {
		printk_err("MDD_NANDF_DATA was 0x%08x%08x\n", msr.hi,msr.lo);
		msr.lo = 0x00100010;
		wrmsr(MDD_NANDF_DATA, msr);
	}
	msr = rdmsr(MDD_NANDF_DATA);
	printk_err("MDD_NANDF_DATA is 0x%08x%08x\n", msr.hi,msr.lo);

	msr = rdmsr(MDD_NADF_CNTL);
	if (msr.lo != 0x0010) {
		printk_err("MDD_NADF_CNTL was 0x%08x%08x\n", msr.hi,msr.lo);
		msr.lo = 0x0010;
		wrmsr(MDD_NADF_CNTL, msr);
	}
	msr = rdmsr(MDD_NADF_CNTL);
	printk_err("MDD_NADF_CNTL is 0x%08x%08x\n", msr.hi,msr.lo);
	printk_err("cs5536: EXIT %s\n", __FUNCTION__);
}

#if 0
/* note: this is a candidate for inclusion in src/devices/pci_device.c */
void
setup_irq(unsigned irq, char *name, unsigned level, unsigned bus, unsigned device, unsigned fn){
	if (irq)  {
		unsigned devfn = PCI_DEVFN(device,fn);
		device_t dev  = dev_find_slot(bus, devfn);
		if (dev) {
			pci_write_config8(dev, PCI_INTERRUPT_LINE, irq);
			if (level)
				pci_level_irq(irq);
		}
		else
			printk_err("%s: Can't find %s at 0x%x\n", __FUNCTION__, name, devfn);
	}
}
#endif

static void southbridge_init(struct device *dev)
{
	struct southbridge_amd_cs5536_config  *sb = (struct southbridge_amd_cs5536_config *)dev->chip_info;
	const unsigned char slots_cpu[4] = {11, 0, 0, 0};
	const unsigned char slots_sb[4] = {11, 5, 10, 10};
 	msr_t msr;
	int i;
	/*
	 * struct device *gpiodev;
	 * unsigned short gpiobase = MDD_GPIO;
	 */

	printk_err("cs5536: %s\n", __FUNCTION__);
	setup_i8259(); 

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

	if (sb->enable_uarta){
		printk_err("cs5536: %s: enable uarta, msr MDD_IRQM_YHIGH(%x) \n",
				 __FUNCTION__, MDD_IRQM_YHIGH);
		msr = rdmsr(MDD_IRQM_YHIGH);
		msr.lo |= 0x04000000;
		wrmsr(MDD_IRQM_YHIGH, msr);
	}

	printk_err("cs5536: %s: enable_ide_nand_flash is %d\n", __FUNCTION__, sb->enable_ide_nand_flash);
	if (sb->enable_ide_nand_flash) {
		enable_ide_nand_flash();
	}

#if 0
 	/* irq handling */
 	setup_irq(sb->audio_irq, "audio", 1, 0, 0xf, 2);
 	setup_irq(sb->usbf4_irq, "usb f4", 1, 0, 0xf, 4);
 	setup_irq(sb->usbf5_irq, "usb f5", 1, 0, 0xf, 5);
 	setup_irq(sb->usbf6_irq, "usb f6", 1, 0, 0xf, 6);
 	setup_irq(sb->usbf7_irq, "usb f7", 1, 0, 0xf, 7);
#else
	/* CPU (80000800 = 00.01.00) */
	pci_assign_irqs(0, 0x01, slots_cpu);	/* bus=0, device=0x01, slots={11,0,0,0} */
 	
	/* Southbridge (80007800 = 00.0F.00) */
	pci_assign_irqs(0, 0x0F, slots_sb);	/* bus=0, device=0x0F, slots={11,5,10,10} */
#endif
	/* disable unwanted virtual PCI devices */
	for (i = 0; (i < MAX_UNWANTED_VPCI) && (0 != sb->unwanted_vpci[i]); i++) {
		printk_debug("Disabling VPCI device: 0x%08X\n", sb->unwanted_vpci[i]);
		outl(sb->unwanted_vpci[i] + 0x7C, 0xCF8);
		outl(0xDEADBEEF,                  0xCFC);
	}

	if (sb->enable_USBP4_host) {
		volatile unsigned long* uocmux;
		unsigned long val;


		printk_err("DES 0x%08x\n",MSR_SB_USB2_MEM_DES);
		
		msr = rdmsr(MSR_SB_USB2_MEM_DES);
		printk_err("DES 0x%08x%08x\n", msr.hi,msr.lo);

		msr.hi = 0x400000fe;
		msr.lo = 0x010fffff;

		wrmsr(MSR_SB_USB2_MEM_DES, msr);

		msr = rdmsr(MSR_SB_USB2_MEM_DES);
		printk_err("New DES 0x%08x%08x\n", msr.hi,msr.lo);

		msr = rdmsr(USB2_SB_GLD_MSR_UOC_BASE);
		printk_err("Old UOC Base 0x%08x%08x\n", msr.hi,msr.lo);
		msr.hi |= 0xa;
		msr.lo |= 0xfe010000;
	
		wrmsr(USB2_SB_GLD_MSR_UOC_BASE, msr);

		msr = rdmsr(USB2_SB_GLD_MSR_UOC_BASE);
		printk_err("New UOC Base 0x%08x%08x\n", msr.hi,msr.lo);

		uocmux = (unsigned long *)(msr.lo+4);
		val = *uocmux;

		printk_err("UOCMUX is 0x%lx\n",val);

		val &= ~(0x3);
		val |= 0x2;

		*uocmux = val;

		val = *uocmux;
		printk_err("New UOCMUX is 0x%lx\n",val);

	}

}


static void southbridge_enable(struct device *dev)
{
	printk_err("cs5536: %s: dev is %p\n", __FUNCTION__, dev);
 }

static void cs5536_pci_dev_enable_resources(device_t dev)
{
	printk_err("cs5536: %s()\n", __FUNCTION__);
	pci_dev_enable_resources(dev);
	enable_childrens_resources(dev);
}

static struct device_operations southbridge_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = cs5536_pci_dev_enable_resources,
	.init             = southbridge_init,
//	.enable           = southbridge_enable,
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
