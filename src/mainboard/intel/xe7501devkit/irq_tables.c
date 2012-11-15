/* Run checkpir to verify any changes to this table...
   Documentation at : http://www.microsoft.com/whdc/archive/pciirq.mspx
*/

#include <arch/pirq_routing.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include "bus.h"

#define UNUSED_INTERRUPT {0, 0}
#define PIRQ_A 0x60
#define PIRQ_B 0x61
#define PIRQ_C 0x62
#define PIRQ_D 0x63
#define PIRQ_E 0x68
#define PIRQ_F 0x69
#define PIRQ_G 0x6A
#define PIRQ_H 0x6B

static const struct irq_routing_table intel_irq_routing_table = {
	PIRQ_SIGNATURE,
	PIRQ_VERSION,
	32 + 16 * CONFIG_IRQ_SLOT_COUNT,		// Size of this struct in bytes
	0,			 							// PCI bus number on which the interrupt router resides
	PCI_DEVFN(31, 0),   					// PCI device/function number of the interrupt router
	0,		 								// PCI-exclusive IRQ bitmap
	PCI_VENDOR_ID_INTEL,					// Vendor ID of compatible PCI interrupt router
	PCI_DEVICE_ID_INTEL_82801CA_LPC,		// Device ID of compatible PCI interrupt router
	0,		 								// Additional miniport information
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 	// Reserved, must be zero
	0xB1,      								// Checksum of the entire structure (causes 8-bit sum == 0)
	{
		// NOTE: For 82801, a nonzero link value is a pointer to a PIRQ[n]_ROUT register in PCI configuration space
		//		 This was determined from linux-2.6.11/arch/x86/pci/irq.c
		// bitmap of 0xdcf8 == routable to IRQ3-IRQ7, IRQ10-IRQ12, or IRQ14-IRQ15
		// ICH-3 doesn't allow SERIRQ or PCI message to generate IRQ0, IRQ2, IRQ8, or IRQ13
		// Not sure why IRQ9 isn't routable (inherited from Tyan S2735)

		//											INTA#			   INTB#		     INTC#		      INTD#
	//  bus,				device #  		   {link  , bitmap}, {link  , bitmap}, {link  , bitmap}, {link  , bitmap},  slot, rfu

		{PCI_BUS_CHIPSET,	PCI_DEVFN(31, 0), {{PIRQ_C, 0xdcf8}, {PIRQ_B, 0xdcf8}, UNUSED_INTERRUPT, UNUSED_INTERRUPT},   0, 0},	// IDE / SMBus
		{PCI_BUS_CHIPSET,	PCI_DEVFN(29, 0), {{PIRQ_A, 0xdcf8}, {PIRQ_D, 0xdcf8}, {PIRQ_C, 0xdcf8}, UNUSED_INTERRUPT},   0, 0},	// USB 1.1

		// P64H2#2 Bus A
		{PCI_BUS_P64H2_2_A, PCI_DEVFN(1, 0),  {{PIRQ_B, 0xdcf8}, {PIRQ_B, 0xdcf8}, UNUSED_INTERRUPT, UNUSED_INTERRUPT},   0, 0},	// SCSI
			// NOTE: Hotplug disabled on this bus

		// P64H2#2 Bus B
		{PCI_BUS_P64H2_2_B, PCI_DEVFN(1, 0),  {{PIRQ_B, 0xdcf8}, {PIRQ_B, 0xdcf8}, {PIRQ_B, 0xdcf8}, {PIRQ_B, 0xdcf8}},  23, 0},	// Slot 2A (J23)
		{PCI_BUS_P64H2_2_B, PCI_DEVFN(2, 0),  {{PIRQ_B, 0xdcf8}, {PIRQ_B, 0xdcf8}, {PIRQ_B, 0xdcf8}, {PIRQ_B, 0xdcf8}},  24, 0},	// Slot 2B (J24)
		{PCI_BUS_P64H2_2_B, PCI_DEVFN(3, 0),  {{PIRQ_B, 0xdcf8}, {PIRQ_B, 0xdcf8}, {PIRQ_B, 0xdcf8}, {PIRQ_B, 0xdcf8}},  25, 0},	// Slot 2C (J25)
		{PCI_BUS_P64H2_2_B, PCI_DEVFN(4, 0),  {{PIRQ_B, 0xdcf8}, {PIRQ_B, 0xdcf8}, {PIRQ_B, 0xdcf8}, {PIRQ_B, 0xdcf8}},  12, 0},	// Slot 2D (J12)
			// NOTE: Hotplug disabled on this bus

		// P64H2#1 Bus A
		{PCI_BUS_P64H2_1_A, PCI_DEVFN(1, 0),  {{PIRQ_A, 0xdcf8}, {PIRQ_A, 0xdcf8}, {PIRQ_A, 0xdcf8}, {PIRQ_A, 0xdcf8}},  20, 0},	// Slot 1A (J20)
			// NOTE: Hotplug disabled on this bus

		// P64H2#1 Bus B
		{PCI_BUS_P64H2_1_B, PCI_DEVFN(1, 0),  {{PIRQ_A, 0xdcf8}, UNUSED_INTERRUPT, UNUSED_INTERRUPT, UNUSED_INTERRUPT},   0, 0},	// GB Ethernet
		{PCI_BUS_P64H2_1_B, PCI_DEVFN(2, 0),  {{PIRQ_A, 0xdcf8}, {PIRQ_A, 0xdcf8}, {PIRQ_A, 0xdcf8}, {PIRQ_A, 0xdcf8}},  21, 0},	// Slot 1B (J21)
			// NOTE: Hotplug disabled on this bus

		// ICH-3 PCI bus
		{PCI_BUS_ICH3,		PCI_DEVFN(0, 0),  {{PIRQ_A, 0xdcf8}, UNUSED_INTERRUPT, UNUSED_INTERRUPT, UNUSED_INTERRUPT},   0, 0},	// Video
		{PCI_BUS_ICH3,		PCI_DEVFN(2, 0),  {{PIRQ_C, 0xdcf8}, {PIRQ_D, 0xdcf8}, {PIRQ_A, 0xdcf8}, {PIRQ_B, 0xdcf8}},  11, 0},	// Debug slot (J11)
	}
};

unsigned long write_pirq_routing_table(unsigned long addr)
{
	return copy_pirq_routing_table(addr, &intel_irq_routing_table);
}
