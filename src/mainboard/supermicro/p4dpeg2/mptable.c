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
        static const char oem[8] = "LNXI    ";
        static const char productid[12] = "P4DPE-G2    ";
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

        smp_write_processors(mc, processor_map);


/*Bus:          Bus ID  Type*/
        smp_write_bus(mc, 0, "PCI   ");
        smp_write_bus(mc, 1, "PCI   ");
        smp_write_bus(mc, 2, "PCI   ");
        smp_write_bus(mc, 3, "PCI   ");
        smp_write_bus(mc, 4, "PCI   ");
        smp_write_bus(mc, 5, "PCI   ");
        smp_write_bus(mc, 6, "PCI   ");
        smp_write_bus(mc, 7, "PCI   ");
        smp_write_bus(mc, 8, "ISA   ");

/*I/O APICs:    APIC ID Version State           Address*/
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

/*I/O Ints:     Type    Polarity    Trigger     Bus ID   IRQ    APIC ID PIN#
*/      smp_write_intsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, 0x8, 0x0, 0x2, 0x0);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, 0x8, 0x1, 0x2, 0x1);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, 0x8, 0x0, 0x2, 0x2);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, 0x8, 0x3, 0x2, 0x3);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, 0x8, 0x4, 0x2, 0x4);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, 0x8, 0x5, 0x2, 0x5);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, 0x8, 0x6, 0x2, 0x6);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, 0x8, 0x7, 0x2, 0x7);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, 0x8, 0x8, 0x2, 0x8);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, 0x8, 0x9, 0x2, 0x9);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0, 0x75, 0x2, 0x13);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0, 0x74, 0x2, 0x10);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, 0x8, 0xc, 0x2, 0xc);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, 0x8, 0xd, 0x2, 0xd);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, 0x8, 0xe, 0x2, 0xe);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, 0x8, 0xf, 0x2, 0xf);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x0, 0x76, 0x2, 0x12);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x2, 0x4, 0x4, 0x0);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x3, 0x8, 0x3, 0x4);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x3, 0x9, 0x3, 0x5);
        smp_write_intsrc(mc, mp_INT, MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 0x7, 0x4, 0x2, 0x10);
/*Local Ints:   Type    Polarity    Trigger     Bus ID   IRQ    APIC ID PIN#*/
        smp_write_intsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, 0x8, 0x0, MP_APIC_ALL, 0x0);
        smp_write_intsrc(mc, mp_NMI, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH, 0x8, 0x0, MP_APIC_ALL, 0x1);

	/* p4dp8-g2 onboard scsi? */
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 
		0x06, (2 << 2)|0, 0x05, 0x04);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW, 
		0x06, (2 << 2)|1, 0x05, 0x05);

        /* Slot 1 */
        smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
                0x02, (1<<2)|0, 0x04, 0x00);
        /* Slot 2 */
        smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
                0x03, (1<<2)|0, 0x03, 0x00);
        /* Slot 3 */
        smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
                0x06, (1<<2)|0, 0x05, 0x00);
        /* Slot 4 */
        smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
                0x05, (1<<2)|0, 0x08, 0x00);
        /* Slot 5 */
        smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
                0x05, (2<<2)|0, 0x08, 0x04);
        /* Slot 6 */
        smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
                0x05, (3<<2)|0, 0x08, 0x08);

        /* There is no extension information... */

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
