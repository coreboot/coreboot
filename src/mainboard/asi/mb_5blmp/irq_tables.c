/* TODO: This is currently copied from the IEI NOVA-4899R target, but it's
 * quite surely wrong for this board. It gets me further in the boot process
 * than using no irq_tables.c file at all, though!
 */

/* TODO: Add license header. */

#include <arch/pirq_routing.h>

static const struct irq_routing_table intel_irq_routing_table = {
	PIRQ_SIGNATURE,  /* u32 signature */
	PIRQ_VERSION,    /* u16 version   */
	32+16*CONFIG_IRQ_SLOT_COUNT,	 /* there can be total CONFIG_IRQ_SLOT_COUNT devices on the bus */
	0x00,		 /* Where the interrupt router lies (bus) */
	(0x12<<3)|0x0,   /* Where the interrupt router lies (dev) */
	0xe00,		 /* IRQs devoted exclusively to PCI usage */
	0x1078,		 /* Vendor */
	0x0002,		 /* Device */
	0,		 /* Miniport data */
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* u8 rfu[11] */
	0x2d,         /*  u8 checksum , this hase to set to some value that would give 0 after the sum of all bytes for this structure (including checksum) */
	{
		/* bus,     dev|fn,   {link, bitmap}, {link, bitmap}, {link, bitmap}, {link, bitmap},  slot, rfu */
		// USB
		{0x00,(0x13<<3)|0x0, {{0x01, 0xdeb8}, {0x00, 0xdeb8}, {0x00, 0xdeb8}, {0x00, 0x0deb8}}, 0x0, 0x0},
		// eth0
		{0x00,(0x0a<<3)|0x0, {{0x02, 0xdeb8}, {0x03, 0xdeb8}, {0x04, 0xdeb8}, {0x00, 0x0deb8}}, 0x0, 0x0},//0x3, 0x0},
		// eth1
		{0x00,(0x0b<<3)|0x0, {{0x03, 0xdeb8}, {0x04, 0xdeb8}, {0x02, 0xdeb8}, {0x00, 0x0deb8}}, 0x0, 0x0},//0x2, 0x0},
		// eth2
		{0x00,(0x0c<<3)|0x0, {{0x04, 0xdeb8}, {0x02, 0xdeb8}, {0x03, 0xdeb8}, {0x00, 0x0deb8}}, 0x0, 0x0},//0x1, 0x0},
		// PCI slot
		{0x00,(0x0f<<3)|0x0, {{0x04, 0xdeb8}, {0x03, 0xdeb8}, {0x02, 0xdeb8}, {0x00, 0x0deb8}}, 0x0, 0x0},
	}
};
unsigned long write_pirq_routing_table(unsigned long addr)
{
        return copy_pirq_routing_table(addr, &intel_irq_routing_table);
}
