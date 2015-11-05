/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Urja Rannikko <urjaman@gmail.com>
 *
 * Code based on KFSN4-DRE irq_tables.c:
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
 *
 * Copyright (C) 2007 AMD
 * (Written by Yinghai Lu <yinghailu@amd.com> for AMD)
 * Copyright (C) 2007 Philipp Degler <pdegler@rumms.uni-mannheim.de>
 * (Thanks to LSRA University of Mannheim for their support)
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
#include <device/pci_ids.h>
#include <string.h>
#include <stdint.h>
#include <arch/pirq_routing.h>

static void write_pirq_info(struct irq_info *pirq_info, uint8_t bus,
			    uint8_t devfn, uint8_t link0, uint8_t link1,  uint8_t link2,
			    uint8_t link3, uint8_t slot, uint8_t rfu)
{
	const uint16_t valid_irqs = 0xccb8;
	pirq_info->bus = bus;
	pirq_info->devfn = devfn;
	pirq_info->irq[0].link = link0;
	pirq_info->irq[0].bitmap = link0 ? valid_irqs : 0;
	pirq_info->irq[1].link = link1;
	pirq_info->irq[1].bitmap = link1 ? valid_irqs : 0;
	pirq_info->irq[2].link = link2;
	pirq_info->irq[2].bitmap = link2 ? valid_irqs : 0;
	pirq_info->irq[3].link = link3;
	pirq_info->irq[3].bitmap = link3 ? valid_irqs : 0;
	pirq_info->slot = slot;
	pirq_info->rfu = rfu;
}


unsigned long write_pirq_routing_table(unsigned long addr)
{
	struct irq_routing_table *pirq;
	struct irq_info *pirq_info;
	uint8_t *v, sum = 0;
	unsigned int slot_num = 0;
	int i;

	/* Align the table to be 16 byte aligned. */
	addr = ALIGN_UP(addr, 16);

	/* This table must be between 0xf0000 & 0x100000. */
	printk(BIOS_INFO, "Writing IRQ routing tables to 0x%lx...", addr);

	pirq = (void *)(addr);
	v = (uint8_t *)(addr);

	pirq->signature = PIRQ_SIGNATURE;
	pirq->version = PIRQ_VERSION;
	pirq->rtr_bus = 0;
	pirq->rtr_devfn = ((0x11) << 3) | 0;
	pirq->exclusive_irqs = 0;
	pirq->rtr_vendor = PCI_VENDOR_ID_VIA;
	pirq->rtr_device = PCI_DEVICE_ID_VIA_VT8237R_LPC;
	pirq->miniport_data = 0;

	memset(pirq->rfu, 0, sizeof(pirq->rfu));

	pirq_info = (void *)(&pirq->checksum + 1);

	/* AGP Bridge. */
	write_pirq_info(pirq_info, 0, (0x01 << 3) | 0,  1, 2, 0, 0,  0, 0);
	pirq_info++;
	slot_num++;

	/* ISA Bridge + AC97 + MC97 */
	write_pirq_info(pirq_info, 0, (0x11 << 3) | 0,  0, 0, 3, 0,  0, 0);
	pirq_info++;
	slot_num++;

	/* PATA and SATA. */
	write_pirq_info(pirq_info, 0, (0x0f << 3) | 0,  1, 2, 3, 5,  0, 0);
	pirq_info++;
	slot_num++;

	/* USB (UHCI and EHCI) */
	write_pirq_info(pirq_info, 0, (0x10 << 3) | 0,  1, 2, 3, 5,  0, 0);
	pirq_info++;
	slot_num++;

	/* 5 PCI Slots */
	write_pirq_info(pirq_info, 0, (0x0b << 3) | 0,  1, 2, 3, 5,  1, 0);
	pirq_info++;
	slot_num++;

	write_pirq_info(pirq_info, 0, (0x0c << 3) | 0,  2, 3, 5, 1,  2, 0);
	pirq_info++;
	slot_num++;

	write_pirq_info(pirq_info, 0, (0x0d << 3) | 0,  3, 5, 1, 2,  3, 0);
	pirq_info++;
	slot_num++;

	write_pirq_info(pirq_info, 0, (0x0e << 3) | 0,  5, 1, 2, 3,  4, 0);
	pirq_info++;
	slot_num++;

	write_pirq_info(pirq_info, 0, (0x09 << 3) | 0,  1, 2, 3, 5,  5, 0);
	pirq_info++;
	slot_num++;

	/* Ethernet */
	write_pirq_info(pirq_info, 0, (0x0a << 3) | 0,  2, 0, 0, 0,  0, 0);
	pirq_info++;
	slot_num++;

	/* AGP Slot. */
	write_pirq_info(pirq_info, 1, (0x00 << 3) | 0,  1, 2, 0, 0,  6, 0);
	pirq_info++;
	slot_num++;

	pirq->size = 32 + 16 * slot_num;

	for (i = 0; i < pirq->size; i++)
		sum += v[i];

	sum = pirq->checksum - sum;
	if (sum != pirq->checksum)
		pirq->checksum = sum;

	printk(BIOS_INFO, "done.\n");

	/* Call copy for side effects: setting PCI IRQ registers. Hack? */
	return copy_pirq_routing_table(addr, pirq);
}
