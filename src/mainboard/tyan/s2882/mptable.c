#include <console/console.h>
#include <arch/smp/mpspec.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <string.h>
#include <stdint.h>

#define ASSIGN_IRQ 0

void *smp_write_config_table(void *v)
{
        static const char sig[4] = "PCMP";
        static const char oem[8] = "TYAN    ";
        static const char productid[12] = "S2882       ";
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
	smp_write_ioapic(mc, 2, 0x11, 0xfec00000);
        {
                device_t dev;
                struct resource *res;
                dev = dev_find_slot(1, PCI_DEVFN(0x1,1));
                if (dev) {
                        res = find_resource(dev, PCI_BASE_ADDRESS_0);
                        if (res) {
                                smp_write_ioapic(mc, 0x03, 0x11, res->base);
                        }
                }
                dev = dev_find_slot(1, PCI_DEVFN(0x2,1));
                if (dev) {
                        res = find_resource(dev, PCI_BASE_ADDRESS_0);
                        if (res) {
                                smp_write_ioapic(mc, 0x04, 0x11, res->base);
                        }
                }

	}
  
/*I/O Ints:	Type	Polarity    Trigger	Bus ID	 IRQ	APIC ID	PIN# */
        smp_write_intsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0x0, 0x2, 0x0);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0x1, 0x2, 0x1);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0x0, 0x2, 0x2);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0x3, 0x2, 0x3);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0x4, 0x2, 0x4);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0x6, 0x2, 0x6);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0x7, 0x2, 0x7);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0x8, 0x2, 0x8);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0xc, 0x2, 0xc);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0xd, 0x2, 0xd);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0xe, 0x2, 0xe);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0xf, 0x2, 0xf);

#if ASSIGN_IRQ 
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x1, (4<<2)|3, 0x2, 0x13);

	{
		device_t dev;
        	dev = dev_find_device(PCI_VENDOR_ID_AMD, 0x746b, 0);
        	if (dev) {
                	/* initialize PCI interupts - these assignments depend
                   	on the PCB routing of PINTA-D 

                   	PINTA = IRQ5
                   	PINTB = IRQ9
                   	PINTC = IRQ11
                   	PINTD = IRQ10
                	*/
                	pci_write_config16(dev, 0x56, 0xab95);
		}
        }
#endif

#if ASSIGN_IRQ
        printk_info("setting Onboard AMD Southbridge \n");
        static const unsigned char slotIrqs_1_4[4] = { 5, 9, 11, 10 };
        pci_assign_irqs(1, 4, slotIrqs_1_4);
#endif
	
//On Board AMD USB
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (0<<2)|3, 0x2, 0x13);

#if ASSIGN_IRQ
        printk_info("setting Onboard AMD USB \n");
        static const unsigned char slotIrqs_8111_1_0[4] = { 0, 0, 0, 10 };
        pci_assign_irqs(bus_8111_1, 0, slotIrqs_8111_1_0);
#endif

//On Board ATI Display Adapter
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (6<<2)|0, 0x2, 0x12);

#if ASSIGN_IRQ
        printk_info("setting Onboard ATI Display Adapter\n");
        static const unsigned char slotIrqs_8111_1_6[4] = { 11, 0, 0, 0 };
        pci_assign_irqs(bus_8111_1, 6, slotIrqs_8111_1_6);
#endif

#if 1
//Slot 5 PCI 32
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (4<<2)|0, 0x2, 0x10);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (4<<2)|1, 0x2, 0x11);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (4<<2)|2, 0x2, 0x12); //
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (4<<2)|3, 0x2, 0x13); //

#if ASSIGN_IRQ
        printk_info("setting Slot 5 \n");
        static const unsigned char slotIrqs_8111_1_4[4] = { 5, 9, 11, 10 };
        pci_assign_irqs(bus_8111_1, 4, slotIrqs_8111_1_4);
#endif

#endif
//Onboard SI Serial ATA
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (5<<2)|0, 0x2, 0x13);

#if ASSIGN_IRQ
        printk_info("setting Onboard SI Serail ATA\n");
        static const unsigned char slotIrqs_8111_1_5[4] = { 10, 0, 0, 0 };
        pci_assign_irqs(bus_8111_1, 5, slotIrqs_8111_1_5);
#endif

//Onboard Intel 82551 10/100M NIC
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8111_1, (8<<2)|0, 0x2, 0x12);

#if ASSIGN_IRQ
        printk_info("setting Onboard Intel NIC\n");
        static const unsigned char slotIrqs_8111_1_8[4] = { 11, 0, 0, 0 };
        pci_assign_irqs(bus_8111_1, 8, slotIrqs_8111_1_8);
#endif

#if 1
//Slot 3 PCIX 100/66
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (3<<2)|0, 0x3, 0x3);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (3<<2)|1, 0x3, 0x0);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (3<<2)|2, 0x3, 0x1);//
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (3<<2)|3, 0x3, 0x2);//

#if ASSIGN_IRQ
        printk_info("setting Slot 3\n");
        static const unsigned char slotIrqs_8131_1_3[4] = { 10, 5, 9, 11 };
        pci_assign_irqs(bus_8131_1, 3, slotIrqs_8131_1_3);
#endif

//Slot 4 PCIX 100/66        
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (2<<2)|0, 0x3, 0x2);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (2<<2)|1, 0x3, 0x3);//
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (2<<2)|2, 0x3, 0x0);//
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (2<<2)|3, 0x3, 0x1);//

#if ASSIGN_IRQ
        printk_info("setting Slot 4\n");
        static const unsigned char slotIrqs_8131_1_2[4] = { 11, 10, 5, 9 };
        pci_assign_irqs(bus_8131_1, 2, slotIrqs_8131_1_2);
#endif

#endif
//Onboard adaptec scsi
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (6<<2)|0, 0x3, 0x0);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (6<<2)|1, 0x3, 0x1);

#if ASSIGN_IRQ
        printk_info("setting Onboard Adaptec  SCSI\n");
        static const unsigned char slotIrqs_8131_1_6[4] = { 5, 9, 0, 0 };
        pci_assign_irqs(bus_8131_1, 6, slotIrqs_8131_1_6);
#endif

//On Board NIC
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (9<<2)|0, 0x3, 0x0);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (9<<2)|1, 0x3, 0x1);


#if ASSIGN_IRQ
        printk_info("setting Onboard Broadcom NIC\n");
	static const unsigned char slotIrqs_8131_1_9[4] = { 5, 9, 0, 0 };	
        pci_assign_irqs(bus_8131_1, 9, slotIrqs_8131_1_9);
#endif

#if 1
//Slot 1 PCI-X 133/100/66
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_2, (3<<2)|0, 0x4, 0x0);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_2, (3<<2)|1, 0x4, 0x1);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_2, (3<<2)|2, 0x4, 0x2); //
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_2, (3<<2)|3, 0x4, 0x3); //

#if ASSIGN_IRQ
        printk_info("setting Slot 1\n");
        static const unsigned char slotIrqs_8131_2_3[4] = { 5, 9, 11, 10 };
        pci_assign_irqs(bus_8131_2, 3, slotIrqs_8131_2_3);
#endif

//Slot 2 PCI-X 133/100/66
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_2, (1<<2)|0, 0x4, 0x1);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_2, (1<<2)|1, 0x4, 0x2);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_2, (1<<2)|2, 0x4, 0x3);//
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_2, (1<<2)|3, 0x4, 0x0);//

#if ASSIGN_IRQ
        printk_info("setting Slot 2\n");
        static const unsigned char slotIrqs_8131_2_1[4] = { 9, 11, 10, 5 };
        pci_assign_irqs(bus_8131_2, 1, slotIrqs_8131_2_1);
#endif

#endif
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
