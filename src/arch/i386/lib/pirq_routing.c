#include <printk.h>
#include <pci.h>
#include <arch/pirq_routing.h>
#include <string.h>

#ifdef DEBUG
void check_pirq_routing_table(void)
{
	const u8 *addr;
	const struct irq_routing_table *rt;
	int i;
	u8 sum;

	printk_info("Checking IRQ routing tables...");

	rt = &intel_irq_routing_table;
	addr = (u8 *)rt;

	sum = 0;
	for (i = 0; i < rt->size; i++)
		sum += addr[i];

	printk_debug("%s:%6d:%s() - irq_routing_table located at: 0x%p\n",
	    __FILE__, __LINE__, __FUNCTION__, addr);

	sum = (unsigned char)(rt->checksum-sum);

	if (sum != rt->checksum) {
		printk_warning("%s:%6d:%s() - "
		       "checksum is: 0x%02x but should be: 0x%02x\n",
		       __FILE__, __LINE__, __FUNCTION__, rt->checksum, sum);
	}

	if (rt->signature != PIRQ_SIGNATURE || rt->version != PIRQ_VERSION ||
	    rt->size % 16 || rt->size < sizeof(struct irq_routing_table)) {
		printk_warning("%s:%6d:%s() - "
		       "Interrupt Routing Table not valid\n",
		       __FILE__, __LINE__, __FUNCTION__);
		return;
	}

	sum = 0;
	for (i=0; i<rt->size; i++)
		sum += addr[i];

	if (sum) {
		printk_warning("%s:%6d:%s() - "
		       "checksum error in irq routing table\n",
		       __FILE__, __LINE__, __FUNCTION__);
	}

	printk_info("done.\n");
}
#endif

unsigned long copy_pirq_routing_table(unsigned long addr)
{
	/* Align the table to be 16 byte aligned. */
	addr += 15;
	addr &= ~15;

	/* This table must be betweeen 0xf0000 & 0x100000 */
	printk_info("Copying IRQ routing tables...");
	memcpy((void *)addr, &intel_irq_routing_table, intel_irq_routing_table.size);
	printk_info("done.\n");

	return addr + intel_irq_routing_table.size;
}
