/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef DRIVERS_GENERIC_IOAPIC_CHIP_H
#define DRIVERS_GENERIC_IOAPIC_CHIP_H

typedef struct drivers_generic_ioapic_config {
	u32 version;
	u8 apicid;
	u8 have_isa_interrupts;
	void *base;
} ioapic_config_t;

#endif
