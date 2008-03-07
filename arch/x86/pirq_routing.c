/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000 Ollie Lo, Silicon Integrated Systems
 * Copyright (C) 2000 Ron Minnich
 * Copyright (C) 2001 Eric Biederman
 * Copyright (C) 2002 Andrew Ip
 * Copyright (C) 2008 Ron Minnich
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
#include <types.h>
#include <string.h>
#include <lar.h>
#include <console.h>
#include <device/device.h>
#include <tables.h>
#include <pirq_routing.h>

static void check_pirq_routing_table(struct irq_routing_table *rt)
{
	u8 *addr = (u8 *)rt;
	u8 sum=0;
	int i;

	printk(BIOS_INFO, "Checking IRQ routing table consistency...\n");

	for (i = 0; i < rt->size; i++)
		sum += addr[i];

	printk(BIOS_DEBUG, "%s() - irq_routing_table located at: 0x%p\n",
		     __FUNCTION__, addr);

	
	sum = rt->checksum - sum;

	if (sum != rt->checksum) {
		printk(BIOS_WARNING, "%s:%6d:%s() - "
			       "checksum is: 0x%02x but should be: 0x%02x\n",
			       __FILE__, __LINE__, __FUNCTION__, rt->checksum, sum);
		rt->checksum = sum;
	}

	if (rt->signature != PIRQ_SIGNATURE || rt->version != PIRQ_VERSION ||
	    rt->size % 16 ) {
		printk(BIOS_WARNING, "%s:%6d:%s() - "
			       "Interrupt Routing Table not valid\n",
			       __FILE__, __LINE__, __FUNCTION__);
		return;
	}

	sum = 0;
	for (i=0; i<rt->size; i++)
		sum += addr[i];

	if (sum) {
		printk(BIOS_WARNING, "%s:%6d:%s() - "
			       "checksum error in irq routing table\n",
			       __FILE__, __LINE__, __FUNCTION__);
	}

	printk(BIOS_INFO, "done.\n");
}

static int verify_copy_pirq_routing_table(unsigned long addr)
{
	int i;
	u8 *rt_orig, *rt_curr;

	rt_curr = (u8*)addr;
	rt_orig = (u8*)&intel_irq_routing_table;
	printk(BIOS_INFO, "Verifing copy of IRQ routing tables at 0x%lux...", addr);
	for (i = 0; i < intel_irq_routing_table.size; i++) {
		if (*(rt_curr + i) != *(rt_orig + i)) {
			printk(BIOS_INFO, "failed\n");
			return -1;
		}
	}
	printk(BIOS_INFO, "done\n");
	
	check_pirq_routing_table((struct irq_routing_table *)addr);
	
	return 0;
}
unsigned long copy_pirq_routing_table(unsigned long addr)
{
	/* Align the table to be 16 byte aligned. */
	addr += 15;
	addr &= ~15;

	/* This table must be betweeen 0xf0000 & 0x100000 */
	printk(BIOS_INFO, "Copying IRQ routing tables to 0x%lux...", addr);
	memcpy((void *)addr, &intel_irq_routing_table, intel_irq_routing_table.size);
	printk(BIOS_INFO, "done.\n");
	verify_copy_pirq_routing_table(addr);
	return addr + intel_irq_routing_table.size;
}

