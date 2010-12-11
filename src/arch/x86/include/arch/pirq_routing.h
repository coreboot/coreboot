#ifndef ARCH_PIRQ_ROUTING_H
#define ARCH_PIRQ_ROUTING_H

#if CONFIG_GENERATE_PIRQ_TABLE==1
#include <stdint.h>

#define PIRQ_SIGNATURE	(('$' << 0) + ('P' << 8) + ('I' << 16) + ('R' << 24))
#define PIRQ_VERSION 0x0100

struct irq_info {
	u8 bus, devfn;	    /* Bus, device and function */
	struct {
		u8 link;    /* IRQ line ID, chipset dependent, 0=not routed */
		u16 bitmap; /* Available IRQs */
	} __attribute__((packed)) irq[4];
	u8 slot;	    /* Slot number, 0=onboard */
	u8 rfu;
} __attribute__((packed));

#ifndef CONFIG_IRQ_SLOT_COUNT
#warning "IRQ_SLOT_COUNT is not defined in Kconfig. PIRQ won't work correctly."
#endif

struct irq_routing_table {
	u32 signature;		/* PIRQ_SIGNATURE should be here */
	u16 version;		/* PIRQ_VERSION */
	u16 size;		/* Table size in bytes */
	u8  rtr_bus, rtr_devfn;	/* Where the interrupt router lies */
	u16 exclusive_irqs;	/* IRQs devoted exclusively to PCI usage */
	u16 rtr_vendor, rtr_device;/* Vendor/device ID of interrupt router */
	u32 miniport_data;
	u8  rfu[11];
	u8  checksum;		/* Modulo 256 checksum must give zero */
	struct irq_info slots[CONFIG_IRQ_SLOT_COUNT];
} __attribute__((packed));

extern const struct irq_routing_table intel_irq_routing_table;

unsigned long copy_pirq_routing_table(unsigned long start);
unsigned long write_pirq_routing_table(unsigned long start);

#if CONFIG_PIRQ_ROUTE==1
void pirq_routing_irqs(unsigned long start);
void pirq_assign_irqs(const unsigned char pIntAtoD[4]);
#else
#define pirq_routing_irqs(start) {}
#endif

#else
#define copy_pirq_routing_table(start) (start)
#define write_pirq_routing_table(start) (start)
#endif

#endif /* ARCH_PIRQ_ROUTING_H */
