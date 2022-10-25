/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_PCI_UTIL_H
#define AMD_BLOCK_PCI_UTIL_H

#include <types.h>
#include <soc/amd_pci_int_defs.h>
#include <device/device.h>

/* FCH index/data registers */
#define PCI_INTR_INDEX		0xc00
#define PCI_INTR_DATA		0xc01

#define FCH_IRQ_ROUTING_ENTRIES	0x80

struct fch_irq_routing {
	uint8_t intr_index;
	uint8_t pic_irq_num;
	uint8_t apic_irq_num;
};

struct pirq_struct {
	u8 devfn;
	u8 PIN[4];	/* PINA/B/C/D are index 0/1/2/3 */
};

struct irq_idx_name {
	uint8_t index;
	const char *const name;
};

extern const struct pirq_struct *pirq_data_ptr;
extern u32 pirq_data_size;
extern const u8 *intr_data_ptr;
extern const u8 *picr_data_ptr;

u8 read_pci_int_idx(u8 index, int mode);
void write_pci_int_idx(u8 index, int mode, u8 data);
void write_pci_cfg_irqs(void);
void write_pci_int_table(void);
const struct irq_idx_name *sb_get_apic_reg_association(size_t *size);

enum pci_routing_swizzle {
	PCI_SWIZZLE_ABCD = 0,
	PCI_SWIZZLE_BCDA,
	PCI_SWIZZLE_CDAB,
	PCI_SWIZZLE_DABC,
};

/**
 * Each PCI bridge has its INTx lines routed to one of the GNB IO-APIC PCI
 * groups. Each group has 4 interrupts. The INTx lines can be swizzled before
 * being routed to the IO-APIC. If the IO-APIC redirection entry is masked, the
 * interrupt is reduced modulo 8 onto INT[A-H] and forwarded to the FCH IO-APIC.
 **/
struct pci_routing_info {
	uint8_t devfn;
	uint8_t group;
	uint8_t swizzle;
	uint8_t irq;
} __packed;

void populate_pirq_data(void);

/* Implemented by the SoC */
const struct pci_routing_info *get_pci_routing_table(size_t *entries);

const struct pci_routing_info *get_pci_routing_info(unsigned int devfn);

unsigned int pci_calculate_irq(const struct pci_routing_info *routing_info, unsigned int pin);

void acpigen_write_pci_GNB_PRT(const struct device *dev);
void acpigen_write_pci_FCH_PRT(const struct device *dev);

#endif /* AMD_BLOCK_PCI_UTIL_H */
