#include <arch/smp/mpspec.h>
#include <string.h>
#include <printk.h>
#include <cpu/p6/apic.h>

void *smp_write_config_table(void *v, unsigned long * processor_map)
{
	static const char sig[4] = "PCMP";
	static const char oem[8] = "TYAN    ";
	static const char productid[12] = "GUINESS     ";
	struct mp_config_table *mc;

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
	smp_write_bus(mc, 0, "PCI   ");
	smp_write_bus(mc, 1, "PCI   ");
	smp_write_bus(mc, 2, "ISA   ");

	smp_write_ioapic(mc, 2, 0x11, 0xfec00000);
	
	/* ISA backward compatibility interrupts  */
	smp_write_intsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
		0x02, 0x00, 0x02, 0x00);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
		0x02, 0x01, 0x02, 0x01);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
		0x02, 0x00, 0x02, 0x02);
	/* PCI 0x03 <-> 0x13 ? */
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x02, 0x03, 0x02, 0x03);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
		0x02, 0x04, 0x02, 0x04);
	/* PCI 0x05 <-> 0x12 ? */
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x02, 0x05, 0x02, 0x05);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
		0x02, 0x06, 0x02, 0x06);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
		0x02, 0x07, 0x02, 0x07);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
		0x02, 0x08, 0x02, 0x08);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
		0x02, 0x09, 0x02, 0x09);
	/* PCI 0x0a <-> 0x11 ? */
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x02, 0x0a, 0x02, 0x0a);
	/* PCI 0x0b <-> 0x10 ? */
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x02, 0x0b, 0x02, 0x0b);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
		0x02, 0x0c, 0x02, 0x0c);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
		0x02, 0x0d, 0x02, 0x0d);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
		0x02, 0x0e, 0x02, 0x0e);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
		0x02, 0x0f, 0x02, 0x0f);


	/* Onboard SCSI 0 */
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x00, (0xd<<2)|0, 0x02, 0x10);
	/* Onboard SCSI 1 */
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x00, (0xd<<2)|1, 0x02, 0x11);
	/* Onboard eth0 */
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x00, (0x0f<<2)|0, 0x02, 0x12);
	/* Onboard eth1 */
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x00, (0x10<<2)|0, 0x02, 0x13);

#if 0
	/* Onboard IDE0 */
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x00, (0x7<<2)|1, 0x02, 0x10);
#endif
	
	/* PCI slot 0 */
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x00, (0x08<<2)|0, 0x02, 0x10);
	/* PCI slot 1 */
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x00, (0x09<<2)|0, 0x02, 0x11);
	/* PCI Slot 2 */
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_HIGH,
		0x00, (0x0a<<2)|0, 0x02, 0x12);
	/* PCI Slot 3 */
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_HIGH,
		0x00, (0x0b<<2)|0, 0x02, 0x13);
	/* PCI Slot 4 */
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_HIGH,
		0x00, (0x0c<<2)|0, 0x02, 0x10);
       

	/* Standard local interrupt assignments */
	smp_write_lintsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
		0x02, 0x00, MP_APIC_ALL, 0x00);
	smp_write_lintsrc(mc, mp_NMI,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
		0x01, 0x00, MP_APIC_ALL, 0x01);


	/* The following information in the extension section linux doesn't currnetly need
	 * and has just been copied from the bios for now.
	 */
	smp_write_address_space(mc, 0x00, ADDRESS_TYPE_IO,       0x00000000, 0x00000000, 0x00010000, 0x00000000);
	smp_write_address_space(mc, 0x00, ADDRESS_TYPE_MEM,      0x10000000, 0x00000000, 0xe4100000, 0x00000000);
	smp_write_address_space(mc, 0x00, ADDRESS_TYPE_PREFETCH, 0xf4100000, 0x00000000, 0x07f00000, 0x00000000);
	smp_write_address_space(mc, 0x00, ADDRESS_TYPE_MEM,      0xfc000000, 0x00000000, 0x02e00000, 0x00000000);
	smp_write_address_space(mc, 0x00, ADDRESS_TYPE_MEM,      0xfee01000, 0x00000000, 0x011ff000, 0x00000000);
	smp_write_address_space(mc, 0x00, ADDRESS_TYPE_MEM,      0x000a0000, 0x00000000, 0x00024000, 0x00000000);
	smp_write_address_space(mc, 0x00, ADDRESS_TYPE_MEM,      0x000c8000, 0x00000000, 0x00002000, 0x00000000);
	smp_write_address_space(mc, 0x00, ADDRESS_TYPE_MEM,      0x000cc000, 0x00000000, 0x00002000, 0x00000000);
	smp_write_address_space(mc, 0x00, ADDRESS_TYPE_MEM,      0x000d0000, 0x00000000, 0x00001000, 0x00000000);
	smp_write_address_space(mc, 0x00, ADDRESS_TYPE_MEM,      0x000d2000, 0x00000000, 0x00001000, 0x00000000);
	smp_write_address_space(mc, 0x00, ADDRESS_TYPE_MEM,      0x000d4000, 0x00000000, 0x00001000, 0x00000000);
	smp_write_address_space(mc, 0x00, ADDRESS_TYPE_MEM,      0x000d6000, 0x00000000, 0x00001000, 0x00000000);
	smp_write_address_space(mc, 0x00, ADDRESS_TYPE_MEM,      0x000d8000, 0x00000000, 0x00002000, 0x00000000);
	smp_write_address_space(mc, 0x00, ADDRESS_TYPE_MEM,      0x000e0000, 0x00000000, 0x00012000, 0x00000000);
	smp_write_address_space(mc, 0x00, ADDRESS_TYPE_MEM,      0x000f4000, 0x00000000, 0x00002000, 0x00000000);
	smp_write_address_space(mc, 0x00, ADDRESS_TYPE_MEM,      0x000f8000, 0x00000000, 0x00004000, 0x00000000);
	smp_write_bus_hierarchy(mc, 0x02, BUS_SUBTRACTIVE_DECODE, 0x00);
	smp_write_compatibility_address_space(mc, 0x00, ADDRESS_RANGE_ADD,      RANGE_LIST_IO_ISA);
	smp_write_compatibility_address_space(mc, 0x00, ADDRESS_RANGE_ADD,      RANGE_LIST_IO_VGA);


	mc->mpe_checksum = smp_compute_checksum(smp_next_mpc_entry(mc), mc->mpe_length);
	mc->mpc_checksum = smp_compute_checksum(mc, mc->mpc_length);
	printk_debug("Wrote the mp table end at: %p - %p\n",
		mc, smp_next_mpe_entry(mc));
	return smp_next_mpe_entry(mc);
}

unsigned long write_smp_table(unsigned long addr, unsigned long *processor_map)
{
	void *v;
	printk_debug("Writing the mp table\n");
	v = smp_write_floating_table(addr);
	return (unsigned long)smp_write_config_table(v, processor_map);
}


