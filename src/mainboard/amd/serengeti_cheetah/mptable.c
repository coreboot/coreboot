#include <console/console.h>
#include <arch/smp/mpspec.h>
#include <arch/ioapic.h>
#include <device/pci.h>
#include <string.h>
#include <stdint.h>
#if CONFIG_LOGICAL_CPUS==1
#include <cpu/amd/multicore.h>
#endif
#include <cpu/amd/amdk8_sysconf.h>
#include "mb_sysconf.h"

static void *smp_write_config_table(void *v)
{
        struct mp_config_table *mc;
	int i, j, bus_isa;
	struct mb_sysconf_t *m;

        mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);

	mptable_init(mc, LAPIC_ADDR);

        smp_write_processors(mc);

	get_bus_conf();

	m = sysconf.mb;

	mptable_write_buses(mc, NULL, &bus_isa);

/*I/O APICs:	APIC ID	Version	State		Address*/
	smp_write_ioapic(mc, m->apicid_8111, 0x11, IO_APIC_ADDR); //8111
        {
                device_t dev;
		struct resource *res;
                dev = dev_find_slot(m->bus_8132_0, PCI_DEVFN(m->sbdn3, 1));
                if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_0);
			if (res) {
				smp_write_ioapic(mc, m->apicid_8132_1, 0x11, res->base);
			}
                }
                dev = dev_find_slot(m->bus_8132_0, PCI_DEVFN(m->sbdn3+1, 1));
                if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_0);
			if (res) {
				smp_write_ioapic(mc, m->apicid_8132_2, 0x11, res->base);
			}
                }

                j = 0;

                for(i=1; i< sysconf.hc_possible_num; i++) {
                        if(!(sysconf.pci1234[i] & 0x1) ) continue;

                        switch(sysconf.hcid[i]) {
                        case 1: // 8132
			case 3: // 8131
                                dev = dev_find_slot(m->bus_8132a[j][0], PCI_DEVFN(m->sbdn3a[j], 1));
                                if (dev) {
                                        res = find_resource(dev, PCI_BASE_ADDRESS_0);
                                        if (res) {
                                                smp_write_ioapic(mc, m->apicid_8132a[j][0], 0x11, res->base);
                                        }
                                }
                                dev = dev_find_slot(m->bus_8132a[j][0], PCI_DEVFN(m->sbdn3a[j]+1, 1));
                                if (dev) {
                                        res = find_resource(dev, PCI_BASE_ADDRESS_0);
                                        if (res) {
                                                smp_write_ioapic(mc, m->apicid_8132a[j][1], 0x11, res->base);
                                        }
                                }
                                break;
                        }
                        j++;
                }

	}

	mptable_add_isa_interrupts(mc, bus_isa, m->apicid_8111, 0);

/*I/O Ints:	Type	Polarity    Trigger	Bus ID	 IRQ	APIC ID	PIN# */
//??? What
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_8111_0, ((sysconf.sbdn+1)<<2)|3, m->apicid_8111, 0x13);

// Onboard AMD USB
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_8111_1, (0<<2)|3, m->apicid_8111, 0x13);

//Slot 3  PCI 32
        for(i=0;i<4;i++) {
                smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_8111_1, (5<<2)|i, m->apicid_8111, 0x10 + (1+i)%4); //16
        }


//Slot 4 PCI 32
        for(i=0;i<4;i++) {
                smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_8111_1, (4<<2)|i, m->apicid_8111, 0x10 + (0+i)%4); //16
        }


//Slot 1 PCI-X 133/100/66
        for(i=0;i<4;i++) {
                smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_8132_2, (1<<2)|i, m->apicid_8132_2, (0+i)%4); //
        }


//Slot 2 PCI-X 133/100/66
        for(i=0;i<4;i++) {
                smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_8132_1, (1<<2)|i, m->apicid_8132_1, (1+i)%4); //25
        }

        j = 0;

        for(i=1; i< sysconf.hc_possible_num; i++) {
                if(!(sysconf.pci1234[i] & 0x1) ) continue;
                int ii;
                device_t dev;
                struct resource *res;
                switch(sysconf.hcid[i]) {
                case 1:
		case 3:
                        dev = dev_find_slot(m->bus_8132a[j][0], PCI_DEVFN(m->sbdn3a[j], 1));
                        if (dev) {
                                res = find_resource(dev, PCI_BASE_ADDRESS_0);
                                if (res) {
                                        //Slot 1 PCI-X 133/100/66
                                        for(ii=0;ii<4;ii++) {
                                                smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_8132a[j][1], (0<<2)|ii, m->apicid_8132a[j][0], (0+ii)%4); //
                                        }
                                }
                        }

                        dev = dev_find_slot(m->bus_8132a[j][0], PCI_DEVFN(m->sbdn3a[j]+1, 1));
                        if (dev) {
                                res = find_resource(dev, PCI_BASE_ADDRESS_0);
                                if (res) {
                                        //Slot 2 PCI-X 133/100/66
                                        for(ii=0;ii<4;ii++) {
                                                smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_8132a[j][2], (0<<2)|ii, m->apicid_8132a[j][1], (0+ii)%4); //25
                                        }
                                }
                        }

                        break;
                case 2:

                //  Slot AGP
                        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, m->bus_8151[j][1], 0x0, m->apicid_8111, 0x11);
                        break;
                }

                j++;
        }



/*Local Ints:	Type	Polarity    Trigger	Bus ID	 IRQ	APIC ID	PIN#*/
	mptable_lintsrc(mc, bus_isa);
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
	v = smp_write_floating_table(addr, 0);
	return (unsigned long)smp_write_config_table(v);
}
