/* generatred by MPTable, version 2.0.15*/
/* as modified by RGM for LinuxBIOS */
#include <arch/smp/mpspec.h>
#include <string.h>
#include <printk.h>
#include <pci.h>
#include <stdint.h>

void *smp_write_config_table(void *v, unsigned long * processor_map)
{
        static const char sig[4] = "PCMP";
        static const char oem[8] = "LnxLabs ";
        static const char productid[12] = "7501CW2     ";
        struct mp_config_table *mc;

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

#if 0
	smp_write_processor(mc, 0x00, 0x14, CPU_BOOTPROCESSOR | CPU_ENABLED, 
		0x00000f24, 0x3febfbff);
	smp_write_processor(mc, 0x06, 0x14, CPU_ENABLED, 0x00000f24, 0x3febfbff);
	smp_write_processor(mc, 0x01, 0x14, CPU_ENABLED, 0x00000f24, 0x3febfbff);
	smp_write_processor(mc, 0x07, 0x14, CPU_ENABLED, 0x00000f24, 0x3febfbff);
#else
        smp_write_processors(mc, processor_map);
#endif


/*Bus:		Bus ID	Type*/
	smp_write_bus(mc, 0, "PCI   ");
	smp_write_bus(mc, 1, "PCI   ");
	smp_write_bus(mc, 2, "PCI   ");
	smp_write_bus(mc, 3, "PCI   ");
	smp_write_bus(mc, 4, "PCI   ");
	smp_write_bus(mc, 5, "ISA   ");
/*I/O APICs:	APIC ID	Version	State		Address*/
	smp_write_ioapic(mc, 2, 0x20, 0xfec00000);
	{
		struct pci_dev *dev;
		uint32_t base;
		dev = pci_find_slot(1, PCI_DEVFN(0x1e,0));
		if (dev) {
			pci_read_config_dword(dev, PCI_BASE_ADDRESS_0, &base);
			base &= PCI_BASE_ADDRESS_MEM_MASK;
			smp_write_ioapic(mc, 3, 0x20, base);
		}
		dev = pci_find_slot(1, PCI_DEVFN(0x1c,0));
		if (dev) {
			pci_read_config_dword(dev, PCI_BASE_ADDRESS_0, &base);
			base &= PCI_BASE_ADDRESS_MEM_MASK;
			smp_write_ioapic(mc, 4, 0x20, base);
		}
                dev = pci_find_slot(4, PCI_DEVFN(0x1e,0));
                if (dev) {
                        pci_read_config_dword(dev, PCI_BASE_ADDRESS_0, &base);
                        base &= PCI_BASE_ADDRESS_MEM_MASK;
                        smp_write_ioapic(mc, 5, 0x20, base);
                }
                dev = pci_find_slot(4, PCI_DEVFN(0x1c,0));
                if (dev) {
                        pci_read_config_dword(dev, PCI_BASE_ADDRESS_0, &base);
                        base &= PCI_BASE_ADDRESS_MEM_MASK;
                        smp_write_ioapic(mc, 8, 0x20, base);
                }
	}
/*I/O Ints:	Type	Polarity    Trigger	Bus ID	 IRQ	APIC ID	PIN#
*/	smp_write_intsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, 0x5, 0x0, 0x2, 0x0);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, 0x5, 0x1, 0x2, 0x1);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, 0x5, 0x0, 0x2, 0x2);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, 0x5, 0x3, 0x2, 0x3);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, 0x5, 0x4, 0x2, 0x4);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0, 0x75, 0x2, 0x13);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, 0x5, 0x6, 0x2, 0x6);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, 0x5, 0x7, 0x2, 0x7);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, 0x5, 0x8, 0x2, 0x8);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, 0x5, 0x9, 0x2, 0x9);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0, 0x74, 0x2, 0x10);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0, 0x7d, 0x2, 0x11);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, 0x5, 0xc, 0x2, 0xc);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, 0x5, 0xd, 0x2, 0xd);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, 0x5, 0xe, 0x2, 0xe);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, 0x5, 0xf, 0x2, 0xf);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x4, 0xc, 0x2, 0x15);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x4, 0x10, 0x2, 0x14);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x4, 0x14, 0x2, 0x17);
	smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x4, 0x18, 0x2, 0x16);
/*Local Ints:	Type	Polarity    Trigger	Bus ID	 IRQ	APIC ID	PIN#*/
	smp_write_intsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, 0x5, 0x0, MP_APIC_ALL, 0x0);
	smp_write_intsrc(mc, mp_NMI, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, 0x5, 0x0, MP_APIC_ALL, 0x1);
/* MP Config Extended Table Entries:

--
System Address Space
 bus ID: 0 address type: I/O address
 address base: 0x0
 address range: 0x10000
--
System Address Space
 bus ID: 0 address type: memory address
 address base: 0x40000000
 address range: 0xbee00000
--
System Address Space
 bus ID: 0 address type: memory address
 address base: 0xfee01000
 address range: 0x11ff000
--
System Address Space
 bus ID: 4 address type: memory address
 address base: 0xa0000
 address range: 0x20000
--
System Address Space
 bus ID: 4 address type: memory address
 address base: 0xcc000
 address range: 0x10000
--
System Address Space
 bus ID: 4 address type: memory address
 address base: 0xe0000
 address range: 0x4000
--
Bus Heirarchy
 bus ID: 5 bus info: 0x01 parent bus ID: 0--
Compatibility Bus Address
 bus ID: 0 address modifier: add
 predefined range: 0x00000000--
Compatibility Bus Address
 bus ID: 0 address modifier: add
 predefined range: 0x00000001	 There is no extension information... */

	/* Compute the checksums */
	mc->mpe_checksum = smp_compute_checksum(smp_next_mpc_entry(mc), mc->mpe_length);
	mc->mpc_checksum = smp_compute_checksum(mc, mc->mpc_length);
	printk_debug("Wrote the mp table end at: %p - %p
",
		mc, smp_next_mpe_entry(mc));
	return smp_next_mpe_entry(mc);
}

unsigned long write_smp_table(unsigned long addr, unsigned long *processor_map)
{
	void *v;
	v = smp_write_floating_table(addr);
	return (unsigned long)smp_write_config_table(v, processor_map);
}
