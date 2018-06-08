/*
 * Copyright (c) 2011,2014 Oskar Enoksson <enok@lysator.liu.se>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */
#include <console/console.h>
#include <arch/smp/mpspec.h>
#include <arch/ioapic.h>
#include <device/pci.h>
#include <string.h>
#include <stdint.h>
#include <cpu/amd/amdk8_sysconf.h>
#include "mb_sysconf.h"

static void *smp_write_config_table(void *v)
{
	struct mp_config_table *mc;
	int bus_isa;

	mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);

	mptable_init(mc, LOCAL_APIC_ADDR);

	smp_write_processors(mc);

	get_bus_conf();

	struct mb_sysconf_t *m = sysconf.mb;

	mptable_write_buses(mc, NULL, &bus_isa);

/*I/O APICs:	APIC ID	Version	State		Address*/
	smp_write_ioapic(mc, m->apicid_8111, 0x20, VIO_APIC_VADDR);
	{
		struct device *dev;
		struct resource *res;
		dev = dev_find_slot(m->bus_8131_0, PCI_DEVFN(m->sbdn3,1));
		if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_0);
			if (res) {
				smp_write_ioapic(mc, m->apicid_8131_1, 0x20,
						 res2mmio(res, 0, 0));
			}
		}
		dev = dev_find_slot(m->bus_8131_0, PCI_DEVFN(m->sbdn3+1,1));
		if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_0);
			if (res) {
				smp_write_ioapic(mc, m->apicid_8131_2, 0x20,
						 res2mmio(res, 0, 0));
			}
		}

	}

	mptable_add_isa_interrupts(mc, bus_isa, m->apicid_8111, 0);

	//
	// The commented-out lines are auto-detected on my servers.
	//
/*I/O Ints:	Type	Polarity    Trigger	Bus ID	 IRQ	APIC ID	PIN# */
	// Integrated SMBus 2.0
	//smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_8111_0, ( 0x4 <<2)|3, apicid_8111  , 0x15);
	// Integrated AMD AC97 Audio
	//smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_8111_0, ( 0x4 <<2)|1, apicid_8111  , 0x11);
	//smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_8111_0, ( 0x4 <<2)|2, apicid_8111  , 0x12);
	// Integrated AMD USB
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_8111_1, ( 0x4 <<2)|0, m->apicid_8111  , 0x10);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_8111_1, ( 0x0 <<2)|3, m->apicid_8111  , 0x13);
	// On board ATI Rage XL
	//smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_8111_1, ( 0x5 <<2)|0, apicid_8111  , 0x14);
	// On board Broadcom nics
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_8131_2, ( 0x3 <<2)|0, m->apicid_8131_2, 0x03);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_8131_2, ( 0x3 <<2)|1, m->apicid_8131_2, 0x00);
	// On board LSI SCSI
	//smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_8131_2, ( 0x2 <<2)|0, apicid_8131_2, 0x02);

	// PCIX-133 Slot
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_8131_1, ( 0x1 <<2)|0, m->apicid_8131_1, 0x01);
	//smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_8131_1, ( 0x1 <<2)|1, apicid_8131_1, 0x02);
	//smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_8131_1, ( 0x1 <<2)|2, apicid_8131_1, 0x03);
	//smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_8131_1, ( 0x1 <<2)|3, apicid_8131_1, 0x04);

/*Local Ints:	Type	Polarity    Trigger	Bus ID	 IRQ	APIC ID	PIN#*/
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
