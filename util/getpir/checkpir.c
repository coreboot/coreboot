/* checkpir.c : This software is released under GPL
 * For coreboot use only
 * Aug 26 2001 , Nikolai Vladychevski, <niko@isl.net.mx>
 */

#include <stdio.h>
#include "pirq_routing.h"
#include "checksum.h"

struct irq_routing_table *rt;

int main(void)
{
	uint8_t sum, newsum;

	rt = (struct irq_routing_table *) &intel_irq_routing_table;
	sum = calc_checksum(rt);

	printf("Validating checksum, file: irq_tables.c that was in ./ at compile time...\n");
	printf("(no other tests are done)\n");

	if (!sum) {
		printf("Checksum for IRQ Routing table is ok. You can use irq_tables.c in coreboot now.\n");
	} else {
		newsum = rt->checksum - sum;
		printf("BAD CHECKSUM for IRQ Routing table !!!!\n");
		printf("If you want to make it valid, change the checksum to: %#x\n",
		       newsum);
	}
	return 0;
}
