#include <arch/smp/mpspec.h>
#include <string.h>
#include <printk.h>
#include <cpu/p6/apic.h>

#define __STR(X) #X
#define STR(X) __STR(X)

void *smp_write_config_table(void *v, unsigned long * processor_map)
{
	static const char sig[4] = "PCMP";
	static const char oem[] = STR(MAINBOARD_VENDOR);
	static const char productid[12] = STR(MAINBOARD_PART_NUMBER);
	struct mp_config_table *mc;
	int len;
	int apic;
	int i;

	mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);
	memset(mc, 0, sizeof(*mc));

	memcpy(mc->mpc_signature, sig, sizeof(sig));
	mc->mpc_length = sizeof(*mc); /* initially just the header */
	mc->mpc_spec = 0x04;
	mc->mpc_checksum = 0; /* not yet computed */
	memset(mc->mpc_oem, ' ', sizeof(mc->mpc_oem));
	len = strnlen(oem, sizeof(mc->mpc_oem));
	memcpy(mc->mpc_oem, oem, len);
	memset(mc->mpc_productid, ' ', sizeof(mc->mpc_productid));
	len = strnlen(productid, sizeof(mc->mpc_productid));
	memcpy(mc->mpc_productid, productid, len);
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
	smp_write_bus(mc, 2, "PCI   ");
	smp_write_bus(mc, 3, "ISA   ");
#if 0
	for(i=apic=0;i<MAX_CPUS;i++)
		if(processor_map[i])
			apic++;
#endif
	apic=2;

	smp_write_ioapic(mc, apic, 0x11, 0xfec00000);

	smp_write_intsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
	        0x03, 0x00, apic, 0x00);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
	        0x03, 0x01, apic, 0x01);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
	        0x03, 0x00, apic, 0x02);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
	        0x03, 0x03, apic, 0x03);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
	        0x03, 0x04, apic, 0x04);
	/* Slot 1 */
        smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
                0x00, (0x08<<2)|0, apic, 0x10);
        smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
                0x00, (0x08<<2)|1, apic, 0x11);
        smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
                0x00, (0x08<<2)|2, apic, 0x12);
        smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
                0x00, (0x08<<2)|3, apic, 0x13);
	/* Slot 2 */
        smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
                0x00, (0x09<<2)|0, apic, 0x11);
        smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
                0x00, (0x09<<2)|1, apic, 0x12);
        smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
                0x00, (0x09<<2)|2, apic, 0x10);
        smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
                0x00, (0x09<<2)|3, apic, 0x11);
	/* Slot 3 */
        smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
                0x02, (0x04<<2)|0, apic, 0x10);
        smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
                0x02, (0x04<<2)|1, apic, 0x11);
        smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
                0x02, (0x04<<2)|2, apic, 0x12);
        smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
                0x02, (0x04<<2)|3, apic, 0x13);
	/* Slot 4 */
        smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
                0x02, (0x05<<2)|0, apic, 0x11);
        smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
                0x02, (0x05<<2)|1, apic, 0x12);
        smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
                0x02, (0x05<<2)|2, apic, 0x13);
        smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
                0x02, (0x05<<2)|3, apic, 0x10);

	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x00, 0x29, apic, 0x11);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
	        0x03, 0x06, apic, 0x06);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
	        0x03, 0x07, apic, 0x07);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
	        0x03, 0x08, apic, 0x08);

	/* ??? from factory */

        smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
                0x02, 0x03, apic, 0x13);
 
	/* Slot 5 */
        smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
                0x02, (0x06<<2)|0, apic, 0x12);
        smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
                0x02, (0x06<<2)|1, apic, 0x13);
        smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
                0x02, (0x06<<2)|2, apic, 0x10);
        smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
                0x02, (0x06<<2)|3, apic, 0x11);

	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
	        0x03, 0x0c, apic, 0x0c);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
	        0x03, 0x0d, apic, 0x0d);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
	        0x03, 0x0e, apic, 0x0e);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
	        0x03, 0x0f, apic, 0x0f);

	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x00, 0x28, apic, 0x10);

	/* Onboard eths */
	/* EEPRO100 */
        smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
                0x02, (0x08<<2)|0, apic, 0x12); 
	/* e1000 */
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x00, (0x0b<<2)|0, apic, 0x13);

	smp_write_lintsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
	         0x03, 0x00, MP_APIC_ALL, 0x00);
	smp_write_lintsrc(mc, mp_NMI,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
	         0x01, 0x00, MP_APIC_ALL, 0x01);

#if 1

smp_write_address_space(mc, 0x00, ADDRESS_TYPE_IO,       0x00000000, 0x00000000, 0x00010000, 0x00000000);
smp_write_address_space(mc, 0x00, ADDRESS_TYPE_MEM,      0x10000000, 0x00000000, 0xe6200000, 0x00000000);
smp_write_address_space(mc, 0x00, ADDRESS_TYPE_PREFETCH, 0xf6200000, 0x00000000, 0x05e00000, 0x00000000);
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


#endif

smp_write_bus_hierarchy(mc, 0x03, BUS_SUBTRACTIVE_DECODE, 0x00);
0x81, 0x08, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00,
smp_write_compatibility_address_space(mc, 0x00, ADDRESS_RANGE_ADD,      RANGE_LIST_IO_ISA);
smp_write_compatibility_address_space(mc, 0x00, ADDRESS_RANGE_ADD,      RANGE_LIST_IO_VGA);

	
	mc->mpe_checksum = smp_compute_checksum(smp_next_mpc_entry(mc), mc->mpe_length);
		mc->mpc_checksum = smp_compute_checksum(mc, mc->mpc_length);
	printk_debug("Wrote the mp table end at: %p - %p  apic %d\n",
		mc, smp_next_mpe_entry(mc),apic);
	return smp_next_mpe_entry(mc);
}

unsigned long write_smp_table(unsigned long addr, unsigned long *processor_map)
{
	void *v;
	printk_debug("Writing the mp table\n");
	v = smp_write_floating_table(addr);
	return (unsigned long)smp_write_config_table(v, processor_map);
}


