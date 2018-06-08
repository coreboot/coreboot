/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Copyright (C) 2010 Stefan Reinauer <stepan@coreboot.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <console/console.h>
#include <compiler.h>
#include <arch/pirq_routing.h>
#include <string.h>
#include <device/pci.h>
#include <arch/pirq_routing.h>

void __weak pirq_assign_irqs(const unsigned char pirq[CONFIG_MAX_PIRQ_LINKS])
{
}

static void check_pirq_routing_table(struct irq_routing_table *rt)
{
	uint8_t *addr = (uint8_t *)rt;
	uint8_t sum = 0;
	int i;

	printk(BIOS_INFO, "Checking Interrupt Routing Table consistency...\n");

	if (sizeof(struct irq_routing_table) != rt->size) {
		printk(BIOS_WARNING,
			"Inconsistent Interrupt Routing Table size (0x%x/0x%x).\n",
			(unsigned int) sizeof(struct irq_routing_table),
			rt->size);
		rt->size = sizeof(struct irq_routing_table);
	}

	for (i = 0; i < rt->size; i++)
		sum += addr[i];

	printk(BIOS_DEBUG, "%s(): Interrupt Routing Table located at %p.\n",
		     __func__, addr);


	sum = rt->checksum - sum;

	if (sum != rt->checksum) {
		printk(BIOS_WARNING,
			"Interrupt Routing Table checksum is: 0x%02x but should be: 0x%02x.\n",
			rt->checksum, sum);
		rt->checksum = sum;
	}

	if (rt->signature != PIRQ_SIGNATURE || rt->version != PIRQ_VERSION ||
	    rt->size % 16) {
		printk(BIOS_WARNING, "Interrupt Routing Table not valid.\n");
		return;
	}

	sum = 0;
	for (i = 0; i < rt->size; i++)
		sum += addr[i];

	/* We're manually fixing the checksum above. This warning can probably
	 * never happen because if the target location is read-only this
	 * function would have bailed out earlier.
	 */
	if (sum) {
		printk(BIOS_WARNING, "Checksum error in Interrupt Routing Table "
				"could not be fixed.\n");
	}

	printk(BIOS_INFO, "done.\n");
}

static int verify_copy_pirq_routing_table(unsigned long addr,
	const struct irq_routing_table *routing_table)
{
	int i;
	uint8_t *rt_orig, *rt_curr;

	rt_curr = (uint8_t *)addr;
	rt_orig = (uint8_t *)routing_table;
	printk(BIOS_INFO,
		"Verifying copy of Interrupt Routing Table at 0x%08lx... ",
		addr);
	for (i = 0; i < routing_table->size; i++) {
		if (*(rt_curr + i) != *(rt_orig + i)) {
			printk(BIOS_INFO, "failed\n");
			return -1;
		}
	}
	printk(BIOS_INFO, "done\n");

	check_pirq_routing_table((struct irq_routing_table *)addr);

	return 0;
}

static u8 pirq_get_next_free_irq(u8 *pirq, u16 bitmap)
{
	int i, link;
	u8 irq = 0;
	for (i = 2; i <= 15; i++) {
		/* Can we assign this IRQ ? */
		if (!((bitmap >> i) & 1))
			continue;
		/* We can, Now let's assume we can use this IRQ */
		irq = i;
		/* And assume we have not yet routed it */
		int already_routed = 0;
		/* Have we already routed it ? */
		for (link = 0; link < CONFIG_MAX_PIRQ_LINKS; link++) {
			if (pirq[link] == irq) {
				already_routed = 1;
				break;
			}
		}
		/* If it's not yet routed, use it */
		if (!already_routed)
			break;
		/* But if it was already routed, try the next one */
		continue;
	}
	/* Now we got our IRQ */
	return irq;
}

static void pirq_route_irqs(unsigned long addr)
{
	int i, intx, num_entries;
	unsigned char irq_slot[MAX_INTX_ENTRIES];
	unsigned char pirq[CONFIG_MAX_PIRQ_LINKS];
	struct irq_routing_table *pirq_tbl;

	memset(pirq, 0, CONFIG_MAX_PIRQ_LINKS);

	pirq_tbl = (struct irq_routing_table *)(addr);
	num_entries = (pirq_tbl->size - 32) / 16;

	/* Set PCI IRQs. */
	for (i = 0; i < num_entries; i++) {

		printk(BIOS_DEBUG, "PIRQ Entry %d Dev/Fn: %X Slot: %d\n", i,
			pirq_tbl->slots[i].devfn >> 3, pirq_tbl->slots[i].slot);

		for (intx = 0; intx < MAX_INTX_ENTRIES; intx++) {

			int link = pirq_tbl->slots[i].irq[intx].link;
			int bitmap = pirq_tbl->slots[i].irq[intx].bitmap;
			int irq = 0;

			printk(BIOS_DEBUG, "INT: %c link: %x bitmap: %x  ",
				'A' + intx, link, bitmap);

			if (!bitmap || !link || link > CONFIG_MAX_PIRQ_LINKS) {

				printk(BIOS_DEBUG, "not routed\n");
				irq_slot[intx] = irq;
				continue;
			}

			/* yet not routed */
			if (!pirq[link - 1]) {
				irq = pirq_get_next_free_irq(pirq, bitmap);
				if (irq)
					pirq[link - 1] = irq;
			} else
				irq = pirq[link - 1];

			printk(BIOS_DEBUG, "IRQ: %d\n", irq);
			irq_slot[intx] = irq;
		}

		/* Bus, device, slots IRQs for {A,B,C,D}. */
		pci_assign_irqs(pirq_tbl->slots[i].bus,
			pirq_tbl->slots[i].devfn >> 3, irq_slot);
	}

	for (i = 0; i < CONFIG_MAX_PIRQ_LINKS; i++)
		printk(BIOS_DEBUG, "PIRQ%c: %d\n", i + 'A',  pirq[i]);

	pirq_assign_irqs(pirq);
}

unsigned long copy_pirq_routing_table(unsigned long addr,
	const struct irq_routing_table *routing_table)
{
	/* Align the table to be 16 byte aligned. */
	addr = ALIGN(addr, 16);

	/* This table must be between 0xf0000 & 0x100000 */
	printk(BIOS_INFO, "Copying Interrupt Routing Table to 0x%08lx... ",
		addr);
	memcpy((void *)addr, routing_table, routing_table->size);
	printk(BIOS_INFO, "done.\n");
	if (IS_ENABLED(CONFIG_DEBUG_PIRQ))
		verify_copy_pirq_routing_table(addr, routing_table);
	if (IS_ENABLED(CONFIG_PIRQ_ROUTE))
		pirq_route_irqs(addr);

	return addr + routing_table->size;
}
