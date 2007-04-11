#include "pirq_routing.h"
#include "checksum.h"

int calc_checksum(struct irq_routing_table *rt)
{
	long i;
	uint8_t *addr, sum = 0;

	addr = (uint8_t *) rt;
	for (i = 0; i < rt->size; i++)
		sum += addr[i];
	return (sum);
}
