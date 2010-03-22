#include <console/console.h>
#include <arch/smp/mpspec.h>
#include <device/pci.h>
#include <string.h>
#include <stdint.h>

static void *smp_write_config_table(void *v)
{
	static const char sig[4] = "PCMP";
	static const char oem[8] = "LNXI    ";
	static const char productid[12] = "SE7520JR20  ";
	struct mp_config_table *mc;
	unsigned char bus_num;
	unsigned char bus_isa;
	unsigned char bus_pxhd_1;
	unsigned char bus_pxhd_2;
	unsigned char bus_pxhd_3 = 0;
	unsigned char bus_pxhd_4 = 0;
	unsigned char bus_pxhd_x;
	unsigned char bus_ich5r_1;
	unsigned int bus_pxhd_id;

	mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);
	memset(mc, 0, sizeof(*mc));

	memcpy(mc->mpc_signature, sig, sizeof(sig));
	mc->mpc_length = sizeof(*mc); /* initially just the header */
	mc->mpc_spec = 0x04;
	mc->mpc_checksum = 0; /* not yet computed */
	memcpy(mc->mpc_oem, oem, sizeof(oem));
	memcpy(mc->mpc_productid, productid, sizeof(productid));
	mc->mpc_oemptr = 0;
	mc->mpc_oemsize = 0;
	mc->mpc_entry_count = 0; /* No entries yet... */
	mc->mpc_lapic = LAPIC_ADDR;
	mc->mpe_length = 0;
	mc->mpe_checksum = 0;
	mc->reserved = 0;

	smp_write_processors(mc);
	
	{
		device_t dev;

		/* ich5r */
		dev = dev_find_slot(0, PCI_DEVFN(0x1e,0));
		if (dev) {
			bus_ich5r_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
			bus_isa	   = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
			bus_isa++;
		}
		else {
			printk(BIOS_DEBUG, "ERROR - could not find PCI 0:1f.0, using defaults\n");

			bus_ich5r_1 = 4;
			bus_isa = 5;
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
	
	/* define bus and isa numbers */
	for(bus_num = 0; bus_num < bus_isa; bus_num++) {
		smp_write_bus(mc, bus_num, "PCI	  ");
	}
	smp_write_bus(mc, bus_isa, "ISA	  ");

	/* IOAPIC handling */

	smp_write_ioapic(mc, 8, 0x20, 0xfec00000);
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

	
	/* ISA backward compatibility interrupts  */
	smp_write_intsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x00, 0x08, 0x00);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x01, 0x08, 0x01);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x00, 0x08, 0x02);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x03, 0x08, 0x03);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x04, 0x08, 0x04);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x06, 0x08, 0x06);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
		bus_isa, 0x08, 0x08, 0x08);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x09, 0x08, 0x09);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x0c, 0x08, 0x0c);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x0d, 0x08, 0x0d);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x0e, 0x08, 0x0e);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x0f, 0x08, 0x0f);
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
	smp_write_lintsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x00, MP_APIC_ALL, 0x00);
	smp_write_lintsrc(mc, mp_NMI, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x00, MP_APIC_ALL, 0x01);


#warning "FIXME verify I have the irqs handled for all of the risers"
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
	mc->mpe_checksum = smp_compute_checksum(smp_next_mpc_entry(mc), mc->mpe_length);

	mc->mpc_checksum = smp_compute_checksum(mc, mc->mpc_length);
	printk(BIOS_DEBUG, "Wrote the mp table end at: %p - %p\n",
		mc, smp_next_mpe_entry(mc));
	return smp_next_mpe_entry(mc);
}

unsigned long write_smp_table(unsigned long addr)
{
	void *v;
	v = smp_write_floating_table(addr);
	return (unsigned long)smp_write_config_table(v);
}

