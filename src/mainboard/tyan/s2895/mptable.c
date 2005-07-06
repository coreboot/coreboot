#include <console/console.h>
#include <arch/smp/mpspec.h>
#include <device/pci.h>
#include <string.h>
#include <stdint.h>

void *smp_write_config_table(void *v)
{
        static const char sig[4] = "PCMP";
        static const char oem[8] = "TYAN    ";
        static const char productid[12] = "S2895       ";
        struct mp_config_table *mc;

        unsigned char bus_num;
        unsigned char bus_isa;
	unsigned char bus_ck804_0; //1
        unsigned char bus_ck804_1; //2
	unsigned char bus_ck804_2; //3
	unsigned char bus_ck804_3; //4
	unsigned char bus_ck804_4; //5
	unsigned char bus_ck804_5; //6
        unsigned char bus_8131_0;  //7
        unsigned char bus_8131_1;  //8
        unsigned char bus_8131_2;  //9 
	unsigned char bus_ck804b_0;//a 
	unsigned char bus_ck804b_1;//b
	unsigned char bus_ck804b_2;//c
	unsigned char bus_ck804b_3;//d
	unsigned char bus_ck804b_4;//e
	unsigned char bus_ck804b_5;//f
	unsigned apicid_base;
	unsigned apicid_ck804;
	unsigned apicid_8131_1;
	unsigned apicid_8131_2;
	unsigned apicid_ck804b;


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


                /* CK804 */
		bus_ck804_0 = 1;
                dev = dev_find_slot(bus_ck804_0, PCI_DEVFN(CK804_DEVN_BASE + 0x09,0));
                if (dev) {
                        bus_ck804_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
                        bus_ck804_5 = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
                        bus_ck804_5++;
                }
                else {
                        printk_debug("ERROR - could not find PCI 1:%02x.0, using defaults\n", CK804_DEVN_BASE + 0x09);

                        bus_ck804_1 = 2;
			bus_ck804_5 = 3;

                }

                dev = dev_find_slot(bus_ck804_0, PCI_DEVFN(CK804_DEVN_BASE + 0x0e,0));
                if (dev) {
                        bus_ck804_5 = pci_read_config8(dev, PCI_SECONDARY_BUS);
                        bus_8131_0 = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
                        bus_8131_0++;
                }
                else {
                        printk_debug("ERROR - could not find PCI 1:%02x.0, using defaults\n",CK804_DEVN_BASE + 0x0e);

                        bus_8131_0 = bus_ck804_5+1;
                }

                /* 8131-1 */
                dev = dev_find_slot(bus_8131_0, PCI_DEVFN(0x01,0));
                if (dev) {
                        bus_8131_1 = pci_read_config8(dev, PCI_SECONDARY_BUS);
			bus_8131_2 = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
			bus_8131_2++;
                }
                else {
                        printk_debug("ERROR - could not find PCI %02x:01.0, using defaults\n", bus_8131_0);

                        bus_8131_1 = bus_8131_0+1;
			bus_8131_2 = bus_8131_0+2;
                }
                /* 8131-2 */
                dev = dev_find_slot(bus_8131_0, PCI_DEVFN(0x02,0));
                if (dev) {
                        bus_8131_2 = pci_read_config8(dev, PCI_SECONDARY_BUS);
			bus_ck804b_0 = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
			bus_ck804b_0++;
                }
                else {
                        printk_debug("ERROR - could not find PCI %02x:02.0, using defaults\n", bus_8131_0);

                        bus_8131_2 = bus_8131_1+1;
			bus_ck804b_0 = bus_8131_1+2;
                }

                /* CK804b */

                dev = dev_find_slot(bus_ck804b_0, PCI_DEVFN(CK804_DEVN_BASE + 0x0e,0));
                if (dev) {
                        bus_ck804b_5 = pci_read_config8(dev, PCI_SECONDARY_BUS);
                        bus_isa = pci_read_config8(dev, PCI_SUBORDINATE_BUS);
                        bus_isa++;
                }
                else {
                        printk_debug("ERROR - could not find PCI %02x:%02x.0, using defaults\n", bus_ck804b_0,CK804_DEVN_BASE+0x0e);
#if 1
			bus_ck804b_5 = bus_ck804b_0+1;
#endif

                        bus_isa = bus_ck804b_5+1;
                }
   
        }



/*Bus:		Bus ID	Type*/
       /* define bus and isa numbers */
        for(bus_num = 0; bus_num < bus_isa; bus_num++) {
                smp_write_bus(mc, bus_num, "PCI   ");
        }
        smp_write_bus(mc, bus_isa, "ISA   ");

/*I/O APICs:	APIC ID	Version	State		Address*/
        apicid_base = CONFIG_MAX_CPUS; 
	apicid_ck804 = apicid_base;
	apicid_8131_1 = apicid_base+1;
	apicid_8131_2 = apicid_base+2;
	apicid_ck804b = apicid_base+3;
//	smp_write_ioapic(mc, 2, 0x11, 0xfec00000);
        {
                device_t dev;
		struct resource *res;
		uint32_t dword;

                dev = dev_find_slot(bus_ck804_0, PCI_DEVFN(CK804_DEVN_BASE+ 0x1,0));
                if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_1);
			if (res) {
				smp_write_ioapic(mc, apicid_ck804, 0x11, res->base);
			}

			dword = 0x0120d218;
	        	pci_write_config32(dev, 0x7c, dword);

		        dword = 0x00001a00;
		        pci_write_config32(dev, 0x80, dword);

	        	dword = 0x00080d72;
		        pci_write_config32(dev, 0x84, dword);

                }

                dev = dev_find_slot(bus_8131_0, PCI_DEVFN(0x1,1));
                if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_0);
			if (res) {
				smp_write_ioapic(mc, apicid_8131_1, 0x11, res->base);
			}
                }
                dev = dev_find_slot(bus_8131_0, PCI_DEVFN(0x2,1));
                if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_0);
			if (res) {
				smp_write_ioapic(mc, apicid_8131_2, 0x11, res->base);
			}
                }

                dev = dev_find_slot(bus_ck804b_0, PCI_DEVFN(CK804_DEVN_BASE + 0x1,0));
                if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_1);
			if (res) {
				smp_write_ioapic(mc, apicid_ck804b, 0x11, res->base);
			}

			dword = 0x0000d218;
                        pci_write_config32(dev, 0x7c, dword);

                        dword = 0x00000000;
                        pci_write_config32(dev, 0x80, dword);

                        dword = 0x00000d00;
                        pci_write_config32(dev, 0x84, dword);

                }

	}
  
/*I/O Ints:	Type	Polarity    Trigger	Bus ID	 IRQ	APIC ID	PIN#
*/	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, bus_isa, 0x0, apicid_ck804, 0x0);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,  bus_isa, 0x1, apicid_ck804, 0x1);
	smp_write_intsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,  bus_isa, 0x0, apicid_ck804, 0x2);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,  bus_isa, 0x3, apicid_ck804, 0x3);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,  bus_isa, 0x4, apicid_ck804, 0x4);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,  bus_isa, 0x6, apicid_ck804, 0x6);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,  bus_isa, 0x7, apicid_ck804, 0x7);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,  bus_isa, 0x8, apicid_ck804, 0x8);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,  bus_isa, 0xc, apicid_ck804, 0xc);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,  bus_isa, 0xd, apicid_ck804, 0xd);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,  bus_isa, 0xe, apicid_ck804, 0xe);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,  bus_isa, 0xf, apicid_ck804, 0xf);

        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_ck804_0, ((CK804_DEVN_BASE+1)<<2)|1, apicid_ck804, 0xa);

        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_ck804_0, ((CK804_DEVN_BASE+2)<<2)|0, apicid_ck804, 0x16); 

        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_ck804_0, ((CK804_DEVN_BASE+2)<<2)|1, apicid_ck804, 0x17); 

        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_ck804_0, ((CK804_DEVN_BASE+4)<<2)|0, apicid_ck804, 0x14); 

        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_ck804_0, ((CK804_DEVN_BASE +7)<<2)|0, apicid_ck804, 0x11); 

        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_ck804_0, ((CK804_DEVN_BASE +8)<<2)|0, apicid_ck804, 0x12); 

        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_ck804_0, ((CK804_DEVN_BASE +0x0a)<<2)|0, apicid_ck804, 0x15); 

#if 1
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_ck804_5, (0x00<<2)|0, apicid_ck804, 0x12); // 
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_ck804_5, (0x00<<2)|1, apicid_ck804, 0x13); // 
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_ck804_5, (0x00<<2)|2, apicid_ck804, 0x10); // 
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_ck804_5, (0x00<<2)|3, apicid_ck804, 0x11); // 
#endif

        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_ck804_1, (0x05<<2)|0, apicid_ck804, 0x13); // 

	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_ck804_1, (0x04<<2)|0, apicid_ck804, 0x10); // 
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_ck804_1, (0x04<<2)|1, apicid_ck804, 0x11); // 
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_ck804_1, (0x04<<2)|2, apicid_ck804, 0x12); // 
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_ck804_1, (0x04<<2)|3, apicid_ck804, 0x13); // 

        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_ck804b_0, ((CK804_DEVN_BASE+0x0a)<<2)|0, apicid_ck804b, 0x15);//

#if 1
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_ck804b_5, (0x00<<2)|0, apicid_ck804b, 0x12);//
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_ck804b_5, (0x00<<2)|1, apicid_ck804b, 0x13); // 
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_ck804b_5, (0x00<<2)|2, apicid_ck804b, 0x10); // 
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_ck804b_5, (0x00<<2)|3, apicid_ck804b, 0x11); // 
#endif


        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_2, (4<<2)|0, apicid_8131_2, 0x0); //
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_2, (4<<2)|1, apicid_8131_2, 0x1);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_2, (4<<2)|2, apicid_8131_2, 0x2); //
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_2, (4<<2)|3, apicid_8131_2, 0x3); //

        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_2, (9<<2)|0, apicid_8131_2, 0x1); //
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_2, (9<<2)|1, apicid_8131_2, 0x2);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_2, (9<<2)|2, apicid_8131_2, 0x3);//
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_2, (9<<2)|3, apicid_8131_2, 0x0);//


        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_2, (6<<2)|0, apicid_8131_2, 0x2); // 
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_2, (6<<2)|1, apicid_8131_2, 0x3);

        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (4<<2)|0, apicid_8131_1, 0x0); // 
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (4<<2)|1, apicid_8131_1, 0x1);//
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (4<<2)|2, apicid_8131_1, 0x2);//
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, bus_8131_1, (4<<2)|3, apicid_8131_1, 0x3);//

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
