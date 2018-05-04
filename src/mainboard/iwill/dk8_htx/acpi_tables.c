/*
 * Island Aruma ACPI support
 * written by Stefan Reinauer <stepan@openbios.org>
 *  (C) 2005 Stefan Reinauer
 *
 *
 *  Copyright 2005 AMD
 *  2005.9 yhlu modify that to more dynamic for AMD Opteron Based MB
 */

#include <console/console.h>
#include <string.h>
#include <arch/acpi.h>
#include <arch/ioapic.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <cpu/amd/amdk8_sysconf.h>
#include "northbridge/amd/amdk8/acpi.h"
#include "mb_sysconf.h"
#include "mainboard.h"
#include <cbfs.h>

unsigned long acpi_fill_madt(unsigned long current)
{
	unsigned int gsi_base = 0x18;

	struct mb_sysconf_t *m;

	get_bus_conf();

	m = sysconf.mb;

	/* create all subtables for processors */
	current = acpi_create_madt_lapics(current);

	/* Write 8111 IOAPIC */
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *)current, m->apicid_8111,
			IO_APIC_ADDR, 0);

	/* Write all 8131 IOAPICs */
	{
		struct device *dev;
		struct resource *res;
		dev = dev_find_slot(m->bus_8132_0, PCI_DEVFN((sysconf.hcdn[0]&0xff), 1));
		if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_0);
			if (res) {
				current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *)current, m->apicid_8132_1,
					res->base, gsi_base );
				gsi_base+=4;

			}
		}
		dev = dev_find_slot(m->bus_8132_0, PCI_DEVFN((sysconf.hcdn[0] & 0xff)+1, 1));
		if (dev) {
			res = find_resource(dev, PCI_BASE_ADDRESS_0);
			if (res) {
				current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *)current, m->apicid_8132_2,
					res->base, gsi_base );
				gsi_base+=4;
			}
		}

		int i;
		int j = 0;

		for(i = 1; i< sysconf.hc_possible_num; i++) {
			unsigned d = 0;
			if(!(sysconf.pci1234[i] & 0x1) ) continue;
			// 8131 need to use +4

			switch (sysconf.hcid[i]) {
			case 1:
				d = 7;
				break;
			case 3:
				d = 4;
				break;
			}
			switch (sysconf.hcid[i]) {
			case 1:
			case 3:
				dev = dev_find_slot(m->bus_8132a[j][0], PCI_DEVFN(m->sbdn3a[j], 1));
				if (dev) {
					res = find_resource(dev, PCI_BASE_ADDRESS_0);
					if (res) {
						current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *)current, m->apicid_8132a[j][0],
							res->base, gsi_base );
						gsi_base+=d;
					}
				}
				dev = dev_find_slot(m->bus_8132a[j][0], PCI_DEVFN(m->sbdn3a[j]+1, 1));
				if (dev) {
					res = find_resource(dev, PCI_BASE_ADDRESS_0);
					if (res) {
						current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *)current, m->apicid_8132a[j][1],
							res->base, gsi_base );
						gsi_base+=d;

					}
				}
				break;
			}

			j++;
		}

	}

	current += acpi_create_madt_irqoverride( (acpi_madt_irqoverride_t *)
			current, 0, 0, 2, 5 );
		/* 0: mean bus 0--->ISA */
		/* 0: PIC 0 */
		/* 2: APIC 2 */
		/* 5 mean: 0101 --> Edge-triggered, Active high*/


		/* create all subtables for processors */
	current = acpi_create_madt_lapic_nmis(current, 5, 1);
		/* 1: LINT1 connect to NMI */


	return current;
}

unsigned long mainboard_write_acpi_tables(struct device *device,
					  unsigned long start,
					  acpi_rsdp_t *rsdp)
{
	unsigned long current;
	acpi_header_t *ssdtx;
	const void *p;
	size_t p_size;

	int i;

	get_bus_conf(); //it will get sblk, pci1234, hcdn, and sbdn

	/* Align ACPI tables to 16byte */
	start   = ALIGN(start, 16);
	current = start;

	//same htio, but different position? We may have to copy, change HCIN, and recalculate the checknum and add_table

	for(i = 1; i < sysconf.hc_possible_num; i++) {  // 0: is hc sblink
		const char *file_name;
		if((sysconf.pci1234[i] & 1) != 1 ) continue;
		uint8_t c;
		if(i < 7) {
			c = (uint8_t) ('4' + i - 1);
		}
		else {
			c = (uint8_t) ('A' + i - 1 - 6);
		}
		printk(BIOS_DEBUG, "ACPI:    * SSDT for PCI%c Aka hcid = %d\n", c, sysconf.hcid[i]); //pci0 and pci1 are in dsdt
		current = ALIGN(current, 8);
		ssdtx = (acpi_header_t *)current;
		switch(sysconf.hcid[i]) {
		case 1: //8132
			file_name = CONFIG_CBFS_PREFIX "/ssdt2.aml";
			break;
		case 2: //8151
			file_name = CONFIG_CBFS_PREFIX "/ssdt3.aml";
			break;
		case 3: //8131
			file_name = CONFIG_CBFS_PREFIX "/ssdt4.aml";
			break;
		default:
			//HTX no io apic
			file_name = CONFIG_CBFS_PREFIX "/ssdt5.aml";
			break;
		}
		p = cbfs_boot_map_with_leak(
					  file_name,
					  CBFS_TYPE_RAW, &p_size);
		if (!p || p_size < sizeof(acpi_header_t))
			continue;

		memcpy(ssdtx, p, sizeof(acpi_header_t));
		current += ssdtx->length;
		memcpy(ssdtx, p, ssdtx->length);
		update_ssdtx((void *)ssdtx, i);
		ssdtx->checksum = 0;
		ssdtx->checksum = acpi_checksum((unsigned char *)ssdtx,ssdtx->length);
		acpi_add_table(rsdp,ssdtx);
	}

	return current;
}
