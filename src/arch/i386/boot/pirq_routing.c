#include <console/console.h>
#include <arch/pirq_routing.h>
#include <string.h>
#include <device/pci.h>

#if CONFIG_DEBUG_PIRQ
static void check_pirq_routing_table(struct irq_routing_table *rt)
{
	uint8_t *addr = (uint8_t *)rt;
	uint8_t sum=0;
	int i;

	printk(BIOS_INFO, "Checking Interrupt Routing Table consistency...\n");

	if (sizeof(struct irq_routing_table) != rt->size) {
		printk(BIOS_WARNING, "Inconsistent Interrupt Routing Table size (0x%x/0x%x).\n",
			       (unsigned int) sizeof(struct irq_routing_table),
			       rt->size
			);
		rt->size=sizeof(struct irq_routing_table);
	}

	for (i = 0; i < rt->size; i++)
		sum += addr[i];

	printk(BIOS_DEBUG, "%s(): Interrupt Routing Table located at %p.\n",
		     __func__, addr);


	sum = rt->checksum - sum;

	if (sum != rt->checksum) {
		printk(BIOS_WARNING, "Interrupt Routing Table checksum is: 0x%02x but should be: 0x%02x.\n",
			       rt->checksum, sum);
		rt->checksum = sum;
	}

	if (rt->signature != PIRQ_SIGNATURE || rt->version != PIRQ_VERSION ||
	    rt->size % 16 ) {
		printk(BIOS_WARNING, "Interrupt Routing Table not valid.\n");
		return;
	}

	sum = 0;
	for (i=0; i<rt->size; i++)
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

static int verify_copy_pirq_routing_table(unsigned long addr)
{
	int i;
	uint8_t *rt_orig, *rt_curr;

	rt_curr = (uint8_t*)addr;
	rt_orig = (uint8_t*)&intel_irq_routing_table;
	printk(BIOS_INFO, "Verifying copy of Interrupt Routing Table at 0x%08lx... ", addr);
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
#endif

unsigned long copy_pirq_routing_table(unsigned long addr)
{
	/* Align the table to be 16 byte aligned. */
	addr += 15;
	addr &= ~15;

	/* This table must be betweeen 0xf0000 & 0x100000 */
	printk(BIOS_INFO, "Copying Interrupt Routing Table to 0x%08lx... ", addr);
	memcpy((void *)addr, &intel_irq_routing_table, intel_irq_routing_table.size);
	printk(BIOS_INFO, "done.\n");
#if CONFIG_DEBUG_PIRQ
	verify_copy_pirq_routing_table(addr);
#endif
	pirq_routing_irqs(addr);
	return addr + intel_irq_routing_table.size;
}

#if CONFIG_PIRQ_ROUTE
void pirq_routing_irqs(unsigned long addr)
{
	int i, j, k, num_entries;
	unsigned char irq_slot[4];
	unsigned char pirq[4] = {0, 0, 0, 0};
	struct irq_routing_table *pirq_tbl;

	pirq_tbl = (struct irq_routing_table *)(addr);
	num_entries = (pirq_tbl->size - 32) / 16;

	/* Set PCI IRQs. */
	for (i = 0; i < num_entries; i++) {

		printk(BIOS_DEBUG, "PIRQ Entry %d Dev/Fn: %X Slot: %d\n", i,
			pirq_tbl->slots[i].devfn >> 3, pirq_tbl->slots[i].slot);

		for (j = 0; j < 4; j++) {

			int link = pirq_tbl->slots[i].irq[j].link;
			int bitmap = pirq_tbl->slots[i].irq[j].bitmap;
			int irq = 0;

			printk(BIOS_DEBUG, "INT: %c link: %x bitmap: %x  ",
				'A' + j, link, bitmap);

			if (!bitmap|| !link || link > 4) {

				printk(BIOS_DEBUG, "not routed\n");
				irq_slot[j] = irq;
				continue;
			}

			/* yet not routed */
			if (!pirq[link - 1]) {

				for (k = 2; k <= 15; k++) {

					if (!((bitmap >> k) & 1))
						continue;

					irq = k;

					/* yet not routed */
					if (pirq[0] != irq && pirq[1] != irq && pirq[2] != irq && pirq[3] != irq)
						break;
				}

				if (irq)
					pirq[link - 1] = irq;
			}
			else
				irq = pirq[link - 1];

			printk(BIOS_DEBUG, "IRQ: %d\n", irq);
			irq_slot[j] = irq;
		}

		/* Bus, device, slots IRQs for {A,B,C,D}. */
		pci_assign_irqs(pirq_tbl->slots[i].bus,
			pirq_tbl->slots[i].devfn >> 3, irq_slot);
	}

	printk(BIOS_DEBUG, "PIRQ1: %d\n", pirq[0]);
	printk(BIOS_DEBUG, "PIRQ2: %d\n", pirq[1]);
	printk(BIOS_DEBUG, "PIRQ3: %d\n", pirq[2]);
	printk(BIOS_DEBUG, "PIRQ4: %d\n", pirq[3]);

	pirq_assign_irqs(pirq);
}
#endif
