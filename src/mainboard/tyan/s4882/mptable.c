#include <console/console.h>
#include <arch/smp/mpspec.h>
#include <device/pci.h>
#include <string.h>
#include <stdint.h>

void *smp_write_config_table(void *v)
{
        static const char sig[4] = "PCMP";
        static const char oem[8] = "TYAN    ";
        static const char productid[12] = "S48822       ";
        struct mp_config_table *mc;

        unsigned char bus_num;
        unsigned char bus_isa;
        unsigned char bus_8131_1;
        unsigned char bus_8131_2;
        unsigned char bus_8111_1;
  
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

       
        {
                device_t dev;
                
                /* 8111 */
                dev = dev_find_slot(1, PCI_DEVFN(0x03,0));
                if (dev) {
                        bus_8111_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
                        bus_isa    = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
                        bus_isa++; 
                }       
                else {
                        printk_debug("ERROR - could not find PCI 1:03.0, using defaults\n");

                        bus_8111_1 = 4;
                        bus_isa = 5;
                }
                /* 8131-1 */
                dev = dev_find_slot(1, PCI_DEVFN(0x01,0));
                if (dev) {
                        bus_8131_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);

                }
                else {
                        printk_debug("ERROR - could not find PCI 1:01.0, using defaults\n");

                        bus_8131_1 = 2;
                }
                /* 8131-2 */
                dev = dev_find_slot(1, PCI_DEVFN(0x02,0));
                if (dev) {
                        bus_8131_2 = pci_read_config8(dev, PCI_SECONDARY_BUS);

                }
                else {
                        printk_debug("ERROR - could not find PCI 1:02.0, using defaults\n");

                        bus_8131_2 = 3;
                }
        }

/*Bus:          Bus ID  Type*/
        /* define bus and isa numbers */
        for(bus_num = 0; bus_num < bus_isa; bus_num++) {
                smp_write_bus(mc, bus_num, "PCI   ");
        }
        smp_write_bus(mc, bus_isa, "ISA   ");

	
/*I/O APICs:	APIC ID	Version	State		Address*/
	smp_write_ioapic(mc, 4, 0x11, 0xfec00000);
        {
                device_t dev;
                struct resource *res;
                dev = dev_find_slot(1, PCI_DEVFN(0x1,1));
                if (dev) {
                        res = find_resource(dev, PCI_BASE_ADDRESS_0);
                        if (res) {
                                smp_write_ioapic(mc, 0x05, 0x11, res->base);
                        }
                }
                dev = dev_find_slot(1, PCI_DEVFN(0x2,1));
                if (dev) {
                        res = find_resource(dev, PCI_BASE_ADDRESS_0);
                        if (res) {
                                smp_write_ioapic(mc, 0x06, 0x11, res->base);
                        }
                }

	}
  
/*I/O Ints:	Type	Polarity    Trigger	Bus ID	 IRQ	APIC ID	PIN#
*/	smp_write_intsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0x0, 0x4, 0x0);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0x1, 0x4, 0x1);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0x0, 0x4, 0x2);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0x3, 0x4, 0x3);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0x4, 0x4, 0x4);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0x5, 0x4, 0x5);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0x6, 0x4, 0x6);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0x7, 0x4, 0x7);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0x8, 0x4, 0x8);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0x9, 0x4, 0x9);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0xa, 0x4, 0xa);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0xb, 0x4, 0xb);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0xc, 0x4, 0xc);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0xd, 0x4, 0xd);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0xe, 0x4, 0xe);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0xf, 0x4, 0xf);
	

	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 1, (4<<2)|0, 0x4, 0x13);


//On Board AMD USB
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0<<2)|3, 0x4, 0x13);

//On Board Via USB 1.1 and 2
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (3<<2)|0, 0x4, 0x11); //1.1
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (3<<2)|1, 0x4, 0x10); //1.1
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (3<<2)|2, 0x4, 0x12); //2

//Slot 5 PCI 32
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (4<<2)|0, 0x4, 0x10);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (4<<2)|1, 0x4, 0x11);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (4<<2)|2, 0x4, 0x12); //
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (4<<2)|3, 0x4, 0x13); //


//On Board SI Serial ATA
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (5<<2)|0, 0x4, 0x13);
//On Board ATI Display Adapter
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (6<<2)|0, 0x4, 0x12);


//Slot 4 PCIX 100/66
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (3<<2)|0, 0x5, 0x3);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (3<<2)|1, 0x5, 0x0);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (3<<2)|2, 0x5, 0x1);//
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (3<<2)|3, 0x5, 0x2);//

//Slot 3 PCIX 100/66        
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (2<<2)|0, 0x5, 0x2);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (2<<2)|1, 0x5, 0x3);//
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (2<<2)|2, 0x5, 0x0);//
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (2<<2)|3, 0x5, 0x1);//

//On Board LSI scsi and NIC
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (4<<2)|0, 0x5, 0x0);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (4<<2)|1, 0x5, 0x1);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (9<<2)|0, 0x5, 0x0);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (9<<2)|1, 0x5, 0x1);

//Slot 2 PCI-X 133/100/66
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_2, (3<<2)|0, 0x6, 0x0);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_2, (3<<2)|1, 0x6, 0x1);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_2, (3<<2)|2, 0x6, 0x2); //
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_2, (3<<2)|3, 0x6, 0x3); //

//Slot 1 PCI-X 133/100/66
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_2, (1<<2)|0, 0x6, 0x1);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_2, (1<<2)|1, 0x6, 0x2);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_2, (1<<2)|2, 0x6, 0x3);//
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_2, (1<<2)|3, 0x6, 0x0);//

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

unsigned long write_smp_table(unsigned long addr)
{
	void *v;
	v = smp_write_floating_table(addr);
	return (unsigned long)smp_write_config_table(v);
}
