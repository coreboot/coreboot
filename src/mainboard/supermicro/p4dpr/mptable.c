#include <arch/smp/mpspec.h>
#include <string.h>
#include <printk.h>

void *smp_write_config_table(void *v, unsigned long * processor_map)
{
	int ioapicid = 0;
	static const char sig[4] = "PCMP";
	static const char oem[8] = "LNXI    ";
	static const char productid[12] = "P4DPR       ";
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

#if 1
	smp_write_processor(mc, 0x00, 0x14, CPU_BOOTPROCESSOR | CPU_ENABLED, 
		0x00000f24, 0x3febfbff);
	smp_write_processor(mc, 0x06, 0x14, CPU_ENABLED, 0x00000f24, 0x3febfbff);
	smp_write_processor(mc, 0x01, 0x14, CPU_ENABLED, 0x00000f24, 0x3febfbff);
	smp_write_processor(mc, 0x07, 0x14, CPU_ENABLED, 0x00000f24, 0x3febfbff);
#else
	smp_write_processors(mc, processor_map);
#endif
	ioapicid = 2;

	smp_write_bus(mc, 0, "PCI   ");
	smp_write_bus(mc, 1, "PCI   ");
	smp_write_bus(mc, 2, "PCI   ");
	smp_write_bus(mc, 3, "PCI   ");
	smp_write_bus(mc, 4, "PCI   ");
	smp_write_bus(mc, 5, "ISA   ");

#if 1	
	smp_write_ioapic(mc, 2, 0x20, 0xfec00000);
	smp_write_ioapic(mc, 3, 0x20, 0xfec80000);
	smp_write_ioapic(mc, 4, 0x20, 0xfec80400);
#else
	smp_write_ioapic(mc, 4, 0x20, 0xfec00000);
	smp_write_ioapic(mc, 5, 0x20, 0xfec80000);
	smp_write_ioapic(mc, 8, 0x20, 0xfec80400);
#endif

	smp_write_intsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
        	0x05, 0x00, 0x02, 0x00);

	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
	        0x05, 0x01, 0x02, 0x01);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
	        0x05, 0x00, 0x02, 0x02);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
	        0x05, 0x03, 0x02, 0x03);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
	        0x05, 0x04, 0x02, 0x04);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
        	0x05, 0x05, 0x02, 0x05);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
	        0x05, 0x06, 0x02, 0x06);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
	        0x00, 0x76, 0x02, 0x12);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
	        0x05, 0x08, 0x02, 0x08);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
	        0x05, 0x09, 0x02, 0x09);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
	        0x00, 0x75, 0x02, 0x13);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
	        0x00, 0x74, 0x02, 0x10);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
	        0x04, 0x08, 0x02, 0x11);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
	        0x05, 0x0d, 0x02, 0x0d);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
	        0x05, 0x0e, 0x02, 0x0e);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
	        0x05, 0x0f, 0x02, 0x0f);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
	        0x03, 0x04, 0x03, 0x00);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
	        0x03, 0x10, 0x03, 0x07);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
	        0x04, 0x04, 0x02, 0x10);
	smp_write_lintsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
	         0x05, 0x00, MP_APIC_ALL, 0x00);
	smp_write_lintsrc(mc, mp_NMI,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
	         0x05, 0x00, MP_APIC_ALL, 0x01);

	
#if 0
	/* Onboard ich2 soutbhridge */
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x00, (0x1f << 2)|3, 0x02, 0x13);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x00, (0x1f << 2)|2, 0x02, 0x17);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x00, (0x1f << 2)|1, 0x02, 0x11);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x00, (0x1f << 2)|0, 0x02, 0x10);

	/* agp slot */
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
        	0x01, 0x00, 0x02, 0x16);

	/* Onboard PCI NIC */
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x04, (4 <<2)|0, 0x02, 0x10);

	/* Four standard PCI slots */
	/* Slot 1 */
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x04, (7 <<2)|0, 0x02, 0x10);
	/* Slot 2 */
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x04, (1 <<2)|0, 0x02, 0x11);
	/* Slot 3 */
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x04, (2 <<2)|0, 0x02, 0x12);
	/* Slot 4 */
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x04, (3 <<2)|0, 0x02, 0x13);

	/* Two 64 bit PCI slots */
	/* Slot 1 */
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x03, (1 <<2)|0, 0x02, 0x12);
	/* Slot 2 */
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x03, (2 <<2)|0, 0x02, 0x12);

	/* Two SCSI */
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x03, (4 <<2)|0, 0x02, 0x12);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x03, (4 <<2)|1, 0x02, 0x12);
	
	/* ISA backward compatibility interrupts  */
	smp_write_intsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		0x05, 0x00, 0x02, 0x00);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		0x05, 0x01, 0x02, 0x01);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		0x05, 0x00, 0x02, 0x02);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		0x05, 0x03, 0x02, 0x03);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		0x05, 0x04, 0x02, 0x04);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		0x05, 0x06, 0x02, 0x06);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		0x05, 0x07, 0x02, 0x07);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
		0x05, 0x08, 0x02, 0x08);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		0x05, 0x0d, 0x02, 0x0d);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		0x05, 0x0e, 0x02, 0x0e);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		0x05, 0x0f, 0x02, 0x0f);

	/* Standard local interrupt assignments */
	smp_write_lintsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		0x00, 0x00, MP_APIC_ALL, 0x00);
	smp_write_lintsrc(mc, mp_NMI,    MP_IRQ_TRIGGER_DEFAULT|MP_IRQ_POLARITY_DEFAULT,
		0x00, 0x00, MP_APIC_ALL, 0x01);
#endif
	
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

