/*
 * (C) 2003 Linux Networx 
 */
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>


struct ioapicreg {
	unsigned int reg;
	unsigned int value_low, value_high;
};

static struct ioapicreg ioapicregvalues[] = {
#define ALL		(0xff << 24)
#define NONE		(0)
#define DISABLED	(1 << 16)
#define ENABLED		(0 << 16)
#define TRIGGER_EDGE	(0 << 15)
#define TRIGGER_LEVEL	(1 << 15)
#define POLARITY_HIGH	(0 << 13)
#define POLARITY_LOW	(1 << 13)
#define PHYSICAL_DEST	(0 << 11)
#define LOGICAL_DEST	(1 << 11)
#define ExtINT		(7 << 8)
#define NMI		(4 << 8)
#define SMI		(2 << 8)
#define INT		(1 << 8)
	/* mask, trigger, polarity, destination, delivery, vector */
	{0x00, ENABLED | TRIGGER_EDGE | POLARITY_HIGH | PHYSICAL_DEST | ExtINT | 0, 0},
	{0x01, DISABLED, NONE},
	{0x02, ENABLED | TRIGGER_EDGE | POLARITY_HIGH | PHYSICAL_DEST | INT | 0,  0},
	{0x03, DISABLED, NONE},
	{0x04, DISABLED, NONE},
	{0x05, DISABLED, NONE},
	{0x06, DISABLED, NONE},
	{0x07, DISABLED, NONE},
	{0x08, DISABLED, NONE},
	{0x09, DISABLED, NONE},
	{0x0a, DISABLED, NONE},
	{0x0b, DISABLED, NONE},
	{0x0c, DISABLED, NONE},
	{0x0d, DISABLED, NONE},
	{0x0e, DISABLED, NONE},
	{0x0f, DISABLED, NONE},
	{0x10, DISABLED, NONE},
	{0x11, DISABLED, NONE},
	{0x12, DISABLED, NONE},
	{0x13, DISABLED, NONE},
	{0x14, DISABLED, NONE},
	{0x14, DISABLED, NONE},
	{0x15, DISABLED, NONE},
	{0x16, DISABLED, NONE},
	{0x17, DISABLED, NONE},
	{0x18, DISABLED, NONE},
	{0x19, DISABLED, NONE},
	{0x20, DISABLED, NONE},
	{0x21, DISABLED, NONE},
	{0x22, DISABLED, NONE},
	{0x23, DISABLED, NONE},
};

static void setup_ioapic(void)
{
	int i;
	unsigned long value_low, value_high;
	unsigned long ioapic_base = 0xfec00000;
	volatile unsigned long *l;
	struct ioapicreg *a = ioapicregvalues;

	l = (unsigned long *) ioapic_base;
	for (i = 0; i < sizeof(ioapicregvalues) / sizeof(ioapicregvalues[0]);
	     i++, a++) {
		l[0] = (a->reg * 2) + 0x10;
		l[4] = a->value_low;
		value_low = l[4];
		l[0] = (a->reg *2) + 0x11;
		l[4] = a->value_high;
		value_high = l[4];
		if ((i==0) && (value_low == 0xffffffff)) {
			printk_warning("IO APIC not responding.\n");
			return;
		}
		printk_spew("for IRQ, reg 0x%08x value 0x%08x 0x%08x\n", 
			a->reg, a->value_low, a->value_high);
	}
}

static void lpc_init(struct device *dev)
{
	uint8_t byte;
	int pwr_on=-1;

	printk_debug("lpc_init\n");

#if 0
	/* IO APIC initialization */
	byte = pci_read_config8(dev, 0x4B);
	byte |= 1;
	pci_write_config8(dev, 0x4B, byte);
	setup_ioapic();
#endif

	/* posted memory write enable */
	byte = pci_read_config8(dev, 0x46);
	pci_write_config8(dev, 0x46, byte | (1<<0));

	/* power after power fail */
	byte = pci_read_config8(dev, 0x43);
	if (pwr_on) { 
		byte &= ~(1<<6);
	} else {
		byte |= (1<<6);
	}
	pci_write_config8(dev, 0x43, byte);


}

static struct device_operations lpc_ops  = {
	.read_resources = pci_dev_read_resources,
	.set_resources  = pci_dev_set_resources,
	.init = lpc_init,
	.scan_bus = 0,
};

static struct pci_driver lpc_driver __pci_driver = {
	.ops    = &lpc_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_8111_ISA,
};
