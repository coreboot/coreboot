#include <console/console.h>
#include <arch/smp/mpspec.h>
#include <device/pci.h>
#include <string.h>
#include <stdint.h>

void *smp_write_config_table(void *v, unsigned long * processor_map)
{
        static const char sig[4] = "PCMP";
        static const char oem[8] = "TYAN    ";
        static const char productid[12] = "S2885       ";
        struct mp_config_table *mc;

        unsigned char bus_num;
        unsigned char bus_isa;
        unsigned char bus_8131_1;
        unsigned char bus_8131_2;
	unsigned char bus_8111_0;
        unsigned char bus_8111_1;
	unsigned char bus_8151_1;


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

       {
                device_t dev;

                /* 8111 */
                dev = dev_find_slot(3, PCI_DEVFN(0x03,0));
                if (dev) {
			bus_8111_0 = pci_read_config8(dev, PCI_PRIMARY_BUS);
                        bus_8111_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
                        bus_isa    = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
                        bus_isa++;
			printk_debug("bus_isa=%d\n",bus_isa);
                }
                else {
                        printk_debug("ERROR - could not find PCI 3:03.0, using defaults\n");

                        bus_8111_1 = 6;
                        bus_isa = 7;
                }
                /* 8131-1 */
                dev = dev_find_slot(3, PCI_DEVFN(0x01,0));
                if (dev) {
                        bus_8131_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);

                }
                else {
                        printk_debug("ERROR - could not find PCI 3:01.0, using defaults\n");

                        bus_8131_1 = 4;
                }
                /* 8131-2 */
                dev = dev_find_slot(3, PCI_DEVFN(0x02,0));
                if (dev) {
                        bus_8131_2 = pci_read_config8(dev, PCI_SECONDARY_BUS);

                }
                else {
                        printk_debug("ERROR - could not find PCI 3:02.0, using defaults\n");

                        bus_8131_2 = 5;
                }
	               /* 8151 */
                dev = dev_find_slot(1, PCI_DEVFN(0x02,0));
                if (dev) {
                        bus_8151_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
                        printk_debug("bus_8151_1=%d\n",bus_8151_1);
   
                }
                else {
                        printk_debug("ERROR - could not find PCI 1:02.0, using defaults\n");

                        bus_8151_1 = 2;
                }
  
   
        }



/*Bus:		Bus ID	Type*/
       /* define bus and isa numbers */
        for(bus_num = 0; bus_num < bus_isa; bus_num++) {
                smp_write_bus(mc, bus_num, "PCI   ");
        }
        smp_write_bus(mc, bus_isa, "ISA   ");

/*I/O APICs:	APIC ID	Version	State		Address*/
	smp_write_ioapic(mc, 2, 0x11, 0xfec00000);
        {
                struct pci_dev *dev;
                uint32_t base;
                dev = dev_find_slot(3, PCI_DEVFN(0x1,1));
                if (dev) {
                        base = pci_read_config32(dev, PCI_BASE_ADDRESS_0);
                        base &= PCI_BASE_ADDRESS_MEM_MASK;
                        smp_write_ioapic(mc, 3, 0x11, base);
                }
                dev = dev_find_slot(3, PCI_DEVFN(0x2,1));
                if (dev) {
                        base = pci_read_config32(dev, PCI_BASE_ADDRESS_0);
                        base &= PCI_BASE_ADDRESS_MEM_MASK;
                        smp_write_ioapic(mc, 4, 0x11, base);
                }
	}
  
/*I/O Ints:	Type	Polarity    Trigger	Bus ID	 IRQ	APIC ID	PIN#
*/	smp_write_intsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0x0, 0x2, 0x0);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,  bus_isa, 0x1, 0x2, 0x1);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,  bus_isa, 0x0, 0x2, 0x2);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,  bus_isa, 0x3, 0x2, 0x3);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,  bus_isa, 0x4, 0x2, 0x4);
  	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,  bus_isa, 0x5, 0x2, 0x5);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,  bus_isa, 0x6, 0x2, 0x6);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,  bus_isa, 0x7, 0x2, 0x7);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,  bus_isa, 0x8, 0x2, 0x8);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,  bus_isa, 0xc, 0x2, 0xc);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,  bus_isa, 0xd, 0x2, 0xd);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,  bus_isa, 0xe, 0x2, 0xe);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,  bus_isa, 0xf, 0x2, 0xf);
//??? What
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_0, (4<<2)|3, 0x2, 0x13);
//Onboard AMD AC97 Audio 
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_0, (4<<2)|1, 0x2, 0x11);
// Onboard AMD USB
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0<<2)|3, 0x2, 0x13);

//  AGP Display Adapter
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8151_1, 0x0, 0x2, 0x10);

// Onboard Serial ATA        
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0x0b<<2)|0, 0x2, 0x11);
//Onboard Firewire
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0x0c<<2)|0, 0x2, 0x13);
//Onboard Broadcom NIC
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (9<<2)|0, 0x3, 0x0);

//Slot 5 PCI 32
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0x0a<<2)|0, 0x2, 0x10);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0x0a<<2)|1, 0x2, 0x11);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0x0a<<2)|2, 0x2, 0x12); //
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0x0a<<2)|3, 0x2, 0x13); //

//Slot 3 PCIX 100/66
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (8<<2)|0, 0x3, 0x3);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (8<<2)|1, 0x3, 0x0);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (8<<2)|2, 0x3, 0x1);//
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (8<<2)|3, 0x3, 0x2);//

//Slot 4 PCIX 100/66        
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (7<<2)|0, 0x3, 0x2);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (7<<2)|1, 0x3, 0x3);//
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (7<<2)|2, 0x3, 0x0);//
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (7<<2)|3, 0x3, 0x1);//

//Slot 1 PCI-X 133/100/66
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_2, (3<<2)|0, 0x4, 0x0);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_2, (3<<2)|1, 0x4, 0x1);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_2, (3<<2)|2, 0x4, 0x2); //
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_2, (3<<2)|3, 0x4, 0x3); //

//Slot 2 PCI-X 133/100/66
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_2, (6<<2)|0, 0x4, 0x1);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_2, (6<<2)|1, 0x4, 0x2);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_2, (6<<2)|2, 0x4, 0x3);//
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_2, (6<<2)|3, 0x4, 0x0);//

/*Local Ints:	Type	Polarity    Trigger	Bus ID	 IRQ	APIC ID	PIN#*/
	smp_write_intsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0x0, MP_APIC_ALL, 0x0);
	smp_write_intsrc(mc, mp_NMI, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0x0, MP_APIC_ALL, 0x1);
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
