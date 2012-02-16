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
	unsigned char bus_pxhd_3 = 0;
	unsigned char bus_pxhd_4 = 0;
	unsigned char bus_pxhd_x = 0;
	unsigned char bus_ich5r_1;
	unsigned int bus_pxhd_id;

	mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);

	mptable_init(mc, LOCAL_APIC_ADDR);

	smp_write_processors(mc);

	{
		device_t dev;

		/* ich5r */
		dev = dev_find_slot(0, PCI_DEVFN(0x1e,0));
		if (dev) {
			bus_ich5r_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
		}
		else {
			printk(BIOS_DEBUG, "ERROR - could not find PCI 0:1f.0, using defaults\n");

			bus_ich5r_1 = 4;
		}
		/* pxhd-1 */
		dev = dev_find_slot(1, PCI_DEVFN(0x0,0));
		if (dev) {
			bus_pxhd_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);

		}
		else {
			printk(BIOS_DEBUG, "ERROR - could not find PCI 1:00.1, using defaults\n");

			bus_pxhd_1 = 2;
		}
		/* pxhd-2 */
		dev = dev_find_slot(1, PCI_DEVFN(0x00,2));
		if (dev) {
			bus_pxhd_2 = pci_read_config8(dev, PCI_SECONDARY_BUS);

		}
		else {
			printk(BIOS_DEBUG, "ERROR - could not find PCI 1:02.0, using defaults\n");

			bus_pxhd_2 = 3;
		}
		/* test for active riser with 2nd pxh device */
		dev = dev_find_slot(0, PCI_DEVFN(0x06,0));
                if (dev) {
			bus_pxhd_id = pci_read_config32(dev, PCI_VENDOR_ID);
			if(bus_pxhd_id == 0x35998086) {
				bus_pxhd_x = pci_read_config8(dev, PCI_SECONDARY_BUS);
				/* pxhd-3 */
				dev = dev_find_slot(bus_pxhd_x, PCI_DEVFN(0x0,0));
				if (dev) {
					bus_pxhd_id = pci_read_config32(dev, PCI_VENDOR_ID);
					if(bus_pxhd_id == 0x03298086) {
					    bus_pxhd_3 = pci_read_config8(dev, PCI_SECONDARY_BUS);
					}
				}
				/* pxhd-4 */
				dev = dev_find_slot(bus_pxhd_x, PCI_DEVFN(0x00,2));
				if (dev) {
					bus_pxhd_id = pci_read_config32(dev, PCI_VENDOR_ID);
                                        if(bus_pxhd_id == 0x032a8086) {
					    bus_pxhd_4 = pci_read_config8(dev, PCI_SECONDARY_BUS);
					}
				}
			}
		}
	}

	mptable_write_buses(mc, NULL, &bus_isa);

	/* IOAPIC handling */

	smp_write_ioapic(mc, 8, 0x20, IO_APIC_ADDR);
	{
		struct resource *res;
		device_t dev;
		/* pxhd apic 3 */
		dev = dev_find_slot(1, PCI_DEVFN(0x00,1));
		if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_0);
			if (res) {
				smp_write_ioapic(mc, 0x09, 0x20, res->base);
			}
		}
		else {
			printk(BIOS_DEBUG, "ERROR - could not find IOAPIC PCI 1:00.1\n");
		}
		/* pxhd apic 4 */
		dev = dev_find_slot(1, PCI_DEVFN(0x00,3));
		if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_0);
			if (res) {
				smp_write_ioapic(mc, 0x0a, 0x20, res->base);
			}
		}
		else {
			printk(BIOS_DEBUG, "ERROR - could not find IOAPIC PCI 1:00.3\n");
		}

		/* pxhd apic 5 */
		if(bus_pxhd_3) { /* Active riser pxhd */
			dev = dev_find_slot(bus_pxhd_x, PCI_DEVFN(0x00,1));
			if (dev) {
				res = find_resource(dev, PCI_BASE_ADDRESS_0);
				if (res) {
					smp_write_ioapic(mc, 0x0b, 0x20, res->base);
				}
			}
			else {
				printk(BIOS_DEBUG, "ERROR - could not find IOAPIC PCI %d:00.1\n",bus_pxhd_x);
			}
		}
		/* pxhd apic 6 */
		if(bus_pxhd_4) { /* active riser pxhd */
			dev = dev_find_slot(bus_pxhd_x, PCI_DEVFN(0x00,3));
			if (dev) {
				res = find_resource(dev, PCI_BASE_ADDRESS_0);
				if (res) {
					smp_write_ioapic(mc, 0x0c, 0x20, res->base);
				}
			}
			else {
				printk(BIOS_DEBUG, "ERROR - could not find IOAPIC PCI %d:00.3\n",bus_pxhd_x);
			}
		}
	}

	mptable_add_isa_interrupts(mc, bus_isa, 0x8, 0);

	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		bus_isa, 0x0a, 0x08, 0x10);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		bus_isa, 0x0b, 0x08, 0x11);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		bus_isa, 0x0a, 0x08, 0x10);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		bus_isa, 0x07, 0x08, 0x13);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		bus_isa, 0x0b, 0x08, 0x12);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		bus_isa, 0x05, 0x08, 0x17);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		bus_isa, 0x0b, 0x08, 0x12);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		bus_isa, 0x07, 0x08, 0x13);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		bus_isa, 0x0b, 0x08, 0x11);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		bus_isa, 0x0a, 0x08, 0x10);

	/* Standard local interrupt assignments */
	mptable_lintsrc(mc, bus_isa);

	/* FIXME verify I have the irqs handled for all of the risers */

	/* 2:3.0 PCI Slot 1 */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_pxhd_1, (3<<2)|0, 0x9, 0x0);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_pxhd_1, (3<<2)|1, 0x9, 0x3);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_pxhd_1, (3<<2)|2, 0x9, 0x5);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_pxhd_1, (3<<2)|3, 0x9, 0x4);


	/* 3:7.0 PCI Slot 2 */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_pxhd_2, (7<<2)|0, 0xa, 0x4);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_pxhd_2, (7<<2)|1, 0xa, 0x3);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_pxhd_2, (7<<2)|2, 0xa, 0x2);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_pxhd_2, (7<<2)|3, 0xa, 0x1);

	/* PCI Slot 3 (if active riser) */
	if(bus_pxhd_3) {
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
	                bus_pxhd_3, (1<<2)|0, 0xb, 0x0);
	}

	/* PCI Slot 4 (if active riser) */
	if(bus_pxhd_4) {
		smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
	                bus_pxhd_4, (1<<2)|0, 0xc, 0x0);
	}

	/* Onboard SCSI 0 */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_pxhd_1, (5<<2)|0, 0x9, 0x2);

	/* Onboard SCSI 1 */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_pxhd_1, (5<<2)|1, 0x9, 0x1);

	/* Onboard NIC 0 */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_pxhd_2, (4<<2)|0, 0xa, 0x6);

	/* Onboard NIC 1 */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_pxhd_2, (4<<2)|1, 0xa, 0x7);

	/* Onboard VGA */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		 bus_ich5r_1, (12<<2)|0, 0x8, 0x11);

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

