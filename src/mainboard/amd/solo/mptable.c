#include <console/console.h>
#include <arch/smp/mpspec.h>
#include <device/pci.h>
#include <string.h>
#include <stdint.h>

void *smp_write_config_table(void *v, unsigned long * processor_map)
{
	static const char sig[4] = "PCMP";
	static const char oem[8] = "AMD     ";
	static const char productid[12] = "SOLO7       ";
	struct mp_config_table *mc;
	unsigned char bus_num;
	unsigned char bus_isa;
	unsigned char bus_8151_1;
	unsigned char bus_8111_1;

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

	smp_write_processors(mc, processor_map);

	{
		device_t dev;

		printk_info("creating mp table...\n");
		
		/* 8111 */
		dev = dev_find_slot(1, PCI_DEVFN(0x04,0));
		if (dev) {
			bus_8111_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
			bus_isa	   = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
			bus_isa++;
			printk_debug("  mptable: 8111 PCI bus %d\n", bus_8111_1);
			printk_debug("  mptable: 8111 ISA bus %d\n", bus_isa);
		}
		else {
			printk_debug("ERROR - could not find 8111 at PCI 1:04.0, using defaults\n");

			bus_8111_1 = 3;
			bus_isa = 4;
		}
		/* 8151-1 */
		dev = dev_find_slot(1, PCI_DEVFN(0x01,0));
		if (dev) {
			bus_8151_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
			printk_debug("  mptable: 8151 PCI bus %d\n", bus_8151_1);
		}
		else {
			printk_debug("ERROR - could not find 8151 at PCI 1:01.0, using defaults\n");

			bus_8151_1 = 2;
		}
	}

	/* define bus and isa numbers */
	for(bus_num = 0; bus_num < bus_isa; bus_num++) {
		smp_write_bus(mc, bus_num, "PCI	  ");
	}
	smp_write_bus(mc, bus_isa, "ISA	  ");

	/* IOAPIC handling */

	smp_write_ioapic(mc, 2, 0x11, 0xfec00000);

	/* ISA backward compatibility interrupts  */
	smp_write_intsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x00, 0x02, 0x00);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x01, 0x02, 0x01);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x00, 0x02, 0x02);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x03, 0x02, 0x03);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x04, 0x02, 0x04);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x05, 0x02, 0x05);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x06, 0x02, 0x06);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x07, 0x02, 0x07);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x08, 0x02, 0x08);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x09, 0x02, 0x09);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x0a, 0x02, 0x0a);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x0b, 0x02, 0x0b);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x0c, 0x02, 0x0c);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x0d, 0x02, 0x0d);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x0e, 0x02, 0x0e);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x0f, 0x02, 0x0f);

	/* Standard local interrupt assignments */
	smp_write_lintsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x00, MP_APIC_ALL, 0x00);
	smp_write_lintsrc(mc, mp_NMI, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_isa, 0x00, MP_APIC_ALL, 0x01);


	/* AGP Slot */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_8151_1, (0<<2)|0, 0x02, 0x10);

	/* PCI Slot 1 */
#warning "FIXME get the irqs right, it's just hacked to work for now"
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_8111_1, (4<<2)|0, 0x02, 0x10);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_8111_1, (4<<2)|1, 0x02, 0x11);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_8111_1, (4<<2)|2, 0x02, 0x12);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_8111_1, (4<<2)|3, 0x02, 0x13);


	/* PCI Slot 2 */
#warning "FIXME get the irqs right, it's just hacked to work for now"
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_8111_1, (5<<2)|0, 0x02, 0x11);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_8111_1, (5<<2)|1, 0x02, 0x12);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_8111_1, (5<<2)|2, 0x02, 0x13);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_8111_1, (5<<2)|3, 0x02, 0x10);


	/* PCI Slot 3 */
#warning "FIXME get the irqs right, it's just hacked to work for now"
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_8111_1, (6<<2)|0, 0x02, 0x12);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_8111_1, (6<<2)|1, 0x02, 0x13);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_8111_1, (6<<2)|2, 0x02, 0x10);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_8111_1, (6<<2)|3, 0x02, 0x11);

	/* PCI Slot 4 */
#warning "FIXME get the irqs right, it's just hacked to work for now"
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_8111_1, (7<<2)|0, 0x02, 0x13);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_8111_1, (7<<2)|1, 0x02, 0x10);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_8111_1, (7<<2)|2, 0x02, 0x11);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		bus_8111_1, (7<<2)|3, 0x02, 0x12);

	/* Local devices */

	/* USB */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		bus_8111_1, (0<<2)|3, 0x02, 0x13);
	/* Sound */
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		1, (5<<2)|1, 0x02, 0x11);
	

	/* There is no extension information... */

	/* Compute the checksums */
	mc->mpe_checksum = smp_compute_checksum(smp_next_mpc_entry(mc), mc->mpe_length);

	mc->mpc_checksum = smp_compute_checksum(mc, mc->mpc_length);
	printk_debug("Wrote the mp table end at: %p - %p\n",
		mc, smp_next_mpe_entry(mc));
	return smp_next_mpe_entry(mc);
}

unsigned long write_smp_table(unsigned long addr, unsigned long *processor_map)
{
	void *v;
	v = smp_write_floating_table(addr);
	return (unsigned long)smp_write_config_table(v, processor_map);
}

