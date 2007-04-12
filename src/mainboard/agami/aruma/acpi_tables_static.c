/*
 * Agami Aruma ACPI support
 * 
 * written by Stefan Reinauer <stepan@coresystems.de>
 *  (C) 2005 Stefan Reinauer
 *  (C) 2007 coresystems GmbH
 */

#include <console/console.h>
#include <string.h>
#include <arch/acpi.h>
#include <device/pci.h>
#include <device/pci_ids.h>

extern unsigned char AmlCode[];

#define IO_APIC_ADDR	0xfec00000UL

unsigned long acpi_fill_madt(unsigned long current)
{
	unsigned int gsi_base=0x18, ioapic_nr=2, i;
	device_t dev=0;
 
	/* create all subtables for 4p */
	current += acpi_create_madt_lapic((acpi_madt_lapic_t *)current, 0, 0); //SDE BSP APIC ID=0
	current += acpi_create_madt_lapic((acpi_madt_lapic_t *)current, 1, 17);
	current += acpi_create_madt_lapic((acpi_madt_lapic_t *)current, 2, 18);
	current += acpi_create_madt_lapic((acpi_madt_lapic_t *)current, 3, 19);
	
	/* Write 8111 IOAPIC */
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *)current, 1,
			IO_APIC_ADDR, 0);

        /* Write the first 8131 IOAPICs */
	for(i = 0; i < 2; i++) {
		if (dev = dev_find_device(PCI_VENDOR_ID_AMD, 0x7451, dev)){
        		ACPI_WRITE_MADT_IOAPIC(dev, ioapic_nr);
			ioapic_nr++;
		}
	}

	/* Write the 8132 IOAPICs if they exist */	
	for(i = 0; i < 4; i++) {
		if (dev = dev_find_device(PCI_VENDOR_ID_AMD, 0x7459, dev)){
        		ACPI_WRITE_MADT_IOAPIC(dev, ioapic_nr);
			ioapic_nr++;
		}
	}

	/* in the event there were no 8132s reset dev and look for the 8131s */
        /* first skip the onboard 8131 */
        dev=dev_find_device(PCI_VENDOR_ID_AMD, 0x7451, 0);
        dev=dev_find_device(PCI_VENDOR_ID_AMD, 0x7451, dev);

        /* Write all 8131 IOAPICs */
	while((dev = dev_find_device(PCI_VENDOR_ID_AMD, 0x7451, dev))) {
        	ACPI_WRITE_MADT_IOAPIC(dev, ioapic_nr);
		ioapic_nr++;
	}

	current += acpi_create_madt_irqoverride( (acpi_madt_irqoverride_t *)
			current, 1, 0, 2, 0 );

	current += acpi_create_madt_irqoverride( (acpi_madt_irqoverride_t *)
			current, 1, 0, 2, 0 );

	return current;
}


/* The next two tables are used by our DSDT and are freely defined
 * here. This construct is used because the bus numbers containing 
 * the 8131 bridges may vary so that we need to pass LinuxBIOS 
 * knowledge into the DSDT
 */
typedef struct lnxc_busses {
	u8 secondary;
	u8 subordinate;
} acpi_lnxb_busses_t;

typedef struct acpi_lnxb {
	struct acpi_table_header header;
	acpi_lnxb_busses_t busses[5];
} acpi_lnxb_t;

/* special linuxbios acpi table */
void acpi_create_lnxb(acpi_lnxb_t *lnxb)
{
	device_t dev;
	int busidx=0;
	
        acpi_header_t *header=&(lnxb->header);

        /* fill out header fields */
        memcpy(header->signature, "LNXB", 4);
        memcpy(header->oem_id, OEM_ID, 6);
        memcpy(header->oem_table_id, "LNXBIOS ", 8);
        memcpy(header->asl_compiler_id, ASLC, 4);

        header->length = sizeof(acpi_lnxb_t);
        header->revision = 1;

	/* 
	 * Write external 8131 bus ranges 
	 */
	/* first skip the onboard 8131 */
	dev=dev_find_device(PCI_VENDOR_ID_AMD, 0x7450, 0);
	dev=dev_find_device(PCI_VENDOR_ID_AMD, 0x7450, dev);
	/* now look at the last 8131 in each chain, 
	 * as it contains the valid bus ranges
	 */
	/* Add a check for 8132 devices, device ID == 0x7458 */
	while((dev = dev_find_device(PCI_VENDOR_ID_AMD, 0x7458, dev)) 
			&& busidx<5 ) {
		int subu, fn, slot;
		acpi_lnxb_busses_t *busses;
		
		if(PCI_SLOT(dev->path.u.pci.devfn)!=4)
			continue;
		
		busses=&(lnxb->busses[busidx]);
		lnxb->busses[busidx].secondary	 = dev->bus->secondary;
		lnxb->busses[busidx].subordinate = 
			pci_read_config8(dev, PCI_SUBORDINATE_BUS);
#if 0	
	        /*  SDE-test print out lnbx table values  */
		printk_info("ACPI: 7458 lnxb value, secondary %lx, subordinate %1x \n", 
                	lnxb->busses[busidx].secondary, lnxb->busses[busidx].subordinate);
#endif		
		busidx++;
	}
	dev=dev_find_device(PCI_VENDOR_ID_AMD, 0x7450, 0);
	dev=dev_find_device(PCI_VENDOR_ID_AMD, 0x7450, dev);
	while((dev = dev_find_device(PCI_VENDOR_ID_AMD, 0x7450, dev)) 
			&& busidx<5 ) {
		int subu, fn, slot;
		acpi_lnxb_busses_t *busses;
		
		if(PCI_SLOT(dev->path.u.pci.devfn)!=4)
			continue;
		
		busses=&(lnxb->busses[busidx]);
		lnxb->busses[busidx].secondary	 = dev->bus->secondary;
		lnxb->busses[busidx].subordinate = 
			pci_read_config8(dev, PCI_SUBORDINATE_BUS);
#if 0	
	        /*  SDE-test print out lnbx table values  */
		printk_info("ACPI: 7450 lnxb value, secondary %lx, subordinate %1x \n", 
                	lnxb->busses[busidx].secondary, lnxb->busses[busidx].subordinate);
#endif		
		busidx++;
	}
	header->checksum = acpi_checksum((void *)lnxb, sizeof(acpi_lnxb_t));
}



unsigned long write_acpi_tables(unsigned long start)
{
	unsigned long current;
	acpi_rsdp_t *rsdp;
	acpi_rsdt_t *rsdt;
	acpi_hpet_t *hpet;
	acpi_madt_t *madt;
	acpi_fadt_t *fadt;
	acpi_facs_t *facs;
	acpi_lnxb_t *lnxb;
	acpi_header_t *dsdt;

	/* Align ACPI tables to 16byte */
	start   = ( start + 0x0f ) & -0x10;
	current = start;
	
	printk_info("ACPI: Writing ACPI tables at %lx...\n", start);

	/* We need at least an RSDP and an RSDT Table */
	rsdp = (acpi_rsdp_t *) current;
	current += sizeof(acpi_rsdp_t);
	rsdt = (acpi_rsdt_t *) current;
	current += sizeof(acpi_rsdt_t);

	/* clear all table memory */
	memset((void *)start, 0, current - start);
	
	acpi_write_rsdp(rsdp, rsdt);
	acpi_write_rsdt(rsdt);
	
	/*
	 * We explicitly add these tables later on:
	 */
	printk_debug("ACPI:    * HPET\n");

	hpet = (acpi_hpet_t *) current;
	current += sizeof(acpi_hpet_t);
	acpi_create_hpet(hpet);
	acpi_add_table(rsdt,hpet);

	/* If we want to use HPET Timers Linux wants an MADT */
	printk_debug("ACPI:    * MADT\n");

	madt = (acpi_madt_t *) current;
	acpi_create_madt(madt);
	current+=madt->header.length;
	acpi_add_table(rsdt,madt);

	printk_debug("ACPI:    * LNXB\n");
	lnxb=(acpi_lnxb_t *)current;
	current += sizeof(acpi_facs_t);
	acpi_create_lnxb(lnxb);

	printk_debug("ACPI:    * FACS\n");
	facs = (acpi_facs_t *) current;
	current += sizeof(acpi_facs_t);
	acpi_create_facs(facs);

	dsdt = (acpi_header_t *)current;
	current += ((acpi_header_t *)AmlCode)->length;
	memcpy((void *)dsdt,(void *)AmlCode, \
			((acpi_header_t *)AmlCode)->length);

	/* fix up dsdt */
	((u32 *)dsdt)[11]=((u32)lnxb)+sizeof(acpi_header_t);
	
	/* recalculate checksum */
	dsdt->checksum = 0;
	dsdt->checksum = acpi_checksum(dsdt,dsdt->length);
	printk_debug("ACPI:    * DSDT @ %08x Length %x\n",dsdt,dsdt->length);
	printk_debug("ACPI:    * FADT\n");
	
	fadt = (acpi_fadt_t *) current;
	current += sizeof(acpi_fadt_t);

	acpi_create_fadt(fadt,facs,dsdt);
	acpi_add_table(rsdt,fadt);

	printk_info("ACPI: done.\n");
	return current;
}

