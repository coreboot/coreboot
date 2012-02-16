#include <console/console.h>
#include <arch/smp/mpspec.h>
#include <arch/ioapic.h>
#include <device/pci.h>
#include <string.h>
#include <stdint.h>

static void *smp_write_config_table(void *v)
{
        struct mp_config_table *mc;
        int isa_bus;

        mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);

	mptable_init(mc, LOCAL_APIC_ADDR);

        smp_write_processors(mc);
	mptable_write_buses(mc, NULL, &isa_bus);
/*I/O APICs:	APIC ID	Version	State		Address*/
	smp_write_ioapic(mc, 8, 0x20, IO_APIC_ADDR);
	{
                device_t dev;
                struct resource *res;
		dev = dev_find_slot(1, PCI_DEVFN(0x1e,0));
		if (dev) {
                        res = find_resource(dev, PCI_BASE_ADDRESS_0);
                        if (res) {
                                smp_write_ioapic(mc, 0x09, 0x20, res->base);
                        }
		}
		dev = dev_find_slot(1, PCI_DEVFN(0x1c,0));
		if (dev) {
                        res = find_resource(dev, PCI_BASE_ADDRESS_0);
                        if (res) {
                                smp_write_ioapic(mc, 0x0a, 0x20, res->base);
                        }
		}
	}
	mptable_add_isa_interrupts(mc, isa_bus, 0x8, 0);

/*I/O Ints:	Type	Polarity    Trigger	Bus ID	 IRQ	APIC ID	PIN#
*/
//USB
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0, 0x7c, 0x8, 0x12);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0, 0x7d, 0x8, 0x11);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0, 0x74, 0x8, 0x10);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0, 0x75, 0x8, 0x13);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0, 0x77, 0x8, 0x17);

//onboard ati
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x4, 0x8, 0x8, 0x12);

//onboard intel 82551 10/100
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x4, 0x4, 0x8, 0x11);

// onboard Intel 82547 1000
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x2, 0x4, 0xa, 0x0);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x2, 0x5, 0xa, 0x1);

//Slot 4
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x4, (0x3<<2)|0, 0x8, 0x12);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x4, (0x3<<2)|1, 0x8, 0x13);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x4, (0x3<<2)|2, 0x8, 0x10);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x4, (0x3<<2)|3, 0x8, 0x11);
//Slot 3
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x4, (0x4<<2)|0, 0x8, 0x13);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x4, (0x4<<2)|1, 0x8, 0x10);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x4, (0x4<<2)|2, 0x8, 0x11);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x4, (0x4<<2)|3, 0x8, 0x12);
//Slot 1
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x3, (0x3<<2)|0, 0x9, 0x0);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x3, (0x3<<2)|1, 0x9, 0x1);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x3, (0x3<<2)|2, 0x9, 0x2);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x3, (0x3<<2)|3, 0x9, 0x3);
//Slot 2
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x3, (0x6<<2)|0, 0x9, 0x4);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x3, (0x6<<2)|1, 0x9, 0x5);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x3, (0x6<<2)|2, 0x9, 0x6);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x3, (0x6<<2)|3, 0x9, 0x7);

/*Local Ints:	Type	Polarity    Trigger	Bus ID	 IRQ	APIC ID	PIN#*/
	mptable_lintsrc(mc, 0x0);
/*
MP Config Extended Table Entries:

--
System Address Space
 bus ID: 0 address type: I/O address
 address base: 0x9000
 address range: 0x6000
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
 address base: 0xfc700000
 address range: 0x2500000
--
System Address Space
 bus ID: 0 address type: prefetch address
 address base: 0xff600000
 address range: 0x500000
--
Bus Heirarchy
 bus ID: 5 bus info: 0x01 parent bus ID: 0--
Compatibility Bus Address
 bus ID: 0 address modifier: add
 predefined range: 0x00000000--
Compatibility Bus Address
 bus ID: 0 address modifier: add
 predefined range: 0x00000001	// There is no extension information...
*/
	/* Compute the checksums */
	return mptable_finalize(mc);
}

unsigned long write_smp_table(unsigned long addr)
{
	void *v;
	v = smp_write_floating_table(addr, 0);
	return (unsigned long)smp_write_config_table(v);
}
