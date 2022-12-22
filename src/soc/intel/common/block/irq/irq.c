/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <intelblocks/gpio.h>
#include <intelblocks/irq.h>
#include <intelblocks/lpc_lib.h>
#include <soc/pci_devs.h>
#include <southbridge/intel/common/acpi_pirq_gen.h>
#include <stdlib.h>
#include <types.h>

#define MIN_SHARED_IRQ		16
#define MAX_SHARED_IRQ		23
#define TOTAL_SHARED_IRQ	(MAX_SHARED_IRQ - MIN_SHARED_IRQ + 1)
#define MAX_IRQS		120

#define IDX2PIN(i)		(enum pci_pin)((i) + PCI_INT_A)
#define PIN2IDX(p)		(size_t)((p) - PCI_INT_A)

struct pin_info {
	enum pin_state {
		FREE_PIN,
		SHARED_IRQ_PIN,
		UNIQUE_IRQ_PIN,
	} pin_state;
	unsigned int usage_count;
	unsigned int irq;
};

static unsigned int irq_share_count[TOTAL_SHARED_IRQ];

/*
 * Assign PCI IRQs & pins according to controller rules.
 *
 * This information is provided to the FSP in order for it to do the
 * programming; this is required because the FSP is also responsible for
 * enabling some PCI devices so they will show up on their respective PCI
 * buses. The FSP & PCH BIOS Specification contain rules for how certain IPs
 * require their interrupt pin and interrupt line to be programmed.
 *
 * IOAPIC IRQs are used for PCI devices & GPIOs. The GPIO IRQs are fixed in
 * hardware (the IRQ field is RO), and often start at 24, which means
 * conflicts with PCI devices (if using the default FSP configuration) are very
 * possible.
 *
 * These are the rules:
 * 1) One entry per slot/function
 * 2) Functions using PIRQs must use IOxAPIC IRQs 16-23
 * 3) Single-function devices must use INTA
 * 4) Each slot must have consistent INTx<->PIRQy mappings
 * 5) Some functions have special interrupt pin requirements (FIXED_INT_ANY_PIRQ)
 * 6) PCI Express RPs must be assigned in a special way (FIXED_INT_PIRQ)
 * 7) Some functions require a unique IRQ number (mostly LPSS devices, DIRECT_IRQ)
 * 8) PCI functions must avoid sharing an IRQ with a GPIO pad which routes its
 *    IRQ through IO-APIC.
 */

static int find_free_unique_irq(void)
{
	static unsigned int next_irq = MAX_SHARED_IRQ + 1;

	while (next_irq < MAX_IRQS && gpio_routes_ioapic_irq(next_irq))
		++next_irq;

	if (next_irq == MAX_IRQS)
		return INVALID_IRQ;

	return next_irq++;
}

static enum pci_pin find_free_pin(const struct pin_info pin_info[PCI_INT_MAX])
{
	for (size_t pin_idx = 0; pin_idx < PCI_INT_MAX; pin_idx++) {
		if (pin_info[pin_idx].pin_state == FREE_PIN)
			return IDX2PIN(pin_idx);
	}

	return PCI_INT_NONE;
}

static enum pci_pin find_shareable_pin(const struct pin_info pin_info[PCI_INT_MAX])
{
	unsigned int least_shared = 255;
	int least_index = -1;

	for (size_t pin_idx = 0; pin_idx < PCI_INT_MAX; pin_idx++) {
		if (pin_info[pin_idx].pin_state == SHARED_IRQ_PIN &&
		    pin_info[pin_idx].usage_count < least_shared) {
			least_shared = pin_info[pin_idx].usage_count;
			least_index = pin_idx;
		}
	}

	if (least_index < 0)
		return PCI_INT_NONE;

	return IDX2PIN(least_index);
}

static enum pirq find_global_least_used_pirq(void)
{
	unsigned int least_shared = 255;
	int least_index = -1;

	for (size_t i = 0; i < TOTAL_SHARED_IRQ; i++) {
		if (irq_share_count[i] < least_shared) {
			least_shared = irq_share_count[i];
			least_index = i;
		}
	}

	if (least_index >= 0)
		return (enum pirq)least_index + PIRQ_A;

	return PIRQ_INVALID;
}


static int pirq_to_irq(enum pirq pirq)
{
	return pirq_idx(pirq) + MIN_SHARED_IRQ;
}

static bool assign_pirq(struct pin_info pin_info[PCI_INT_MAX], enum pci_pin pin, enum pirq pirq)
{
	if (pirq < PIRQ_A || pirq > PIRQ_H) {
		printk(BIOS_ERR, "Invalid pirq constraint %u\n", pirq);
		return false;
	}

	const int irq = pirq_to_irq(pirq);
	pin_info[PIN2IDX(pin)].irq = irq;
	irq_share_count[pirq_idx(pirq)]++;
	return true;
}

static bool assign_pin(enum pci_pin pin, unsigned int fn, enum pin_state state,
		       struct pin_info *pin_info,
		       enum pci_pin fn_pin_map[MAX_FNS])
{
	if (pin < PCI_INT_A || pin > PCI_INT_D) {
		printk(BIOS_ERR, "Invalid pin constraint %u\n", pin);
		return false;
	}

	const size_t pin_idx = PIN2IDX(pin);
	pin_info[pin_idx].pin_state = state;
	pin_info[pin_idx].usage_count++;
	fn_pin_map[fn] = pin;

	return true;
}

static bool assign_fixed_pins(const struct slot_irq_constraints *constraints,
			      struct pin_info *pin_info, enum pci_pin fn_pin_map[MAX_FNS])
{
	for (size_t i = 0; i < MAX_FNS; i++) {
		const enum pci_pin fixed_int_pin = constraints->fns[i].fixed_int_pin;
		if (fixed_int_pin == PCI_INT_NONE)
			continue;

		if (!assign_pin(fixed_int_pin, i, SHARED_IRQ_PIN, pin_info, fn_pin_map))
			return false;
	}

	return true;
}

static bool assign_fixed_pirqs(const struct slot_irq_constraints *constraints,
			      struct pin_info *pin_info, enum pci_pin fn_pin_map[MAX_FNS])
{
	for (size_t i = 0; i < MAX_FNS; i++) {
		const enum pirq fixed_pirq = constraints->fns[i].fixed_pirq;
		if (fixed_pirq == PIRQ_INVALID)
			continue;

		/* A constraint with a fixed pirq is assumed to also have a
		   fixed pin */
		const enum pci_pin pin = fn_pin_map[i];
		if (pin == PCI_INT_NONE) {
			printk(BIOS_ERR, "Slot %u, pirq %u, no pin for function %zu\n",
			       constraints->slot, fixed_pirq, i);
			return false;
		}

		if (!assign_pirq(pin_info, pin, fixed_pirq))
			return false;
	}

	return true;
}

static bool assign_direct_irqs(const struct slot_irq_constraints *constraints,
			       struct pin_info *pin_info, enum pci_pin fn_pin_map[MAX_FNS])
{
	for (size_t i = 0; i < MAX_FNS; i++) {
		if (constraints->fns[i].irq_route != IRQ_DIRECT)
			continue;

		enum pci_pin pin = find_free_pin(pin_info);
		if (pin == PCI_INT_NONE)
			return false;

		if (!assign_pin(pin, i, UNIQUE_IRQ_PIN, pin_info, fn_pin_map))
			return false;

		const int irq = find_free_unique_irq();
		if (irq == INVALID_IRQ) {
			printk(BIOS_ERR, "No free unique IRQs found\n");
			return false;
		}

		const size_t pin_idx = PIN2IDX(pin);
		pin_info[pin_idx].irq = irq;
	}

	return true;
}

static bool assign_shareable_pins(const struct slot_irq_constraints *constraints,
				  struct pin_info *pin_info, enum pci_pin fn_pin_map[MAX_FNS])
{
	for (size_t i = 0; i < MAX_FNS; i++) {
		if (constraints->fns[i].irq_route != IRQ_PIRQ)
			continue;

		if (fn_pin_map[i] == PCI_INT_NONE) {
			enum pci_pin pin = find_free_pin(pin_info);
			if (pin == PCI_INT_NONE) {
				pin = find_shareable_pin(pin_info);

				if (pin == PCI_INT_NONE) {
					printk(BIOS_ERR, "No shareable pins found\n");
					return false;
				}
			}

			if (!assign_pin(pin, i, SHARED_IRQ_PIN, pin_info, fn_pin_map))
				return false;
		}
	}

	return true;
}

static bool assign_pirqs(struct pin_info pin_info[PCI_INT_MAX])
{
	for (size_t pin_idx = 0; pin_idx < PCI_INT_MAX; pin_idx++) {
		if (pin_info[pin_idx].pin_state != SHARED_IRQ_PIN || pin_info[pin_idx].irq != 0)
			continue;

		enum pirq pirq = find_global_least_used_pirq();
		if (pirq == PIRQ_INVALID)
			return false;

		if (!assign_pirq(pin_info, IDX2PIN(pin_idx), pirq))
			return false;
	}

	return true;
}

static void add_entry(struct pci_irq_entry **head, pci_devfn_t devfn, enum pci_pin pin,
		      unsigned int irq)
{
	struct pci_irq_entry *entry = malloc(sizeof(*entry));
	struct pci_irq_entry **tmp = head;

	entry->devfn = devfn;
	entry->pin = pin;
	entry->irq = irq;
	entry->next = NULL;

	while (*tmp)
		tmp = &(*tmp)->next;

	*tmp = entry;
}

static void add_slot_entries(struct pci_irq_entry **head, unsigned int slot,
			     struct pin_info pin_info[PCI_INT_MAX],
			     const enum pci_pin fn_pin_map[MAX_FNS])
{
	for (size_t fn = 0; fn < MAX_FNS; fn++) {
		if (fn_pin_map[fn] == PCI_INT_NONE)
			continue;

		const size_t pin_idx = PIN2IDX(fn_pin_map[fn]);
		add_entry(head, PCI_DEVFN(slot, fn), fn_pin_map[fn], pin_info[pin_idx].irq);
	}
}

static bool assign_slot(struct pci_irq_entry **head,
			const struct slot_irq_constraints *constraints)
{
	struct pin_info pin_info[PCI_INT_MAX] = {0};
	enum pci_pin fn_pin_map[MAX_FNS] = {0};

	/* The order in which pins are assigned is important in that strict constraints must
	 * be resolved first. This means fixed_int_pin -> fixed_pirq -> direct route ->
	 * shared pins -> shared pirqs
	 */
	if (!assign_fixed_pins(constraints, pin_info, fn_pin_map))
		return false;

	if (!assign_fixed_pirqs(constraints, pin_info, fn_pin_map))
		return false;

	if (!assign_direct_irqs(constraints, pin_info, fn_pin_map))
		return false;

	if (!assign_shareable_pins(constraints, pin_info, fn_pin_map))
		return false;

	if (!assign_pirqs(pin_info))
		return false;

	add_slot_entries(head, constraints->slot, pin_info, fn_pin_map);
	return true;
}

static struct pci_irq_entry *cached_entries;

bool assign_pci_irqs(const struct slot_irq_constraints *constraints, size_t num_slots)
{
	for (size_t i = 0; i < num_slots; i++) {
		if (!assign_slot(&cached_entries, &constraints[i]))
			return false;
	}

	const struct pci_irq_entry *entry = cached_entries;
	while (entry) {
		printk(BIOS_INFO, "PCI %2X.%X, %s, using IRQ #%d\n",
		       PCI_SLOT(entry->devfn), PCI_FUNC(entry->devfn),
		       pin_to_str(entry->pin), entry->irq);

		entry = entry->next;
	}

	return true;
}

const struct pci_irq_entry *get_cached_pci_irqs(void)
{
	return cached_entries;
}

static enum pirq irq_to_pirq(unsigned int irq)
{
	if (irq >= MIN_SHARED_IRQ && irq <= MAX_SHARED_IRQ)
		return (enum pirq)(irq - MIN_SHARED_IRQ + PIRQ_A);
	else
		/*
		 * Unknown if devices that require unique IRQs will
		 * even work in legacy PIC mode, given they cannot map
		 * to a PIRQ, therefore skip adding an entry.
		 */
		return PIRQ_INVALID;
}

bool generate_pin_irq_map(void)
{
	struct slot_pin_irq_map *pin_irq_map;
	const uint8_t *legacy_pirq_routing;
	struct pic_pirq_map pirq_map = {0};
	size_t map_count = 0;
	size_t pirq_routes;
	size_t i;

	if (!cached_entries)
		return false;

	pin_irq_map = calloc(MAX_SLOTS, sizeof(struct slot_pin_irq_map) * PCI_INT_MAX);

	pirq_map.type = PIRQ_GSI;
	legacy_pirq_routing = lpc_get_pic_pirq_routing(&pirq_routes);
	for (i = 0; i < PIRQ_COUNT && i < pirq_routes; i++)
		pirq_map.gsi[i] = legacy_pirq_routing[i];

	const struct pci_irq_entry *entry = cached_entries;
	while (entry) {
		const unsigned int slot = PCI_SLOT(entry->devfn);

		if (is_slot_pin_assigned(pin_irq_map, map_count, slot, entry->pin)) {
			entry = entry->next;
			continue;
		}

		pin_irq_map[map_count].slot = slot;
		pin_irq_map[map_count].pin = entry->pin;
		pin_irq_map[map_count].apic_gsi = entry->irq;
		pin_irq_map[map_count].pic_pirq = irq_to_pirq(entry->irq);
		map_count++;
		entry = entry->next;
	}

	intel_write_pci0_PRT(pin_irq_map, map_count, &pirq_map);
	free(pin_irq_map);

	return true;
}

bool __weak is_pch_slot(unsigned int devfn)
{
	if (PCI_SLOT(devfn) >= MIN_PCH_SLOT)
		return true;
	return false;
}

bool irq_program_non_pch(void)
{
	const struct pci_irq_entry *entry = cached_entries;

	if (!entry)
		return false;

	while (entry) {
		if (is_pch_slot(entry->devfn)) {
			entry = entry->next;
			continue;
		}

		if (entry->irq) {
			pci_devfn_t dev = PCI_DEV(0, PCI_SLOT(entry->devfn),
						  PCI_FUNC(entry->devfn));
			pci_s_write_config8(dev, PCI_INTERRUPT_LINE, entry->irq);
			pci_s_write_config8(dev, PCI_INTERRUPT_PIN, (uint8_t)entry->pin);
		}

		entry = entry->next;
	}

	return true;
}

int get_pci_devfn_irq(unsigned int devfn)
{
	const struct pci_irq_entry *entry = cached_entries;

	while (entry) {
		if (entry->devfn == devfn)
			return entry->irq;

		entry = entry->next;
	}

	return INVALID_IRQ;
}
