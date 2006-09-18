
#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <console/console.h>
#include <cpu/amd/lxdef.h>
#include <cpu/x86/msr.h>
#include "chip.h"

// prototypes here, avoid warnings
void setup_i8259(void);
void pci_assign_irqs(unsigned bus, unsigned slot, const unsigned char pIntAtoD[4]);
void print_conf(void);

#define PIN_OPT_IDE		(1ULL<<0)	/* 0 for flash, 1 for IDE */

/* Intended value for LBAR_FLSH0: 4KiB, enabled, MMIO, NAND, @0x20000000 */
/* NOTE: no longer used, prune at some point */
/* OOPS: steve's changes don't work, so we have to keep this */
msr_t flsh1 = { .hi=0xFFFFF007, .lo=0x20000000};

// ide is ENABLED by default (in early init), this disables it if neccesary
// and enables flash
static void enable_ide_nand_flash()
{
	msr_t msr;
	printk_debug("cs5536: %s\n", __FUNCTION__);
#if 1
	printk_err("WARNING: using deprecated flash enable mechanism\n");
	/* steve took this one out ... not sure if needed or not */
	msr = rdmsr(MDD_LBAR_FLSH1);

	if ( ((msr.hi) & 7) != 7) {
		printk_debug("MDD_LBAR_FLSH1 was 0x%08x%08x\n", msr.hi,msr.lo);
		wrmsr(MDD_LBAR_FLSH1, flsh1);
	}
	msr = rdmsr(MDD_LBAR_FLSH1);
	printk_debug("MDD_LBAR_FLSH1 is 0x%08x%08x\n", msr.hi,msr.lo);
#endif
	msr = rdmsr(MDD_PIN_OPT);
	if (msr.lo & PIN_OPT_IDE) {
		printk_debug("MDD_PIN_OPT was 0x%08x%08x\n", msr.hi,msr.lo);
		msr.lo &= ~PIN_OPT_IDE;
		wrmsr(MDD_PIN_OPT, msr);
	}
	msr = rdmsr(MDD_PIN_OPT);
	printk_debug("MDD_PIN_OPT is 0x%08x%08x\n", msr.hi,msr.lo);

	msr = rdmsr(MDD_NANDF_DATA);
	if (msr.lo != 0x00100010) {
		printk_debug("MDD_NANDF_DATA was 0x%08x%08x\n", msr.hi,msr.lo);
		msr.lo = 0x00100010;
		wrmsr(MDD_NANDF_DATA, msr);
	}
	msr = rdmsr(MDD_NANDF_DATA);
	printk_debug("MDD_NANDF_DATA is 0x%08x%08x\n", msr.hi,msr.lo);

	msr = rdmsr(MDD_NADF_CNTL);
	if (msr.lo != 0x0010) {
		printk_debug("MDD_NADF_CNTL was 0x%08x%08x\n", msr.hi,msr.lo);
		msr.lo = 0x0010;
		wrmsr(MDD_NADF_CNTL, msr);
	}
	msr = rdmsr(MDD_NADF_CNTL);
	printk_debug("MDD_NADF_CNTL is 0x%08x%08x\n", msr.hi,msr.lo);
	printk_debug("cs5536: EXIT %s\n", __FUNCTION__);
}

/* note: this is a candidate for inclusion in src/devices/pci_device.c */
// set pci headers to indicate correct IRQ number of the device
void setup_irq(unsigned irq, unsigned level, unsigned bus, unsigned device, unsigned fn)
{
	if (irq)
	{
		unsigned devfn = PCI_DEVFN(device,fn);
		device_t dev  = dev_find_slot(bus, devfn);
		if (dev) 
		{
			printk_debug("%s: assigning IRQ %d to %x.%x (0x%x)\n", 
				__FUNCTION__, irq, device, fn, devfn);

			pci_write_config8(dev, PCI_INTERRUPT_LINE, irq);
			if (level) pci_level_irq(irq);
		}
		else
			printk_err("%s: Can't find PCI device function 0x%x\n", __FUNCTION__, devfn);
	}
}

// map an Y or Z irq source in programmable irq controller 
// dev 0..15 Y, 16...31 Z
void map_pic_irq(unsigned int dev, unsigned int irq){
	msr_t msr;
	unsigned long mask;
	unsigned long val;
	
	
	mask = ~(0xF<<((dev&7)<<2));
	val = irq<<((dev&7)<<2);
	
	printk_debug("%s: mapping %d to src %d: mask %x val %x\n", 
				__FUNCTION__, irq, dev,mask,val);
	
	switch (dev & 0x18){
		case 0:
			msr=rdmsr(MDD_IRQM_YLOW);
			msr.lo = (msr.lo & mask) | val;
			wrmsr(MDD_IRQM_YLOW,msr);
			break;
		case 8:
			msr=rdmsr(MDD_IRQM_YHIGH);
			msr.lo = (msr.lo & mask) | val;
			wrmsr(MDD_IRQM_YHIGH,msr);
			break;
		case 16:
			msr=rdmsr(MDD_IRQM_ZLOW);
			msr.lo = (msr.lo & mask) | val;
			wrmsr(MDD_IRQM_ZLOW,msr);
			break;
		case 24:
			msr=rdmsr(MDD_IRQM_ZHIGH);
			msr.lo = (msr.lo & mask) | val;
			wrmsr(MDD_IRQM_ZHIGH,msr);
	}
}

// map an GPIO pin to PIC Z source and that to an IRQ.
void map_gpio_irq(unsigned int pin, unsigned int gpioirq, unsigned int irq, unsigned int invert){
	
	unsigned long temp;
	unsigned long mask;
	unsigned long val;

	mask = ~(0xF<<((pin&7)<<2));
	val = gpioirq<<((pin&7)<<2);
	
	// configure GPIO pin as INT source
	outl(1<<(pin&0x0F), (pin<16)?GPIOL_INPUT_ENABLE:GPIOH_INPUT_ENABLE);
	outl(1<<(pin&0x0F), (pin<16)?GPIOL_EVENTS_ENABLE:GPIOH_EVENTS_ENABLE);

	if (invert) outl(1<<(pin&0x0F), (pin<16)?GPIOL_INPUT_INVERT_ENABLE:GPIOH_INPUT_INVERT_ENABLE);

	// map current GPIO pin to PIC unrestricted Z GPIO irq source gpioirq
	temp = inl((pin<16)?
			((pin<8)?GPIO_MAPPER_X:GPIO_MAPPER_Y):
			((pin<24)?GPIO_MAPPER_Z:GPIO_MAPPER_W));

	outl((temp & mask)|val, 
		(pin<16)?
			((pin<8)?GPIO_MAPPER_X:GPIO_MAPPER_Y):
			((pin<24)?GPIO_MAPPER_Z:GPIO_MAPPER_W));
			
	// map PIC unrestricted Z GPIO source i to IRQ needed
	map_pic_irq(gpioirq+24,irq);
}

static void southbridge_init(struct device *dev)
{
	struct southbridge_amd_cs5536_config  *sb = (struct southbridge_amd_cs5536_config *)dev->chip_info;
 	msr_t msr;
	int i;

	printk_spew(">> Entering cs5536.c: %s\n", __FUNCTION__);

	/*
	 * struct device *gpiodev;
	 * unsigned short gpiobase = MDD_GPIO;
	 */

	setup_i8259(); 

	// at first, disable all primary IRQ inputs except timer, RTC and FPU
	msr.lo=0x2101;
	msr.hi=0;
	wrmsr(MDD_IRQM_PRIM, msr);

	// LPC bus IRQs are enabled here
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

	// GPIO to IRQ mapping (meant for PCI IRQs)
	//  here, only physical mapping is done. 
	//  pci headers should be configured for all pci slots available in mainboard.c
	//  so that operating system can read correct irq number from there
	for (i=0; i<3; i++){
		if (sb->pci_int[i]){
			printk_debug("cs5536: %s: Configuring PCI INT%c from GPIO %d to INT %d.\n",
				 __FUNCTION__, 'A'+i, sb->pci_int_pin[i], sb->pci_int[i]);
			map_gpio_irq(sb->pci_int_pin[i], i, sb->pci_int[i], 1);
		}
	}
/*	
// PCI IRQ mapping should work like this through vsa, but it doesn't
	outl(0xAC1C, 0xFC53); //magic word to enable hidden register
	outl(0xAC1C, 0x0009); //command: PCI_INT_AB
	outl(0xAC1E, 
		(sb->pci_int[0]>0)?(sb->pci_int_pin[0]):0x21 |
		(sb->pci_int[1]>0)?(sb->pci_int_pin[1]<<8):(0x21<<8)
	);

	outl(0x785C, sb->pci_int[0]|(sb->pci_int[1]<<8));
*/

	printk_debug("cs5536: %s: enable_ide_nand_flash is %d\n", __FUNCTION__, sb->enable_ide_nand_flash);
	if (sb->enable_ide_nand_flash) {
		enable_ide_nand_flash();
	}

 	/* irq handling */
 	// setup_irq sets irq in pci headers only, no configuration is done.
 	// pci headers must be set, operating system reads IRQ numbers there.
 	// to disable, set IRQ=0
 	
	if (sb->isa_irq){
	 	setup_irq(sb->isa_irq, 1, 0, 0xf, 0);
	}

	if (sb->flash_irq){
	 	setup_irq(sb->flash_irq, 1, 0, 0xf, 1);
	 	map_pic_irq(6,sb->flash_irq);
	 	map_pic_irq(7,sb->flash_irq);	 	
	}

	// IDE IRQ 14 (GPIO pin 2) can only be enabled or disabled
	if (sb->enable_ide_irq){
	 	setup_irq(14, 1, 0, 0xf, 2);

		// set up IDE GPIO IRQ pin
		outl(1<<2, GPIOL_INPUT_ENABLE);
		outl(1<<2, GPIOL_IN_AUX1_SELECT);

		// enable IDE IRQ in primary IRQ mask
		msr=rdmsr(MDD_IRQM_PRIM);
		msr.lo |= 0x4000;
		wrmsr(MDD_IRQM_PRIM, msr);
	}

	if (sb->audio_irq){ 	
	 	map_pic_irq(4,sb->audio_irq);
	 	setup_irq(sb->audio_irq, 1, 0, 0xf, 3);
	}

	if (sb->uart0_irq){ 	
	  	map_pic_irq(14,sb->uart0_irq);
	}	
	if (sb->uart1_irq){ 	
	  	map_pic_irq(15,sb->uart1_irq);
	}

	if (sb->usb_irq){ 	
	 	setup_irq(sb->usb_irq, 1, 0, 0xf, 4);
	 	setup_irq(sb->usb_irq, 1, 0, 0xf, 5);
	 	setup_irq(sb->usb_irq, 1, 0, 0xf, 6);
	 	setup_irq(sb->usb_irq, 1, 0, 0xf, 7);
	  	map_pic_irq(2,sb->usb_irq);
	}

	// KEL (Keyboard Emulation Logic) IRQs
	if (sb->enable_kel_keyb_irq){
		msr=rdmsr(MDD_IRQM_PRIM);
		msr.lo |= 0x0002;
		wrmsr(MDD_IRQM_PRIM, msr);
	}

	if (sb->enable_kel_mouse_irq){
		msr=rdmsr(MDD_IRQM_PRIM);
		msr.lo |= 0x1000;
		wrmsr(MDD_IRQM_PRIM, msr);
	}

	if (sb->kel_emul_irq){
	 	map_pic_irq(13,sb->kel_emul_irq);	 	
	}




	/* disable unwanted virtual PCI devices */
	for (i = 0; (i < MAX_UNWANTED_VPCI) && (0 != sb->unwanted_vpci[i]); i++) {
		printk_debug("Disabling VPCI device: 0x%08X\n", sb->unwanted_vpci[i]);
		outl(sb->unwanted_vpci[i] + 0x7C, 0xCF8);
		outl(0xDEADBEEF,                  0xCFC);
	}

	/* What do we have there? */
	printk_debug("\nChipset config after southbridge_init():\n");
	print_conf();
}

void southbridge_enable(struct device *dev)
{
}

static void cs5536_pci_dev_enable_resources(device_t dev)
{
	printk_spew(">> Entering cs5536.c: %s\n", __FUNCTION__);
	pci_dev_enable_resources(dev);
	enable_childrens_resources(dev);
}

static struct device_operations southbridge_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = cs5536_pci_dev_enable_resources,
	.init             = southbridge_init,
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
