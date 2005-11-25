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

const struct irq_routing_table intel_irq_routing_table = {
	PIRQ_SIGNATURE,		/* u32 signature */
	PIRQ_VERSION,           /* u16 version   */
	32+16*IRQ_SLOT_COUNT,	/* there can be total IRQ_SLOT_COUNT table entries */
	IRQ_ROUTER_BUS,		/* Where the interrupt router lies (bus) */
	IRQ_ROUTER_DEVFN,	/* Where the interrupt router lies (dev) */
	0x00,			/* IRQs devoted exclusively to PCI usage */
	IRQ_ROUTER_VENDOR,	/* Vendor */
	IRQ_ROUTER_DEVICE,	/* Device */
	0x00,			/* Crap (miniport) */
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* u8 rfu[11] */
	0x6a,           /*  u8 checksum , mod 256 checksum must give zero */
	{	/* slot(0=onboard), devfn, irqlinks (line id, 0=not routed) */
		IRQ_SLOT(0, 0x01,4,0, 1,2,3,4 ), /* 8111 PCI bridge */
		IRQ_SLOT(0, 0x04,0,0, 4,0,0,0 ), /* 8111 USB */
		IRQ_SLOT(1, 0x06,1,0, 2,3,4,1 ), /* ???? was: bus A*/
		IRQ_SLOT(2, 0x07,1,0, 2,3,4,1 ), /* ???? was: bus 9*/
		IRQ_SLOT(3, 0x0a,1,0, 2,3,4,1 ), /* IBM PCI-X <-> PCI-X */
		IRQ_SLOT(4, 0x08,1,0, 2,3,4,1 ), /* IBM PCI-X <-> PCI-X */
		IRQ_SLOT(0, 0x04,4,0, 1,0,0,0 ), /* ATI Rage */
		IRQ_SLOT(0, 0x06,2,0, 3,4,0,0 ), /* ???? was: bus A */
		IRQ_SLOT(0, 0x07,2,0, 3,4,0,0 ), /* ???? was: bus 9 */
		IRQ_SLOT(0, 0x0a,2,0, 3,4,0,0 ), /* Intel 82546EB GBit */
		IRQ_SLOT(0, 0x08,2,0, 3,4,0,0 ), /* Intel 82546EB GBit */
		IRQ_SLOT(0, 0x0d,1,0, 1,0,0,0 ), /* Marvell MV88SX5080 SATA */
		IRQ_SLOT(0, 0x0d,2,0, 2,0,0,0 ), /* Marvell MV88SX5080 SATA */
		IRQ_SLOT(0, 0x0e,1,0, 1,2,3,4 ), /* Intel Memory Controller 031a */
		IRQ_SLOT(0, 0x0f,1,0, 1,0,0,0 ), /* Marvell MV88SX5080 SATA */
		IRQ_SLOT(0, 0x04,5,0, 2,0,0,0 ), /* Intel 8255 Ethernet */
		IRQ_SLOT(5, 0x10,1,0, 1,2,3,4 ), /* ???? was: bus C */
		IRQ_SLOT(0, 0x12,1,0, 1,0,0,0 ), /* Marvell MV88SX5080 SATA */
		IRQ_SLOT(0, 0x12,2,0, 2,0,0,0 ), /* Marvell MV88SX5080 SATA */
		IRQ_SLOT(0, 0x13,1,0, 1,2,3,4 ), /* Intel Memory Controller 031b */
		IRQ_SLOT(0, 0x14,1,0, 1,0,0,0 ), /* Marvell MV88SX5080 SATA */
		IRQ_SLOT(6, 0x15,1,0, 1,2,3,4 ), /* ???? was: bus 11 */
		/* Let Linux know about bus 1 */
		IRQ_SLOT(0, 1,4,3, 0,0,0,0 ),
	}
};
unsigned long write_pirq_routing_table(unsigned long addr)
{
        return copy_pirq_routing_table(addr);
}
