/*
   Contains the IRQ Routing Table 

   Documentation at : http://www.microsoft.com/hwdev/busbios/PCIIRQ.HTM

   Edit the irqs and mask for the stpc.
*
*/

#include <arch/pirq_routing.h>
#include <pci.h>

//
// the values are assigned on bootup
// in mainboard.c
//
#define MASK 0xffff
#define INTA 0
#define INTB 0
#define INTC 0
#define INTD 0

const struct irq_routing_table intel_irq_routing_table = {
	PIRQ_SIGNATURE, /* u32 signature */
	PIRQ_VERSION,   /* u16 version   */
	32+16*3,        /* there can be total 3 devices on the bus */
	0,           /* Where the interrupt router lies (bus) */
	PCI_DEVFN(11,0),           /* Where the interrupt router lies (dev) */
	MASK,         /* IRQs devoted exclusively to PCI usage */
	0x0,         /* Vendor */
	0x104a,         /* Vendor */
	0x020a,         /* Device */
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* u8 rfu[11] */
	0x00,   /*  u8 checksum , this has to be set to value that would give 0 after the sum of all bytes for this structure (including checksum) */
	{
		{0,PCI_DEVFN(0x1d,0), {{INTC, MASK}, {INTD, MASK}, {INTA, MASK}, {INTB, MASK}}, 0x1, 0},
		{0,PCI_DEVFN(0x1e,0), {{INTB, MASK}, {INTC, MASK}, {INTD, MASK}, {INTA, MASK}}, 0x2, 0},
		{0,PCI_DEVFN(0x1f,0), {{INTA, MASK}, {INTB, MASK}, {INTC, MASK}, {INTD, MASK}}, 0x3, 0},
	}
};
