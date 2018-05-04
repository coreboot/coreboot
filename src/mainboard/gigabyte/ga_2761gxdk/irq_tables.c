/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 AMD
 * Written by Yinghai Lu <yinghailu@amd.com> for AMD.
 * Copyright (C) 2007 Silicon Integrated Systems Corp. (SiS)
 * Written by Morgan Tsai <my_tsai@sis.com> for SiS.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <device/pci.h>
#include <string.h>
#include <stdint.h>
#include <arch/pirq_routing.h>
#include <device/pci_ids.h>
#include <cpu/amd/amdk8_sysconf.h>

static void write_pirq_info(struct irq_info *pirq_info, uint8_t bus,
			    uint8_t devfn, uint8_t link0, uint16_t bitmap0,
			    uint8_t link1, uint16_t bitmap1, uint8_t link2,
			    uint16_t bitmap2, uint8_t link3, uint16_t bitmap3,
			    uint8_t slot, uint8_t rfu)
{
	pirq_info->bus = bus;
	pirq_info->devfn = devfn;
	pirq_info->irq[0].link = link0;
	pirq_info->irq[0].bitmap = bitmap0;
	pirq_info->irq[1].link = link1;
	pirq_info->irq[1].bitmap = bitmap1;
	pirq_info->irq[2].link = link2;
	pirq_info->irq[2].bitmap = bitmap2;
	pirq_info->irq[3].link = link3;
	pirq_info->irq[3].bitmap = bitmap3;
	pirq_info->slot = slot;
	pirq_info->rfu = rfu;
}

extern unsigned char bus_sis966[8];	//1

unsigned long write_pirq_routing_table(unsigned long addr)
{

	struct irq_routing_table *pirq;
	struct irq_info *pirq_info;
	unsigned slot_num;
	uint8_t *v;
	unsigned sbdn;

	uint8_t sum = 0;
	int i;

	get_bus_conf();		// it will find out all bus num and apic that share with mptable.c and mptable.c and acpi_tables.c
	sbdn = sysconf.sbdn;

	/* Align the table to be 16 byte aligned. */
	addr += 15;
	addr &= ~15;

	/* This table must be between 0xf0000 & 0x100000 */
	printk(BIOS_INFO, "Writing IRQ routing tables to 0x%lx...", addr);

	pirq = (void *)(addr);
	v = (uint8_t *) (addr);

	pirq->signature = PIRQ_SIGNATURE;
	pirq->version = PIRQ_VERSION;

	pirq->rtr_bus = 0;
	pirq->rtr_devfn = PCI_DEVFN(2, 0);

	pirq->exclusive_irqs = 0;

	pirq->rtr_vendor = PCI_VENDOR_ID_SIS;
	pirq->rtr_device = PCI_DEVICE_ID_SIS_SIS966_LPC;

	pirq->miniport_data = 0;

	memset(pirq->rfu, 0, sizeof(pirq->rfu));

	pirq_info = (void *)(&pirq->checksum + 1);
	slot_num = 0;

	write_pirq_info(pirq_info, 0, PCI_DEVFN(2, 0), 0x1, 0xdef8, 0x2, 0xdef8,
			0x3, 0xdef8, 0x4, 0xdef8, 0, 0);
	pirq_info++;
	slot_num++;

	pirq->size = 32 + 16 * slot_num;

	for (i = 0; i < pirq->size; i++)
		sum += v[i];

	sum = pirq->checksum - sum;

	if (sum != pirq->checksum) {
		pirq->checksum = sum;
	}

	printk(BIOS_INFO, "done.\n");

	{
		struct device *dev;
		dev = dev_find_slot(0, PCI_DEVFN(2, 0));
		if (dev) {
			/* initialize PCI interupts - these assignments depend
			   on the PCB routing of PINTA-D

			   PINTA = IRQ10
			   PINTB = IRQ11
			   PINTC = NA
			   PINTD = IRQ10
			   PINTE = IRQ11
			   PINTF = IRQ5
			   PINTG = NA
			   PINTH = IRQ7

			 */
			uint8_t reg[8] = {
			    0x41, 0x42, 0x43, 0x44, 0x60, 0x61, 0x62, 0x63
			};
			uint8_t irq[8] = {
			    0x0A, 0X0B, 0X0, 0X0a, 0X0B, 0X05, 0X0, 0X07
			};

			for (i = 0; i < 8; i++)
				pci_write_config8(dev, reg[i], irq[i]);
		}

		printk(BIOS_DEBUG, "Setting Onboard SiS Southbridge\n");

		dev = dev_find_slot(0, PCI_DEVFN(2, 5));	// 5513 (IDE)
		pci_write_config8(dev, 0x3C, 0x0A);
		dev = dev_find_slot(0, PCI_DEVFN(3, 0));	// USB 1.1
		pci_write_config8(dev, 0x3C, 0x0B);
		dev = dev_find_slot(0, PCI_DEVFN(3, 1));	// USB 1.1
		pci_write_config8(dev, 0x3C, 0x05);
		dev = dev_find_slot(0, PCI_DEVFN(3, 3));	// USB 2.0
		pci_write_config8(dev, 0x3C, 0x07);
		dev = dev_find_slot(0, PCI_DEVFN(4, 0));	// 191 (LAN)
		pci_write_config8(dev, 0x3C, 0x0A);
		dev = dev_find_slot(0, PCI_DEVFN(5, 0));	// 1183 (SATA)
		pci_write_config8(dev, 0x3C, 0x0B);
		dev = dev_find_slot(0, PCI_DEVFN(6, 0));	// PCI-E
		pci_write_config8(dev, 0x3C, 0x0A);
		dev = dev_find_slot(0, PCI_DEVFN(7, 0));	// PCI-E
		pci_write_config8(dev, 0x3C, 0x0A);
		dev = dev_find_slot(0, PCI_DEVFN(15, 0));	// Azalia
		pci_write_config8(dev, 0x3C, 0x05);
	}

	printk(BIOS_DEBUG, "pirq routing table, size=%d\n", pirq->size);
	for (i = 0; i < pirq->size; i += 4)
		printk(BIOS_DEBUG, "%.2x%.2x%.2x%.2x\n", v[i + 3], v[i + 2],
		       v[i + 1], v[i]);

	return (unsigned long)pirq_info;

}
