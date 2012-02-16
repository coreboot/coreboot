#include <console/console.h>
#include <arch/smp/mpspec.h>
#include <arch/ioapic.h>
#include <device/pci.h>
#include <string.h>
#include <stdint.h>

static void *smp_write_config_table(void *v)
{
	struct mp_config_table *mc;
	int bus_isa;
	unsigned char bus_pxhd_1;
	unsigned char bus_pxhd_2;
	unsigned char bus_esb6300_1;
	unsigned char bus_esb6300_2;

	mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);

	mptable_init(mc, LOCAL_APIC_ADDR);

	smp_write_processors(mc);

	{
		device_t dev;

		/* esb6300_2 */
		dev = dev_find_slot(0, PCI_DEVFN(0x1c,0));
		if (dev) {
			bus_esb6300_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
		} else {
			printk(BIOS_DEBUG, "ERROR - could not find PCI 0:1c.0, using defaults\n");
			bus_esb6300_1 = 6;
		}
		/* esb6300_1 */
		dev = dev_find_slot(0, PCI_DEVFN(0x1e,0));
		if (dev) {
			bus_esb6300_2 = pci_read_config8(dev, PCI_SECONDARY_BUS);
		} else {
			printk(BIOS_DEBUG, "ERROR - could not find PCI 0:1e.0, using defaults\n");
			bus_esb6300_2 = 7;
		}
		/* pxhd-1 */
		dev = dev_find_slot(1, PCI_DEVFN(0x0,0));
		if (dev) {
			bus_pxhd_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
		} else {
			printk(BIOS_DEBUG, "ERROR - could not find PCI 1:00.1, using defaults\n");
			bus_pxhd_1 = 2;
		}
		/* pxhd-2 */
		dev = dev_find_slot(1, PCI_DEVFN(0x00,2));
		if (dev) {
			bus_pxhd_2 = pci_read_config8(dev, PCI_SECONDARY_BUS);
		} else {
			printk(BIOS_DEBUG, "ERROR - could not find PCI 1:02.0, using defaults\n");
			bus_pxhd_2 = 3;
		}
	}

	mptable_write_buses(mc, NULL, &bus_isa);

	/* IOAPIC handling */

	smp_write_ioapic(mc, 2, 0x20, IO_APIC_ADDR);
	smp_write_ioapic(mc, 3, 0x20, IO_APIC_ADDR + 0x10000);
	{
	    	struct resource *res;
		device_t dev;
		/* PXHd apic 4 */
		dev = dev_find_slot(1, PCI_DEVFN(0x00,1));
		if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_0);
			if (res) {
				smp_write_ioapic(mc, 0x04, 0x20, res->base);
			}
		} else {
			printk(BIOS_DEBUG, "ERROR - could not find IOAPIC PCI 1:00.1\n");
			printk(BIOS_DEBUG, "CONFIG_DEBUG: Dev= %p\n", dev);
		}
		/* PXHd apic 5 */
		dev = dev_find_slot(1, PCI_DEVFN(0x00,3));
		if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_0);
			if (res) {
				smp_write_ioapic(mc, 0x05, 0x20, res->base);
			}
		} else {
			printk(BIOS_DEBUG, "ERROR - could not find IOAPIC PCI 1:00.3\n");
			printk(BIOS_DEBUG, "CONFIG_DEBUG: Dev= %p\n", dev);
		}
	}

	mptable_add_isa_interrupts(mc, bus_isa, 0x2, 0);

	/* ISA backward compatibility interrupts  */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x00, 0x74, 0x02, 0x10);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x00, 0x77, 0x02, 0x17);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x00, 0x75, 0x02, 0x13);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x00, 0x7c, 0x02, 0x12);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x00, 0x7d, 0x02, 0x11);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, // -- added
		0x03, 0x08, 0x05, 0x00);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, // -- added
		0x03, 0x08, 0x05, 0x04);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, // -- added
		bus_esb6300_1, 0x04, 0x03, 0x00);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, // -- added
		bus_esb6300_1, 0x08, 0x03, 0x01);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, // -- added
		bus_esb6300_2, 0x04, 0x02, 0x10);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, // -- added
		bus_esb6300_2, 0x08, 0x02, 0x14);

	/* Standard local interrupt assignments */
	mptable_lintsrc(mc, bus_isa);

	/* FIXME verify I have the irqs handled for all of the risers */

	/* Compute the checksums */
	return mptable_finalize(mc);
}

unsigned long write_smp_table(unsigned long addr)
{
	void *v;
	v = smp_write_floating_table(addr, 0);
	return (unsigned long)smp_write_config_table(v);
}

