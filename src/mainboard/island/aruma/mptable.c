#include <console/console.h>
#include <arch/smp/mpspec.h>
#include <cpu/x86/lapic.h>
#include <device/pci.h>
#include <string.h>
#include <stdint.h>

#define WRITE_IOAPIC(bus,device,fn,id,version)		\
do {							\
	device_t dev;					\
	struct resource *res;				\
	dev = dev_find_slot(bus, PCI_DEVFN(device,fn));	\
	if (!dev) break;				\
	res = find_resource(dev, PCI_BASE_ADDRESS_0);	\
	if (!res) break;				\
	smp_write_ioapic(mc, id, version, res->base);	\
} while(0);

unsigned get_apicid_base(unsigned ioapic_num)
{
        device_t dev;
        unsigned apicid_base;

        dev = dev_find_slot(0, PCI_DEVFN(0x18,0));
        apicid_base = ((pci_read_config32(dev, 0x60)>>4) & 7) + 1;

        return apicid_base;
}


void *smp_write_config_table(void *v)
{
        static const char sig[4] = "PCMP";
        static const char oem[8] = "AGAMI   ";
        static const char productid[12] = "ARUMA       ";
        struct mp_config_table *mc;
	int i;
	unsigned apicid_base;
	unsigned char bus_isa;
	device_t dev;

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
	
	/* Write busses */
	bus_isa=22; // ISA
	for (i=0; i<bus_isa; i++)
		smp_write_bus(mc, i, "PCI   ");
	smp_write_bus(mc, bus_isa, "ISA   ");

	/* enable ext_apic_id */
#if 1
	apicid_base = 1;
#else	
	apicid_base = get_apicid_base(15);
        if(lapicid()>=0x10) { 
		apicid_base = 0; 
	}
#endif

	printk_info("APIC ID BASE=0x%x\n",apicid_base);

	/* I/O APICs */
	smp_write_ioapic(mc, apicid_base, 0x11, 0xfec00000); // 8111 IOAPIC
	/* Write all 8131 IOAPICs */
	/* (8131: bus, dev, fn) , id, version */
	WRITE_IOAPIC(0x01,1,1, apicid_base+1, 0x11);
	WRITE_IOAPIC(0x01,2,1, apicid_base+2, 0x11);
	WRITE_IOAPIC(0x05,1,1, apicid_base+3, 0x11);
	WRITE_IOAPIC(0x05,2,1, apicid_base+4, 0x11);
	WRITE_IOAPIC(0x05,3,1, apicid_base+5, 0x11);
	WRITE_IOAPIC(0x05,4,1, apicid_base+6, 0x11);
	WRITE_IOAPIC(0x0c,1,1, apicid_base+7, 0x11);
	WRITE_IOAPIC(0x0c,2,1, apicid_base+8, 0x11);
	WRITE_IOAPIC(0x0c,3,1, apicid_base+9, 0x11);
	WRITE_IOAPIC(0x0c,4,1, apicid_base+10, 0x11);
	WRITE_IOAPIC(0x11,1,1, apicid_base+11, 0x11);
	WRITE_IOAPIC(0x11,2,1, apicid_base+12, 0x11);
	WRITE_IOAPIC(0x11,3,1, apicid_base+13, 0x11);
	WRITE_IOAPIC(0x11,4,1, apicid_base+14, 0x11);
	
	/*I/O Ints:	Type	Polarity    Trigger	Bus ID	 IRQ	APIC ID	PIN# */	
	smp_write_intsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0x0, apicid_base, 0x0);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0x1, apicid_base, 0x1);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0x0, apicid_base, 0x2);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0x3, apicid_base, 0x3);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0x4, apicid_base, 0x4);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0x6, apicid_base, 0x6);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0x7, apicid_base, 0x7);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0x8, apicid_base, 0x8);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0xc, apicid_base, 0xc);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0xd, apicid_base, 0xd);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0xe, apicid_base, 0xe);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0xf, apicid_base, 0xf);

	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 1, (4<<2)|0, apicid_base, 0x13);
	
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x1, 0x1f, apicid_base, 0x13);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x4, 0x03, apicid_base, 0x13);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x4, 0x10, apicid_base, 0x10);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x4, 0x14, apicid_base, 0x11);
	
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0xb, 0x10, 0x5, 0x1);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0xb, 0x11, 0x5, 0x0);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0xb, 0x14, 0x5, 0x0);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0xb, 0x15, 0x5, 0x3);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0xb, 0x18, 0x5, 0x3);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0xb, 0x19, 0x5, 0x0);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0xa, 0x8, 0x5, 0x2);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0xa, 0x9, 0x5, 0x3);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x9, 0x10, 0x6, 0x1);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x9, 0x11, 0x6, 0x0);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x9, 0x14, 0x6, 0x0);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x9, 0x15, 0x6, 0x3);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x9, 0x18, 0x6, 0x3);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x9, 0x19, 0x6, 0x0);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x8, 0x8, 0x6, 0x2);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x8, 0x9, 0x6, 0x3);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0xd, 0x4, 0x7, 0x0);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0xd, 0x8, 0x7, 0x1);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0xe, 0x4, 0x8, 0x0);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0xf, 0x4, 0x9, 0x0);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x12, 0x4, 0xb, 0x0);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x12, 0x8, 0xb, 0x1);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x13, 0x4, 0xc, 0x0);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x14, 0x4, 0xd, 0x0);
	
	/*Local Ints:	Type	Polarity    Trigger	Bus ID	 IRQ	APIC ID	PIN#*/
	smp_write_intsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0x0, MP_APIC_ALL, 0x0);
	smp_write_intsrc(mc, mp_NMI, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0x0, MP_APIC_ALL, 0x1);

	/*
	MP Config Extended Table Entries:
	  --
	  System Address Space
	   bus ID: 0 address type: I/O address
	   address base: 0x9000
	   address range: 0x2000
	  --
	  System Address Space
	   bus ID: 0 address type: I/O address
	   address base: 0x0
	   address range: 0x100
	  --
	  System Address Space
	   bus ID: 0 address type: memory address
	   address base: 0xa0000
	   address range: 0x20000
	  --
	  System Address Space
	   bus ID: 0 address type: memory address
	   address base: 0xaed00000
	   address range: 0x2200000
	  --
	  System Address Space
	   bus ID: 0 address type: prefetch address
	   address base: 0xb0f00000
	   address range: 0x100000
	  --
	  System Address Space
	   bus ID: 4 address type: I/O address
	   address base: 0xb000
	   address range: 0x2000
	  --
	  System Address Space
	   bus ID: 4 address type: memory address
	   address base: 0xb1000000
	   address range: 0x700000
	  --
	  System Address Space
	   bus ID: 4 address type: prefetch address
	   address base: 0xb1700000
	   address range: 0x500000
	  --
	  System Address Space
	   bus ID: 11 address type: memory address
	   address base: 0xb1c00000
	   address range: 0x400000
	  --
	  System Address Space
	   bus ID: 11 address type: prefetch address
	   address base: 0xb2000000
	   address range: 0x2400000
	  --
	  System Address Space
	   bus ID: 16 address type: memory address
	   address base: 0xb4400000
	   address range: 0x400000
	  --
	  System Address Space
	   bus ID: 16 address type: prefetch address
	   address base: 0xb4800000
	   address range: 0x4a400000
	  --
	  Bus Heirarchy
	   bus ID: 21 bus info: 0x01 parent bus ID: 0--
	  Compatibility Bus Address
	   bus ID: 0 address modifier: add
	   predefined range: 0x00000000--
	  Compatibility Bus Address
	   bus ID: 4 address modifier: subtract
	   predefined range: 0x00000000--
	  Compatibility Bus Address
	   bus ID: 11 address modifier: subtract
	   predefined range: 0x00000000--
	  Compatibility Bus Address
	   bus ID: 16 address modifier: subtract
	   predefined range: 0x00000000--
	  Compatibility Bus Address
	   bus ID: 0 address modifier: add
	   predefined range: 0x00000001--
	  Compatibility Bus Address
	   bus ID: 4 address modifier: subtract
	   predefined range: 0x00000001--
	  Compatibility Bus Address
	   bus ID: 11 address modifier: subtract
	   predefined range: 0x00000001--
	  Compatibility Bus Address
	   bus ID: 16 address modifier: subtract
	   predefined range: 0x00000001	
	*/
	
	/* There is no extension information... */
	  
	/* Compute the checksums */
	mc->mpe_checksum = smp_compute_checksum(smp_next_mpc_entry(mc), mc->mpe_length);
	mc->mpc_checksum = smp_compute_checksum(mc, mc->mpc_length);
	printk_debug("Wrote the mp table end at: %p - %p\n",
		mc, smp_next_mpe_entry(mc));
	return smp_next_mpe_entry(mc);
}

unsigned long write_smp_table(unsigned long addr)
{
	void *v;
	v = smp_write_floating_table(addr);
	return (unsigned long)smp_write_config_table(v);
}


