#ifndef ARCH_PIRQ_ROUTING_H
#define ARCH_PIRQ_ROUTING_H

#define PIRQ_SIGNATURE	(('$' << 0) + ('P' << 8) + ('I' << 16) + ('R' << 24))
#define PIRQ_VERSION 0x0100

extern const struct irq_routing_table intel_irq_routing_table;

#if !defined(DEBUG) && defined(HAVE_PIRQ_TABLE)
void check_pirq_routing_table(void);
#else
#define check_pirq_routing_table() do {} while(0)
#endif

#if defined(HAVE_PIRQ_ROUTING_TABLE)
void copy_pirq_routing_table(void);
#else
#define copy_pirq_routing_table() do {} while(0)
#endif

#endif /* ARCH_PIRQ_ROUTING_H */
