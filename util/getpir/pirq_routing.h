#ifndef ARCH_PIRQ_ROUTING_H
#define ARCH_PIRQ_ROUTING_H

#include <stdint.h>

#define PIRQ_SIGNATURE	(('$' << 0) + ('P' << 8) + ('I' << 16) + ('R' << 24))
#define PIRQ_VERSION 0x0100

struct irq_info {
	uint8_t bus, devfn;			/* Bus, device and function */
	struct {
		uint8_t link;			/* IRQ line ID, chipset dependent, 0=not routed */
		uint16_t bitmap;		/* Available IRQs */
	} __attribute__((packed)) irq[4];
	uint8_t slot;				/* Slot number, 0=onboard */
	uint8_t rfu;
} __attribute__((packed));

#if defined(CONFIG_IRQ_SLOT_COUNT)
#define IRQ_SLOTS_COUNT CONFIG_IRQ_SLOT_COUNT
#elif (__GNUC__ < 3)
#define IRQ_SLOTS_COUNT 1
#else
#define IRQ_SLOTS_COUNT
#endif

struct irq_routing_table {
	uint32_t signature;			/* PIRQ_SIGNATURE should be here */
	uint16_t version;			/* PIRQ_VERSION */
	uint16_t size;				/* Table size in bytes */
	uint8_t  rtr_bus, rtr_devfn;		/* Where the interrupt router lies */
	uint16_t exclusive_irqs;		/* IRQs devoted exclusively to PCI usage */
	uint16_t rtr_vendor, rtr_device;	/* Vendor and device ID of interrupt router */
	uint32_t miniport_data;			/* Miniport data */
	uint8_t  rfu[11];
	uint8_t  checksum;			/* Modulo 256 checksum must give zero */
	struct irq_info slots[IRQ_SLOTS_COUNT];
} __attribute__((packed));

extern const struct irq_routing_table intel_irq_routing_table;

#ifdef GETPIR
#define copy_pirq_routing_table(start) (start)
unsigned long write_pirq_routing_table(unsigned long start);
#else
#if CONFIG_HAVE_PIRQ_TABLE==1
unsigned long copy_pirq_routing_table(unsigned long start);
unsigned long write_pirq_routing_table(unsigned long start);
#else
#define copy_pirq_routing_table(start) (start)
#define write_pirq_routing_table(start) (start)
#endif
#endif

#endif /* ARCH_PIRQ_ROUTING_H */
