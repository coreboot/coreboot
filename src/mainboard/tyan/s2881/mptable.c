#include <console/console.h>
#include <arch/smp/mpspec.h>
#include <arch/ioapic.h>
#include <device/pci.h>
#include <string.h>
#include <stdint.h>

#include <cpu/amd/amdk8_sysconf.h>

extern  unsigned char bus_isa;
extern  unsigned char bus_8131_0;
extern  unsigned char bus_8131_1;
extern  unsigned char bus_8131_2;
extern  unsigned char bus_8111_0;
extern  unsigned char bus_8111_1;
extern  unsigned apicid_8111;
extern  unsigned apicid_8131_1;
extern  unsigned apicid_8131_2;

extern  unsigned sbdn3;



static void *smp_write_config_table(void *v)
{
        static const char sig[4] = "PCMP";
        static const char oem[8] = "COREBOOT";
        static const char productid[12] = "S2881       ";
        struct mp_config_table *mc;

        unsigned char bus_num;

	int i;

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

	get_bus_conf();


/*Bus:          Bus ID  Type*/
        /* define bus and isa numbers */
        for(bus_num = 0; bus_num < bus_isa; bus_num++) {
                smp_write_bus(mc, bus_num, "PCI   ");
        }
        smp_write_bus(mc, bus_isa, "ISA   ");


/*I/O APICs:	APIC ID	Version	State		Address*/
	smp_write_ioapic(mc, apicid_8111, 0x11, IO_APIC_ADDR);
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
//8111 LPC ????
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_0, ((sysconf.sbdn+1)<<2)|0, apicid_8111, 0x13);

//On Board AMD USB ???
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0<<2)|3, apicid_8111, 0x13);

//On Board ATI Display Adapter
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (6<<2)|0, apicid_8111, 0x12);

//On Board SI Serial ATA
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (5<<2)|0, apicid_8111, 0x11);

//Slot 3 PCIX 100/66
        for(i=0;i<4;i++) {
                smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (8<<2)|i, apicid_8131_1, (3+i)%4); //27
        }

//On Board NIC and adaptec scsi
        for(i=0;i<2;i++) {
                smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (9<<2)|i, apicid_8131_1, (0+i)%4); //24
                smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (0xa<<2)|i, apicid_8131_1, (0+i)%4); //24
        }

//Slot 1 PCI-X 133/100/66 or Side 1 on raiser card
        for(i=0;i<4;i++) {
                smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_2, (3<<2)|i, apicid_8131_2, (0+i)%4); //28
        }

        //Slot 1 PCI-X 133/100/66, Side 2 on raiser card
        //Fix ME, IRQ Pins?
        for(i=0;i<4;i++) {
                smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_2, (4<<2)|i, apicid_8131_2, (1+i)%4); //28
        }



/*Local Ints:	Type	Polarity    Trigger	Bus ID	 IRQ	APIC ID	PIN#*/
	smp_write_intsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0x0, MP_APIC_ALL, 0x0);
	smp_write_intsrc(mc, mp_NMI, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0x0, MP_APIC_ALL, 0x1);
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
