#include <arch/pirq_routing.h>
#include <device/pci.h>

#define IRQ_ROUTER_BUS		1
#define IRQ_ROUTER_DEVFN	PCI_DEVFN(5,0)
#define IRQ_ROUTER_VENDOR	0x1022
#define IRQ_ROUTER_DEVICE	0x7468
#define IRQS_EXCLUSIVE		0x0c20
#define IRQS_AVAILABLE		0xdeb8


#define IRQ_SLOT(slot, bus, dev, fn, linka, linkb, linkc, linkd) \
	{ bus, (dev<<3)|fn, {{ linka, IRQS_AVAILABLE}, { linkb, IRQS_AVAILABLE}, \
	{linkc, IRQS_AVAILABLE}, {linkd, IRQS_AVAILABLE}}, slot, 0}

/*  Each IRQ_SLOT entry consists of:
 *  bus, devfn, {link, bitmap}, {link, bitmap}, {link, bitmap}, {link, bitmap}, slot, rfu  
 */

const struct irq_routing_table intel_irq_routing_table = {
	PIRQ_SIGNATURE,		/* u32 signature */
	PIRQ_VERSION,		/* u16 version   */
	32 + 16 * IRQ_SLOT_COUNT,	/* there can be total IRQ_SLOT_COUNT 
					 * devices on the bus */
	IRQ_ROUTER_BUS,		/* Where the interrupt router lies (bus) */
	IRQ_ROUTER_DEVFN,	/* Where the interrupt router lies (dev) */
	IRQS_EXCLUSIVE,		/* IRQs devoted exclusively to PCI usage */
	IRQ_ROUTER_VENDOR,	/* Vendor */
	IRQ_ROUTER_DEVICE,	/* Device */
	0x00,			/* Crap (miniport) */
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	/* u8 rfu[11] */
	0x00,			/*  u8 checksum , mod 256 checksum must give
				 *  zero, will be corrected later 
				 */
	{

	 /* slot(0=onboard), devfn, irqlinks (line id, 0=not routed) */

	 /* PCI SLOT 1-4 */
	 IRQ_SLOT(1, 3, 4, 0, 1, 2, 3, 4),
	 IRQ_SLOT(2, 3, 5, 0, 2, 3, 4, 1),
	 IRQ_SLOT(3, 3, 6, 0, 3, 4, 1, 2),
	 IRQ_SLOT(4, 3, 7, 0, 4, 1, 2, 3),

	 /* Builtin Devices */
	 IRQ_SLOT(0, 3, 0, 0, 4, 4, 4, 4),	/* USB */
	 IRQ_SLOT(0, 1, 5, 1, 1, 2, 3, 4),	/* IDE */
	 IRQ_SLOT(0, 1, 2, 0, 1, 2, 3, 4),	/* AGP Bridge */

	 /* Let Linux know about bus 1 */
	 IRQ_SLOT(0, 1, 5, 0, 0, 0, 0, 0),

	 }
};
