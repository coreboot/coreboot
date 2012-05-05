#include <console/console.h>
#include <arch/smp/mpspec.h>
#include <arch/ioapic.h>
#include <device/pci.h>
#include <string.h>
#include <stdint.h>
#if CONFIG_LOGICAL_CPUS
#include <cpu/amd/multicore.h>
#endif

static unsigned node_link_to_bus(unsigned node, unsigned link)
{
        device_t dev;
        unsigned reg;

        dev = dev_find_slot(0, PCI_DEVFN(0x18, 1));
        if (!dev) {
                return 0;
        }
        for(reg = 0xE0; reg < 0xF0; reg += 0x04) {
                uint32_t config_map;
                unsigned dst_node;
                unsigned dst_link;
                unsigned bus_base;
                config_map = pci_read_config32(dev, reg);
                if ((config_map & 3) != 3) {
                        continue;
                }
                dst_node = (config_map >> 4) & 7;
                dst_link = (config_map >> 8) & 3;
                bus_base = (config_map >> 16) & 0xff;
#if 0
                printk(BIOS_DEBUG, "node.link=bus: %d.%d=%d 0x%2x->0x%08x\n",
                        dst_node, dst_link, bus_base,
                        reg, config_map);
#endif
                if ((dst_node == node) && (dst_link == link))
                {
                        return bus_base;
                }
        }
        return 0;
}

static void *smp_write_config_table(void *v)
{
        struct mp_config_table *mc;
        int bus_isa;
	unsigned char bus_chain_0;
        unsigned char bus_8111_1;
	unsigned char bus_8151_1;
        unsigned apicid_base;
        unsigned apicid_8111;

        mc = (void *)(((char *)v) + SMP_FLOATING_TABLE_LEN);

	mptable_init(mc, LOCAL_APIC_ADDR);

        smp_write_processors(mc);

        {
                device_t dev;

                /* HT chain 0 */
                bus_chain_0 = node_link_to_bus(0, 0);
                if (bus_chain_0 == 0) {
                        printk(BIOS_DEBUG, "ERROR - cound not find bus for node 0 chain 0, using defaults\n");
                        bus_chain_0 = 1;
                }

                /* 8111 */
                dev = dev_find_slot(bus_chain_0, PCI_DEVFN(0x04,0));
                if (dev) {
                        bus_8111_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
                }
                else {
                        printk(BIOS_DEBUG, "ERROR - could not find PCI 1:03.0, using defaults\n");

                        bus_8111_1 = 3;
                }
	               /* 8151 */
                dev = dev_find_slot(bus_chain_0, PCI_DEVFN(0x02,0));
                if (dev) {
                        bus_8151_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
                        printk(BIOS_DEBUG, "bus_8151_1=%d\n",bus_8151_1);

                }
                else {
                        printk(BIOS_DEBUG, "ERROR - could not find PCI 1:02.0, using defaults\n");

                        bus_8151_1 = 2;
                }


        }

/*Bus:		Bus ID	Type*/
	mptable_write_buses(mc, NULL, &bus_isa);

/*I/O APICs:	APIC ID	Version	State		Address*/
#if CONFIG_LOGICAL_CPUS
	apicid_base = get_apicid_base(1);
#else
        apicid_base = CONFIG_MAX_PHYSICAL_CPUS;
#endif
        apicid_8111 = apicid_base+0;
	smp_write_ioapic(mc, apicid_8111, 0x11, IO_APIC_ADDR);

	mptable_add_isa_interrupts(mc, bus_isa, apicid_8111, 0);

/*I/O Ints:	Type	Polarity    Trigger	Bus ID	 IRQ	APIC ID	PIN# */
//??? What
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_chain_0, (5<<2)|3, apicid_8111, 0x13);
//Onboard AMD AC97 Audio ???
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_chain_0, (5<<2)|1, apicid_8111, 0x11);
// Onboard AMD USB
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0<<2)|3, apicid_8111, 0x13);

//  AGP Display Adapter
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8151_1, 0x0, apicid_8111, 0x10);

// Onboard Serial ATA
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0x05<<2)|0, apicid_8111, 0x13);
//Onboard Firewire
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0x0a<<2)|0, apicid_8111, 0x11);
//Onboard Broadcom NIC
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0x03<<2)|0, apicid_8111, 0x12);

//Onboard VIA USB 1.1
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0x0b<<2)|0, apicid_8111, 0x11);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0x0b<<2)|1, apicid_8111, 0x12);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0x0b<<2)|2, apicid_8111, 0x13);

//Slot 1
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0x06<<2)|0, apicid_8111, 0x12);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0x06<<2)|1, apicid_8111, 0x13);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0x06<<2)|2, apicid_8111, 0x10); //
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0x06<<2)|3, apicid_8111, 0x11); //

//Slot 2
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0x08<<2)|0, apicid_8111, 0x11);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0x08<<2)|1, apicid_8111, 0x12);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0x08<<2)|2, apicid_8111, 0x13); //
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0x08<<2)|3, apicid_8111, 0x10); //

//Slot 3
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0x09<<2)|0, apicid_8111, 0x10);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0x09<<2)|1, apicid_8111, 0x11);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0x09<<2)|2, apicid_8111, 0x12); //
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0x09<<2)|3, apicid_8111, 0x13); //

//Slot 4
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0x07<<2)|0, apicid_8111, 0x13);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0x07<<2)|1, apicid_8111, 0x10);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0x07<<2)|2, apicid_8111, 0x11); //
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0x07<<2)|3, apicid_8111, 0x12); //


//Slot 5
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0x04<<2)|0, apicid_8111, 0x10);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0x04<<2)|1, apicid_8111, 0x11);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0x04<<2)|2, apicid_8111, 0x12); //
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0x04<<2)|3, apicid_8111, 0x13); //



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
