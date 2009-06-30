#ifndef ARCH_PIRQ_ROUTING_H
#define ARCH_PIRQ_ROUTING_H

#include <types.h>

#define PIRQ_SIGNATURE	(('$' << 0) + ('P' << 8) + ('I' << 16) + ('R' << 24))
#define PIRQ_VERSION 0x0100

struct irq_info {
	u8 bus, devfn;			/* Bus, device and function */
	struct {
		u8 link;		/* IRQ line ID, chipset dependent, 0=not routed */
		u16 bitmap;		/* Available IRQs */
	} __attribute__((packed)) irq[4];
	u8 slot;			/* Slot number, 0=onboard */
	u8 rfu;
} __attribute__((packed));

#if defined(CONFIG_IRQ_SLOT_COUNT)
#define IRQ_SLOTS_COUNT CONFIG_IRQ_SLOT_COUNT
#elif (__GNUC__ < 3)
#define IRQ_SLOTS_COUNT 1
#else
#define IRQ_SLOTS_COUNT
#endif

struct irq_routing_table {
	u32 signature;			/* PIRQ_SIGNATURE should be here */
	u16 version;			/* PIRQ_VERSION */
	u16 size;			/* Table size in bytes */
	u8 rtr_bus, rtr_devfn;		/* Where the interrupt router lies */
	u16 exclusive_irqs;		/* IRQs devoted exclusively to PCI usage */
	u16 rtr_vendor, rtr_device;	/* Vendor and device ID of interrupt router */
	u32 miniport_data;		/* Crap */
	u8 rfu[11];
	u8 checksum;			/* Modulo 256 checksum must give zero */
	struct irq_info slots[IRQ_SLOTS_COUNT];
} __attribute__((packed));

extern const struct irq_routing_table intel_irq_routing_table;

#if defined(CONFIG_DEBUG) && defined(CONFIG_HAVE_PIRQ_TABLE)
void check_pirq_routing_table(void);
#else
#define check_pirq_routing_table() do {} while(0)
#endif

#if defined(CONFIG_HAVE_PIRQ_TABLE)
unsigned long copy_pirq_routing_table(unsigned long start);
#else
#define copy_pirq_routing_table(start) (start)
#endif

#endif /* ARCH_PIRQ_ROUTING_H */
