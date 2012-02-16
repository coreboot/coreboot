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
	unsigned char bus_8111_0;
	unsigned char bus_8111_1;
	unsigned char bus_8131_1;
	unsigned char bus_8131_2;

	mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);

	mptable_init(mc, LOCAL_APIC_ADDR);

	smp_write_processors(mc);

	{
		device_t dev;

		/* 8111 */
		dev = dev_find_slot(1, PCI_DEVFN(0x03,0));
		if (dev) {
                	bus_8111_0 = pci_read_config8(dev, PCI_PRIMARY_BUS);
			bus_8111_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
		} else {
			printk(BIOS_DEBUG, "ERROR - could not find PCI 1:03.0, using defaults\n");
			bus_8111_0 = 1;
			bus_8111_1 = 4;
		}

		/* 8131-1 */
		dev = dev_find_slot(1, PCI_DEVFN(0x01,0));
		if (dev) {
			bus_8131_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);

		} else {
			printk(BIOS_DEBUG, "ERROR - could not find PCI 1:01.0, using defaults\n");
			bus_8131_1 = 2;
		}

		/* 8131-2 */
		dev = dev_find_slot(1, PCI_DEVFN(0x02,0));
		if (dev) {
			bus_8131_2 = pci_read_config8(dev, PCI_SECONDARY_BUS);
		} else {
			printk(BIOS_DEBUG, "ERROR - could not find PCI 1:02.0, using defaults\n");
			bus_8131_2 = 3;
		}
	}

	mptable_write_buses(mc, NULL, &bus_isa);

	/* Legacy IOAPIC #2 */
	smp_write_ioapic(mc, 2, 0x11, IO_APIC_ADDR);
	{
		device_t dev;
		struct resource *res;
		/* 8131-1 apic #3 */
		dev = dev_find_slot(1, PCI_DEVFN(0x01,1));
		if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_0);
			if (res) {
				smp_write_ioapic(mc, 0x03, 0x11, res->base);
			}
		}
		/* 8131-2 apic #4 */
		dev = dev_find_slot(1, PCI_DEVFN(0x02,1));
		if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_0);
			if (res) {
				smp_write_ioapic(mc, 0x04, 0x11, res->base);
			}
		}
	}

	mptable_add_isa_interrupts(mc, bus_isa, 0x2, 0);

	/* PCI Ints:	     Type	Polarity    	    Trigger		  	Bus ID      PCIDEVNUM|IRQ  APIC ID PIN# */
	/* Integrated SMBus 2.0 */
        smp_write_intsrc(mc, mp_INT,	MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8111_0, (0x04<<2)|3, 0x2, 0x13);
	/* Integrated AMD AC97 Audio */
        smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8111_0, (0x04<<2)|1, 0x2, 0x11);

	/* Integrated AMD USB */
        smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8111_1, (0x00<<2)|3, 0x2, 0x13);

	/* On board ATI Rage XL */
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8111_1, (0x05<<2)|0, 0x2, 0x10);

	/* On board Broadcom nics */
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8131_1, (0x01<<2)|0, 0x3, 0x00);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8131_1, (0x01<<2)|1, 0x3, 0x01);

	/* On board LSI SCSI */
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8131_1, (0x02<<2)|0, 0x3, 0x02);

	/* PCI Slot 1 PCIX */
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8131_2, (0x03<<2)|0, 0x2, 0x10);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8131_2, (0x03<<2)|1, 0x2, 0x11);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8131_2, (0x03<<2)|2, 0x2, 0x12);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8131_2, (0x03<<2)|3, 0x2, 0x13);

	/* PCI Slot 2 PCIX */
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8131_2, (0x04<<2)|0, 0x2, 0x11);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8131_2, (0x04<<2)|1, 0x2, 0x12);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8131_2, (0x04<<2)|2, 0x2, 0x13);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT, bus_8131_2, (0x04<<2)|3, 0x2, 0x10);

	/* Standard local interrupt assignments:
	 * 		      Type	 Polarity		Trigger			 Bus ID   IRQ	APIC ID	     PIN# */
	mptable_lintsrc(mc, bus_isa);

	/* There is no extension information... */

	/* Compute the checksums */
	return mptable_finalize(mc);
}

unsigned long write_smp_table(unsigned long addr)
{
	void *v;
	v = smp_write_floating_table(addr, 0);
	return (unsigned long)smp_write_config_table(v);
}

