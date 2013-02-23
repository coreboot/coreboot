/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 AMD
 * (Written by Yinghai Lu <yinghailu@amd.com> for AMD)
 * Copyright (C) 2007 Philipp Degler <pdegler@rumms.uni-mannheim.de>
 * (Thanks to LSRA University of Mannheim for their support)
 * Copyright (C) 2008 Jonathan A. Kollasch <jakllsch@kollasch.net>
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* Documentation at: http://www.microsoft.com/hwdev/busbios/PCIIRQ.HTM */

/* This is probably not right, feel free to fix this if you don't want
 * to use the mptable.
 */

#include <console/console.h>
#include <device/pci.h>
#include <string.h>
#include <stdint.h>
#include <arch/pirq_routing.h>
#include <cpu/amd/amdk8_sysconf.h>

extern unsigned char bus_ck804[6];


/**
 * Add one line to IRQ table.
 */
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

/**
 * Create the IRQ routing table.
 * Values are derived from getpir generated code.
 */
unsigned long write_pirq_routing_table(unsigned long addr)
{

	struct irq_routing_table *pirq;
	struct irq_info *pirq_info;
	unsigned slot_num;
	uint8_t *v;

	uint8_t sum = 0;
	int i;
	unsigned sbdn;

	/* get_bus_conf() will find out all bus num and apic that share with
	 * mptable.c and mptable.c
	 */
	get_bus_conf();
	sbdn = sysconf.sbdn;

	/* Align the table to be 16 byte aligned. */
	addr += 15;
	addr &= ~15;

	/* This table must be betweeen 0xf0000 & 0x100000 */
	printk(BIOS_INFO, "Writing IRQ routing tables to 0x%lx...", addr);

	pirq = (void *)(addr);
	v = (uint8_t *) (addr);

	pirq->signature = PIRQ_SIGNATURE;
	pirq->version = PIRQ_VERSION;

	pirq->rtr_bus = bus_ck804[0];
	pirq->rtr_devfn = ((sbdn + 9) << 3) | 0;

	pirq->exclusive_irqs = 0x828;

	pirq->rtr_vendor = 0x10de;
	pirq->rtr_device = 0x005c;

	pirq->miniport_data = 0;

	memset(pirq->rfu, 0, sizeof(pirq->rfu));

	pirq_info = (void *)(&pirq->checksum + 1);
	slot_num = 0;

//Slot1 PCIE 16x
	write_pirq_info(pirq_info, bus_ck804[1], (0 << 3) | 0, 0x3, 0xdeb8, 0x4,
			0xdeb8, 0x1, 0xdeb8, 0x2, 0xdeb8, 4, 0);
	pirq_info++;
	slot_num++;

//Slot2 PCIE 1x
	write_pirq_info(pirq_info, bus_ck804[2], (0 << 3) | 0, 0x4, 0xdeb8, 0x1,
			0xdeb8, 0x2, 0xdeb8, 0x3, 0xdeb8, 5, 0);
	pirq_info++;
	slot_num++;

//Slot3 PCIE 1x
	write_pirq_info(pirq_info, bus_ck804[3], (0 << 3) | 0, 0x1, 0xdeb8, 0x2,
			0xdeb8, 0x3, 0xdeb8, 0x4, 0xdeb8, 6, 0);
	pirq_info++;
	slot_num++;

//Slot4 PCIE 4x
	write_pirq_info(pirq_info, bus_ck804[4], (0x4 << 3) | 0,
			0x2, 0xdeb8, 0x3, 0xdeb8, 0x4, 0xdeb8, 0x1, 0xdeb8,
			7, 0);
	pirq_info++;
	slot_num++;

//Slot5 - 7 PCI
	for (i = 0; i < 3; i++) {
		write_pirq_info(pirq_info, bus_ck804[5], (0 << (6 + i)) | 0,
				((i + 0) % 4) + 1, 0xdeb8,
				((i + 1) % 4) + 1, 0xdeb8,
				((i + 2) % 4) + 1, 0xdeb8,
				((i + 3) % 4) + 1, 0xdeb8, i, 0);
		pirq_info++;
		slot_num++;
	}

//pci bridge
	write_pirq_info(pirq_info, bus_ck804[0], ((sbdn + 9) << 3) | 0, 0x1,
			0xdeb8, 0x2, 0xdeb8, 0x3, 0xdeb8, 0x4, 0xdeb8, 0, 0);
	pirq_info++;
	slot_num++;

//smbus
	write_pirq_info(pirq_info, bus_ck804[0], ((sbdn + 1) << 3) | 0, 0x2,
			0xdeb8, 0, 0, 0, 0, 0, 0, 0, 0);
	pirq_info++;
	slot_num++;

//usb
	write_pirq_info(pirq_info, bus_ck804[0], ((sbdn + 2) << 3) | 0, 0x1,
			0xdeb8, 0x2, 0xdeb8, 0, 0, 0, 0, 0, 0);
	pirq_info++;
	slot_num++;

//audio
	write_pirq_info(pirq_info, bus_ck804[0], ((sbdn + 4) << 3) | 0, 0x1,
			0xdeb8, 0, 0, 0, 0, 0, 0, 0, 0);
	pirq_info++;
	slot_num++;
//sata
	write_pirq_info(pirq_info, bus_ck804[0], ((sbdn + 7) << 3) | 0, 0x1,
			0xdeb8, 0, 0, 0, 0, 0, 0, 0, 0);
	pirq_info++;
	slot_num++;
//sata
	write_pirq_info(pirq_info, bus_ck804[0], ((sbdn + 8) << 3) | 0, 0x1,
			0xdeb8, 0, 0, 0, 0, 0, 0, 0, 0);
	pirq_info++;
	slot_num++;
//nic
	write_pirq_info(pirq_info, bus_ck804[0], ((sbdn + 0xa) << 3) | 0, 0x1,
			0xdeb8, 0, 0, 0, 0, 0, 0, 0, 0);
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

#if 0
	unsigned char irq[4];
	irq[0] = 0;
	irq[1] = 0;
	irq[2] = 0;
	irq[3] = 0;

	/* Bus, device, slots IRQs for {A,B,C,D}. */

	irq[0] = 10; /* SMBus */ /* test me */
	pci_assign_irqs(bus_ck804[0], 1, irq);

	irq[0] = 10; /* USB */
	irq[1] = 10;
	pci_assign_irqs(bus_ck804[0], 2, irq);

	irq[0] = 10; /* AC97 */
	irq[1] = 0;
	pci_assign_irqs(bus_ck804[0], 4, irq);

	irq[0] = 11; /* SATA */
	pci_assign_irqs(bus_ck804[0], 7, irq);

	irq[0] = 5; /* SATA */
	pci_assign_irqs(bus_ck804[0], 8, irq);

	irq[0] = 10; /* Ethernet */
	pci_assign_irqs(bus_ck804[0], 10, irq);


	/* physical slots */

	irq[0] = 5; /* PCI E1 - x1 */
	pci_assign_irqs(bus_ck804[2], 0, irq);

	irq[0] = 11; /* PCI E2 - x16 */
	pci_assign_irqs(bus_ck804[3], 0, irq);

	/* AGP-on-PCI "AGR" ignored */

	irq[0] = 10; /* PCI1 */
	irq[1] = 11;
	irq[2] = 5;
	irq[3] = 0;
	pci_assign_irqs(bus_ck804[1], 7, irq);

	irq[0] = 11; /* PCI2 */
	irq[1] = 10;
	irq[2] = 5;
	irq[3] = 0;
	pci_assign_irqs(bus_ck804[1], 8, irq);

	irq[0] = 5; /* PCI3 */
	irq[1] = 10;
	irq[2] = 11;
	irq[3] = 0;
	pci_assign_irqs(bus_ck804[1], 9, irq);
#endif

	return (unsigned long)pirq_info;
}
