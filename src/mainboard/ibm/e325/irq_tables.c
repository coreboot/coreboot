#include <arch/pirq_routing.h>
#include <device/pci.h>

#define IRQ_ROUTER_BUS		0
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
	0x34,			/*  u8 checksum , mod 256 checksum must give zero */
	{	/* slot(0=onboard), devfn, irqlinks (line id, 0=not routed) */
		/* Northbridge, Node 0 */
		IRQ_SLOT(0x0, 0x00,0x18,0x0, 0,0,0,0),
		/* AMD-8131 PCI-X Bridge */
		IRQ_SLOT(0x0, 0x01,0x01,0x0, 0,0,0,0),
		/* Onboard LSI SCSI Controller */
		IRQ_SLOT(0x0, 0x02,0x02,0x0, 3,0,0,0),
		/* Onboard Broadcom NICs */
		IRQ_SLOT(0x0, 0x02,0x01,0x0, 1,2,0,0),
		/* AMD-8131 PCI-X Bridge */
		IRQ_SLOT(0x0, 0x01,0x02,0x0, 0,0,0,0),
		/* PCI Slot 1-2 */
		IRQ_SLOT(0x1, 0x03,0x03,0x0, 1,2,3,4),
		IRQ_SLOT(0x2, 0x03,0x04,0x0, 2,3,4,1),
		/* AMD-8111 PCI Bridge */
		IRQ_SLOT(0x0, 0x01,0x03,0x0, 0,0,0,0),
		/* USB Controller */
		IRQ_SLOT(0x0, 0x04,0x00,0x0, 0,0,0,4),
		/* ATI Rage XL VGA */
		IRQ_SLOT(0x0, 0x04,0x05,0x0, 1,0,0,0),
		/* AMD-8111 LPC Dridge */
		IRQ_SLOT(0x0, 0x01,0x04,0x0, 0,0,0,0),
		/* Northbridge, Node 1 */
		IRQ_SLOT(0x0, 0x00,0x19,0x0, 0,0,0,0),

	}
};
unsigned long write_pirq_routing_table(unsigned long addr)
{
        return copy_pirq_routing_table(addr, &intel_irq_routing_table);
}
