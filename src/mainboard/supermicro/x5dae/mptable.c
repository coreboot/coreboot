#include <arch/smp/mpspec.h>
#include <string.h>
#include <printk.h>
#include <pci.h>
#include <stdint.h>

void *smp_write_config_table(void *v, unsigned long * processor_map)
{
	static const char sig[4] = "PCMP";
	static const char oem[8] = "LNXI    ";
	static const char productid[12] = "X5DPR       ";
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

	smp_write_bus(mc, 0, "PCI   ");
	smp_write_bus(mc, 1, "PCI   ");
	smp_write_bus(mc, 2, "PCI   ");
	smp_write_bus(mc, 3, "PCI   ");
	smp_write_bus(mc, 4, "PCI   ");
	smp_write_bus(mc, 5, "ISA   ");
	
	/* FIXME better IOAPIC handling */

	smp_write_ioapic(mc, 2, 0x20, 0xfec00000);
	{
		struct pci_dev *dev;
		uint32_t base;
		/* 1:1e.0 */
		dev = pci_find_slot(1, PCI_DEVFN(0x1e,0));
		if (dev) {
			pci_read_config_dword(dev, PCI_BASE_ADDRESS_0, &base);
			base &= PCI_BASE_ADDRESS_MEM_MASK;
			smp_write_ioapic(mc, 3, 0x20, base);
		}
		/* 1:1c.0 */
		dev = pci_find_slot(1, PCI_DEVFN(0x1c,0));
		if (dev) {
			pci_read_config_dword(dev, PCI_BASE_ADDRESS_0, &base);
			base &= PCI_BASE_ADDRESS_MEM_MASK;
			smp_write_ioapic(mc, 4, 0x20, base);
		}
	}

	/* ISA backward compatibility interrupts  */
	smp_write_intsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
        	0x05, 0x00, 0x02, 0x00);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
	        0x05, 0x01, 0x02, 0x01);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
	        0x05, 0x00, 0x02, 0x02);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
	        0x05, 0x03, 0x02, 0x03);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
	        0x05, 0x04, 0x02, 0x04);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
        	0x05, 0x05, 0x02, 0x05);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
	        0x05, 0x06, 0x02, 0x06);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
	        0x05, 0x07, 0x02, 0x07);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
	        0x05, 0x08, 0x02, 0x08);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
	        0x05, 0x09, 0x02, 0x09);

	/* USB Controller */
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
	        0x00, (0x1d<<2)|1, 0x02, 0x13);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
	        0x00, (0x1d<<2)|0, 0x02, 0x10);

	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
	        0x06, 0x0c, 0x02, 0x0c);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
	        0x05, 0x0d, 0x02, 0x0d);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
	        0x05, 0x0e, 0x02, 0x0e);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
	        0x05, 0x0f, 0x02, 0x0f);

	/* USB Controller */
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
	        0x00, (0x1d<<2)|2, 0x02, 0x12);

	/* Onboard 82559 Ethernet */
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
	        0x02, (0x3<<2)|0, 0x04, 0x06);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
	        0x02, (0x3<<2)|1, 0x04, 0x07);

	/* PCI slot 1 */
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x03, (1<<2)|0, 0x03, 0x00);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
		0x03, (1<<2)|1, 0x03, 0x01);

	/* Slot 2 */
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
	        0x02, (1<<2)|0, 0x04, 0x00);
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
	        0x02, (1<<2)|1, 0x04, 0x01);

	/* VGA */
	smp_write_intsrc(mc, mp_INT,    MP_IRQ_TRIGGER_LEVEL|MP_IRQ_POLARITY_LOW,
	        0x04, (1<<2)|0, 0x02, 0x10);

	/* Standard local interrupt assignments */
	smp_write_lintsrc(mc, mp_ExtINT, MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
		0x05, 0x00, MP_APIC_ALL, 0x00);
	smp_write_lintsrc(mc, mp_NMI,    MP_IRQ_TRIGGER_EDGE|MP_IRQ_POLARITY_HIGH,
		0x05, 0x00, MP_APIC_ALL, 0x01);

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

