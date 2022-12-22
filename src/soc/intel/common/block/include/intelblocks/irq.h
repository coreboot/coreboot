/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SOC_INTEL_COMMON_IRQ_H
#define SOC_INTEL_COMMON_IRQ_H

#include <southbridge/intel/common/acpi_pirq_gen.h>
#include <types.h>

#define MAX_FNS		8
#define INVALID_IRQ	-1

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

/*
 * This functions applies rules from FSP, BWG and SoC to come up with a set of
 * PCI slot/function --> IRQ pin/IRQ number entries.
 *
 * The results of this calculation are cached within this module for usage
 * by the other API functions.
 */
bool assign_pci_irqs(const struct slot_irq_constraints *constraints, size_t num_slots);

/* Generate an ACPI PCI IRQ routing table (_PRT) in the \_SB.PCI0 scope, using
   the cached results. */
bool generate_pin_irq_map(void);

/* Typically the FSP can accept a list of the mappings provided above, and
   program them, but for PCH devices only. This function provides the same
   function for non-PCH devices. */
bool irq_program_non_pch(void);

const struct pci_irq_entry *get_cached_pci_irqs(void);

/* Search the cached PCI IRQ assignment results for the matching devfn and
   return the corresponding IRQ, or INVALID_IRQ if not found. */
int get_pci_devfn_irq(unsigned int devfn);

/* Check if a given slot is a PCH slot.
 * Note: For PCH less SoC platforms, like MeteorLake and onwards, this function
 * can be utilized to check if a slot belongs to the SoC or IOE die.
 */
bool is_pch_slot(unsigned int devfn);

#endif /* SOC_INTEL_COMMON_IRQ_H */
