#include <arch/smp/mpspec.h>
#include <string.h>
#include <printk.h>

void smp_write_config_table(void *v)
{
	int ioapicid = 0;
	static const char sig[4] = "PCMP";
	static const char oem[8] = "LNXI    ";
	static const char productid[12] = "L440GX      ";
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


	smp_write_processors(mc);
	ioapicid = 2;

	smp_write_bus(mc, 0, "PCI   ");
	smp_write_bus(mc, 1, "PCI   ");
	smp_write_bus(mc, 2, "PCI   ");
	smp_write_bus(mc, 3, "ISA   ");

	smp_write_ioapic(mc, ioapicid, 0x11, 0xfec00000);
	
	/* ISA backward compatibility interrupts  */
	smp_write_intsrc(mc, mp_ExtINT,  0x05, 0x03, 0x00, ioapicid, 0x00);
	smp_write_intsrc(mc, mp_INT,     0x05, 0x03, 0x01, ioapicid, 0x01);
	smp_write_intsrc(mc, mp_INT,     0x05, 0x03, 0x00, ioapicid, 0x02);
	smp_write_intsrc(mc, mp_INT,     0x05, 0x03, 0x03, ioapicid, 0x03);
	smp_write_intsrc(mc, mp_INT,     0x05, 0x03, 0x04, ioapicid, 0x03);
	smp_write_intsrc(mc, mp_INT,     0x05, 0x03, 0x04, ioapicid, 0x04);
	smp_write_intsrc(mc, mp_INT,     0x05, 0x03, 0x06, ioapicid, 0x06);
	smp_write_intsrc(mc, mp_INT,     0x05, 0x03, 0x07, ioapicid, 0x07);
	smp_write_intsrc(mc, mp_INT,     0x05, 0x03, 0x08, ioapicid, 0x08);
	smp_write_intsrc(mc, mp_INT,     0x05, 0x03, 0x0c, ioapicid, 0x0c);
	smp_write_intsrc(mc, mp_INT,     0x05, 0x03, 0x0d, ioapicid, 0x0d);
	smp_write_intsrc(mc, mp_INT,     0x05, 0x03, 0x0e, ioapicid, 0x0e);
	smp_write_intsrc(mc, mp_INT,     0x05, 0x03, 0x0f, ioapicid, 0x0f);

	/* On bus 0 device 1 is the 440GX AGP/PCI bridge to bus 1 */
	/* On bus 1 device f is a DEC PCI bridge to bus 2 */
	/* Onboard pci devices */
	smp_write_intsrc(mc, mp_INT,     0x0f, 0x00, 0x30, ioapicid, 0x13); /* onboard SCSI */
	smp_write_intsrc(mc, mp_INT,     0x0f, 0x00, 0x38, ioapicid, 0x15); /* onboard NIC */
	smp_write_intsrc(mc, mp_INT,     0x0f, 0x00, 0x4b, ioapicid, 0x15); /* onboard PIIX4 */

	/* PCI card slots */
	smp_write_intsrc(mc, mp_INT,     0x0f, 0x02, 0x1c, ioapicid, 0x17); /* slot 6 A */
	smp_write_intsrc(mc, mp_INT,     0x0f, 0x02, 0x1d, ioapicid, 0x14); /* slot 6 B */
	smp_write_intsrc(mc, mp_INT,     0x0f, 0x02, 0x1e, ioapicid, 0x15); /* slot 6 C */
	smp_write_intsrc(mc, mp_INT,     0x0f, 0x02, 0x1f, ioapicid, 0x16); /* slot 6 D */

	smp_write_intsrc(mc, mp_INT,     0x0f, 0x02, 0x10, ioapicid, 0x14); /* slot 5 A */
	smp_write_intsrc(mc, mp_INT,     0x0f, 0x02, 0x11, ioapicid, 0x15); /* slot 5 B */
	smp_write_intsrc(mc, mp_INT,     0x0f, 0x02, 0x12, ioapicid, 0x16); /* slot 5 C */
	smp_write_intsrc(mc, mp_INT,     0x0f, 0x02, 0x13, ioapicid, 0x17); /* slot 5 D */
	/* 64bit slot */
	smp_write_intsrc(mc, mp_INT,     0x0f, 0x00, 0x24, ioapicid, 0x13); /* slot 4 A */
	smp_write_intsrc(mc, mp_INT,     0x0f, 0x00, 0x25, ioapicid, 0x15); /* slot 4 B */
	smp_write_intsrc(mc, mp_INT,     0x0f, 0x00, 0x26, ioapicid, 0x16); /* slot 4 C */
	smp_write_intsrc(mc, mp_INT,     0x0f, 0x00, 0x27, ioapicid, 0x17); /* slot 4 D */

	smp_write_intsrc(mc, mp_INT,     0x0f, 0x00, 0x2c, ioapicid, 0x12); /* slot 3 A */
	smp_write_intsrc(mc, mp_INT,     0x0f, 0x00, 0x2d, ioapicid, 0x17); /* slot 3 B */
	smp_write_intsrc(mc, mp_INT,     0x0f, 0x00, 0x2e, ioapicid, 0x15); /* slot 3 C */
	smp_write_intsrc(mc, mp_INT,     0x0f, 0x00, 0x2f, ioapicid, 0x16); /* slot 3 D */

	smp_write_intsrc(mc, mp_INT,     0x0f, 0x00, 0x34, ioapicid, 0x11); /* slot 2 A */
	smp_write_intsrc(mc, mp_INT,     0x0f, 0x00, 0x35, ioapicid, 0x16); /* slot 2 B */
	smp_write_intsrc(mc, mp_INT,     0x0f, 0x00, 0x36, ioapicid, 0x17); /* slot 2 C */
	smp_write_intsrc(mc, mp_INT,     0x0f, 0x00, 0x37, ioapicid, 0x15); /* slot 2 D */

	/* If this slot hadn't been removed I'd know it's PCI address,
	 * I'm guessing it's slot 0x0f (i.e. 0x3c >> 2) but I haven't
	 * confirmed that.
	 */
	/* Slot one is the slot farthest from the processor */
	smp_write_intsrc(mc, mp_INT,     0x0f, 0x00, 0x3c, ioapicid, 0x10); /* slot 1 A */
	smp_write_intsrc(mc, mp_INT,     0x0f, 0x00, 0x3d, ioapicid, 0x15); /* slot 1 B */
	smp_write_intsrc(mc, mp_INT,     0x0f, 0x00, 0x3e, ioapicid, 0x16); /* slot 1 C */
	smp_write_intsrc(mc, mp_INT,     0x0f, 0x00, 0x3f, ioapicid, 0x17); /* slot 1 D */

	/* Standard local interrupt assignments */
	smp_write_lintsrc(mc, mp_ExtINT, 0x05, 0x03, 0x00, MP_APIC_ALL, 0x00);
	smp_write_lintsrc(mc, mp_NMI,    0x05, 0x00, 0x00, MP_APIC_ALL, 0x01);

	/* The following information in the extension section linux doesn't currnetly need
	 * and has just been copied from the bios for now.
	 */
	smp_write_address_space(mc, 0x00, ADDRESS_TYPE_IO,       0x00000000, 0, 0x00010000, 0);
	smp_write_address_space(mc, 0x00, ADDRESS_TYPE_MEM,      0x08000000, 0, 0xee000000, 0);
	smp_write_address_space(mc, 0x00, ADDRESS_TYPE_PREFETCH, 0xf6000000, 0, 0x06000000, 0);
	smp_write_address_space(mc, 0x00, ADDRESS_TYPE_MEM,      0xfc000000, 0, 0x02e00000, 0);
	smp_write_address_space(mc, 0x00, ADDRESS_TYPE_MEM,      0xfef00000, 0, 0x01100000, 0);
	smp_write_address_space(mc, 0x00, ADDRESS_TYPE_MEM,      0x000a0000, 0, 0x00200000, 0);
	smp_write_address_space(mc, 0x00, ADDRESS_TYPE_MEM,      0x000d4000, 0, 0x00014000, 0);
	
	smp_write_bus_hierarchy(mc, 0x03, BUS_SUBTRACTIVE_DECODE, 0x00);

	smp_write_compatibility_address_space(mc, 0x00, ADDRESS_RANGE_ADD, RANGE_LIST_IO_ISA);
	smp_write_compatibility_address_space(mc, 0x00, ADDRESS_RANGE_ADD, RANGE_LIST_IO_VGA);

	mc->mpe_checksum = smp_compute_checksum(smp_next_mpc_entry(mc), mc->mpe_length);
	mc->mpc_checksum = smp_compute_checksum(mc, mc->mpc_length);
	printk_debug("Wrote the mp table end at: %p - %p\n",
		mc, smp_next_mpe_entry(mc));
}

void write_smp_table(void *v)
{
	smp_write_floating_table(v);
	smp_write_config_table(v);
}


