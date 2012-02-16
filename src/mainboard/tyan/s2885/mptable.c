#include <console/console.h>
#include <arch/smp/mpspec.h>
#include <arch/ioapic.h>
#include <device/pci.h>
#include <string.h>
#include <stdint.h>
#include <cpu/amd/amdk8_sysconf.h>

extern  unsigned char bus_8131_0;
extern  unsigned char bus_8131_1;
extern  unsigned char bus_8131_2;
extern  unsigned char bus_8111_0;
extern  unsigned char bus_8111_1;
extern  unsigned char bus_8151_0;
extern  unsigned char bus_8151_1;
extern  unsigned apicid_8111;
extern  unsigned apicid_8131_1;
extern  unsigned apicid_8131_2;

extern  unsigned sbdn3;
extern  unsigned sbdn5;

static void *smp_write_config_table(void *v)
{
        struct mp_config_table *mc;
	int i, bus_isa;

        mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);

	mptable_init(mc, LOCAL_APIC_ADDR);

        smp_write_processors(mc);

	get_bus_conf();

	mptable_write_buses(mc, NULL, &bus_isa);

/*I/O APICs:	APIC ID	Version	State		Address*/
	smp_write_ioapic(mc, apicid_8111, 0x11, IO_APIC_ADDR); //8111
        {
                device_t dev;
		struct resource *res;
                dev = dev_find_slot(bus_8131_0, PCI_DEVFN(sbdn3,1));
                if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_0);
			if (res) {
				smp_write_ioapic(mc, apicid_8131_1, 0x11, res->base);
			}
                }
                dev = dev_find_slot(bus_8131_0, PCI_DEVFN(sbdn3+1,1));
                if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_0);
			if (res) {
				smp_write_ioapic(mc, apicid_8131_2, 0x11, res->base);
			}
                }
	}

	mptable_add_isa_interrupts(mc, bus_isa, apicid_8111, 0);

/*I/O Ints:	Type	Polarity    Trigger	Bus ID	 IRQ	APIC ID	PIN# */
//??? What
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_0, ((sysconf.sbdn+1)<<2)|3, apicid_8111, 0x13);
//Onboard AMD AC97 Audio
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_0, ((sysconf.sbdn+1)<<2)|1, apicid_8111, 0x11);
// Onboard AMD USB
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0<<2)|3, apicid_8111, 0x13);

//  AGP Display Adapter
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8151_1, 0x0, apicid_8111, 0x10);

//Onboard Serial ATA
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0x0b<<2)|0, apicid_8111, 0x11);
//Onboard Firewire
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0x0c<<2)|0, apicid_8111, 0x13);
//Onboard Broadcom NIC
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (9<<2)|0, apicid_8131_1, 0x0);

//Slot 5 PCI 32
        for(i=0;i<4;i++) {
                smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0x0a<<2)|i, apicid_8111, 0x10 + (0+i)%4); //16
        }


//Slot 3 PCIX 100/66
        for(i=0;i<4;i++) {
                smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (8<<2)|i, apicid_8131_1, (3+i)%4); //27
        }


//Slot 4 PCIX 100/66
        for(i=0;i<4;i++) {
                smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (7<<2)|i, apicid_8131_1, (2+i)%4); //26
        }


//Slot 1 PCI-X 133/100/66
        for(i=0;i<4;i++) {
                smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_2, (3<<2)|i, apicid_8131_2, (0+i)%4); //28
        }


//Slot 2 PCI-X 133/100/66
        for(i=0;i<4;i++) {
                smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_2, (6<<2)|i, apicid_8131_2, (1+i)%4); //29
        }

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
