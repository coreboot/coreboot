/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef INTEL_COMMON_ACPI_PIRQ_GEN_H
#define INTEL_COMMON_ACPI_PIRQ_GEN_H

#include <assert.h>
#include <device/device.h>

#define MAX_SLOTS	32

enum pci_pin {
	PCI_INT_NONE = 0,
	PCI_INT_A,
	PCI_INT_B,
	PCI_INT_C,
	PCI_INT_D,
	PCI_INT_MAX = PCI_INT_D,
};

enum pirq {
	PIRQ_INVALID,
	PIRQ_A,
	PIRQ_B,
	PIRQ_C,
	PIRQ_D,
	PIRQ_E,
	PIRQ_F,
	PIRQ_G,
	PIRQ_H,
	PIRQ_COUNT = PIRQ_H,
};

static inline size_t pirq_idx(enum pirq pirq)
{
	assert(pirq > PIRQ_INVALID && pirq <= PIRQ_H);
	return (size_t)(pirq - PIRQ_A);
}

/*
 * This struct represents an assignment of slot/pin -> IRQ. Some chipsets may
 * want to provide both PIC-mode and APIC-mode IRQs (e.g. selected using PICM
 * set by the OS), therefore a field for each of a PIRQ for PIC-mode and a
 * GSI for APIC-mode are provided.
 *
 * For APIC mode, only GSIs are supported (`acpi_gsi`).
 *
 * For PIC mode, if the pirq_map_type is PIRQ_GSI, then `pic_pirq` is used as an
 * index into `struct pic_pirq_map.gsi`, or for SOURCE_PATH, `pic_pirq` indexes
 * into `struct pic_pirq_map.source_path` to pick the path to the LNKx device.
 *
 * The reasoning for this structure is related to older vs. newer Intel
 * platforms; older platforms supported routing of PCI IRQs to a PIRQ
 * only. Newer platforms support routing IRQs to either a PIRQ or (for some PCI
 * devices) a non-PIRQ GSI.
 */
struct slot_pin_irq_map {
	unsigned int slot;
	enum pci_pin pin;
	/* PIRQ # for PIC mode */
	unsigned int pic_pirq;
	/* GSI # for APIC mode */
	unsigned int apic_gsi;
};

enum pirq_map_type {
	PIRQ_GSI,
	PIRQ_SOURCE_PATH,
};

/*
 * A PIRQ can be either be statically assigned a GSI or OSPM can use the Methods
 * on the ACPI device (source_path) to assign IRQs at runtime.
 */
struct pic_pirq_map {
	enum pirq_map_type type;
	union {
		unsigned int gsi[PIRQ_COUNT];
		char source_path[PIRQ_COUNT][DEVICE_PATH_MAX];
	};
};

/*
 * Generate an ACPI _PRT table by providing PIRQ and/or GSI information for each
 * slot/pin combination, and optionally providing paths to LNKx devices that can
 * provide IRQs in PIC mode.
 */
void intel_write_pci0_PRT(const struct slot_pin_irq_map *pin_irq_map,
			  unsigned int map_count,
			  const struct pic_pirq_map *pirq_map);

bool is_slot_pin_assigned(const struct slot_pin_irq_map *pin_irq_map,
			  unsigned int map_count, unsigned int slot,
			  enum pci_pin pin);

#endif
