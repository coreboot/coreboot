#include <subr.h>
#include <arch/pirq_routing.h>

/*
 * This table must be located between 0x000f0000 and 0x000fffff.
 * By defining it as a const it gets located in the code segment
 * and therefore inside the necessary 64K block.   -tds
 */

#define CHECKSUM 0xd2

const struct irq_routing_table intel_irq_routing_table = {
	PIRQ_SIGNATURE, /* u32 signature */
	PIRQ_VERSION,   /* u16 version   */
	32+16*3,        /* u16 size           - size of entire table struct */
	0,              /*  u8 rtr_bus        - router bus */
	0x90,		/*  u8 rtr_devfn      - router devfn */
	0x1e00,         /* u16 exclusive_irqs - mask of IRQs for PCI use */
	0x100b,         /* u16 rtr_vendor     - router vendor id */
	0x0500,         /* u16 rtr_devfn      - router device id */
	0,              /*  u8 miniport_data  - "crap" */
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* u8 rfu[11] */
	CHECKSUM, /*  u8 checksum       - mod 256 checksum must give zero */
	/* struct irq_info slots[0] */
	{
		/* bus, devfn, {link, bitmap}, {link, bitmap}, {link, bitmap}, {link, bitmap}, slot, rfu  */
		/* USB */
		{0x00, 0x98, {{0x40, 0xdef8}, {0x41, 0xdef8}, {0x42, 0xdef8}, {0x43, 0xdef8}}, 0, 0 },
		/* eth0 */
		{0x00, 0x78, {{0x41, 0xdef8}, {0, 0}, {0, 0}, {0, 0}}, 0x01, 0x00},
		/* eth1 */
		{0x00, 0x80, {{0x41, 0xdef8}, {0, 0}, {0, 0}, {0, 0}}, 0x02, 0x00},
	}
};

#ifdef CALC
#include <stdio.h>
int main()
{
	int i;
	unsigned char *p = (unsigned char *)&intel_irq_routing_table;
	unsigned char sum = -CHECKSUM;
	for (i = 0; i < sizeof(intel_irq_routing_table); ++i) {
		sum += *p++;
	}
	printf("#define CHECKSUM 0x%x\n", (-sum) & 0xff);
	return 0;
}
#endif

/* 
gcc -D CALC -I ../../../arch/i386/include -I ../../../include irq_tables.c  
*/
