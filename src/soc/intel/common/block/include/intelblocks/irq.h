/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SOC_INTEL_COMMON_IRQ_H
#define SOC_INTEL_COMMON_IRQ_H

#include <southbridge/intel/common/acpi_pirq_gen.h>
#include <types.h>

#define MAX_FNS		8

#define ANY_PIRQ(x)			[PCI_FUNC(x)] = { .fixed_int_pin = PCI_INT_NONE,\
							  .fixed_pirq = PIRQ_INVALID,	\
							  .irq_route = IRQ_PIRQ, }
#define DIRECT_IRQ(x)			[PCI_FUNC(x)] = { .fixed_int_pin = PCI_INT_NONE,\
							  .fixed_pirq = PIRQ_INVALID,	\
							  .irq_route = IRQ_DIRECT,}
#define FIXED_INT_ANY_PIRQ(x, pin)	[PCI_FUNC(x)] = { .fixed_int_pin = pin,		\
							  .fixed_pirq = PIRQ_INVALID,	\
							  .irq_route = IRQ_PIRQ,}
#define FIXED_INT_PIRQ(x, pin, pirq)	[PCI_FUNC(x)] = { .fixed_int_pin = pin,		\
							  .fixed_pirq = pirq,		\
							  .irq_route = IRQ_PIRQ,}

struct slot_irq_constraints {
	unsigned int slot;
	struct {
		enum pci_pin fixed_int_pin;
		enum pirq fixed_pirq;
		enum {
			IRQ_NONE = 0,	/* Empty function */
			IRQ_PIRQ = 1,	/* PIRQ routing, i.e. IRQs 16 - 23 */
			IRQ_DIRECT = 2,	/* No PIRQ routing, i.e., IRQs > 23 */
		} irq_route;
	} fns[MAX_FNS];
};

struct pci_irq_entry {
	unsigned int devfn;
	enum pci_pin pin;
	unsigned int irq;
	struct pci_irq_entry *next;
};

const struct pci_irq_entry *assign_pci_irqs(const struct slot_irq_constraints *constraints,
					    size_t num_slots);

void generate_pin_irq_map(const struct pci_irq_entry *entries);

void irq_program_non_pch(const struct pci_irq_entry *entries);

#endif /* SOC_INTEL_COMMON_IRQ_H */
