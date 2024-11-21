/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __I386_ARCH_IOAPIC_H
#define __I386_ARCH_IOAPIC_H

#define IO_APIC_ADDR	0xfec00000

#ifndef __ACPI__

#include <types.h>

u8 get_ioapic_id(uintptr_t ioapic_base);
u8 get_ioapic_version(uintptr_t ioapic_base);

unsigned int ioapic_get_max_vectors(uintptr_t ioapic_base);
void ioapic_set_max_vectors(uintptr_t ioapic_base, int mre_count);
void ioapic_lock_max_vectors(uintptr_t ioapic_base);

struct device *ioapic_create_dev(struct device *parent,
				 const uintptr_t ioapic_base,
				 const u32 gsi_base);
void setup_ioapic(uintptr_t ioapic_base, u8 ioapic_id);
void register_new_ioapic(uintptr_t ioapic_base);
void register_new_ioapic_gsi0(uintptr_t ioapic_base);

void ioapic_set_boot_config(uintptr_t ioapic_base, bool irq_on_fsb);

void ioapic_get_sci_pin(u8 *gsi, u8 *irq, u8 *flags);
#endif

#endif
