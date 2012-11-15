#include <arch/pirq_routing.h>
#include <device/pci.h>

#define IRQ_ROUTER_BUS		1
#define IRQ_ROUTER_DEVFN	PCI_DEVFN(4,3)
#define IRQ_ROUTER_VENDOR	0x1022
#define IRQ_ROUTER_DEVICE	0x746b

#define AVAILABLE_IRQS 0xdef8
#define IRQ_SLOT(slot, bus, dev, fn, linka, linkb, linkc, linkd) \
	{ bus, (dev<<3)|fn, {{ linka, AVAILABLE_IRQS}, { linkb, AVAILABLE_IRQS}, \
	{linkc, AVAILABLE_IRQS}, {linkd, AVAILABLE_IRQS}}, slot, 0}

/*  Each IRQ_SLOT entry consists of:
 *  bus, devfn, {link, bitmap}, {link, bitmap}, {link, bitmap}, {link, bitmap}, slot, rfu
 */

static const struct irq_routing_table intel_irq_routing_table = {
	PIRQ_SIGNATURE,		/* u32 signature */
	PIRQ_VERSION,           /* u16 version   */
	32+16*CONFIG_IRQ_SLOT_COUNT,	/* there can be total CONFIG_IRQ_SLOT_COUNT table entries */
	IRQ_ROUTER_BUS,		/* Where the interrupt router lies (bus) */
	IRQ_ROUTER_DEVFN,	/* Where the interrupt router lies (dev) */
	0x00,			/* IRQs devoted exclusively to PCI usage */
	IRQ_ROUTER_VENDOR,	/* Vendor */
	IRQ_ROUTER_DEVICE,	/* Device */
	0x00,			/* Miniport data */
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* u8 rfu[11] */
	0xb0,           /*  u8 checksum , mod 256 checksum must give zero */
	{	/* slot(0=onboard), devfn, irqlinks (line id, 0=not routed) */
		/* PCI Slot 1-6 */
		IRQ_SLOT(1, 3,1,0, 2,3,4,1 ),
		IRQ_SLOT(2, 3,2,0, 3,4,1,2 ),
		IRQ_SLOT(3, 2,1,0, 2,3,4,1 ),
		IRQ_SLOT(4, 2,2,0, 3,4,1,2 ),
		IRQ_SLOT(5, 4,5,0, 2,3,4,1 ),
		IRQ_SLOT(6, 4,4,0, 1,2,3,4 ),
		/* Onboard NICs */
		IRQ_SLOT(0, 2,3,0, 4,0,0,0 ),
		IRQ_SLOT(0, 2,4,0, 4,0,0,0 ),
		/* Let Linux know about bus 1 */
		IRQ_SLOT(0, 1,4,3, 0,0,0,0 ),
	}
};
unsigned long write_pirq_routing_table(unsigned long addr)
{
        return copy_pirq_routing_table(addr, &intel_irq_routing_table);
}
