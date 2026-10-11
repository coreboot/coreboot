/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ACPI_PCCT_H__
#define __ACPI_PCCT_H__

#include <acpi/acpi.h>

/* PCCT (Platform Communication Channel Table) */
struct acpi_pcct {
	acpi_header_t header;
	u32 flags;
	u64 reserved;
} __packed;

#define PCCT_FLAG_PLATFORM_IRQ	(1 << 0)

/* HW-Reduced Communications Subspace Structure (type 2) */
#define PCCT_TYPE_HW_REDUCED_2	2

struct acpi_pcct_hw_reduced_2 {
	u8 type;
	u8 length;
	u32 platform_interrupt;
	u8 platform_interrupt_flags;
	u8 reserved;
	u64 base_address;
	u64 length_of_range;
	acpi_addr_t doorbell_register;
	u64 doorbell_preserve;
	u64 doorbell_write;
	u32 nominal_latency;
	u32 max_periodic_access_rate;
	u16 min_request_turnaround_time;
	acpi_addr_t platform_ack_register;
	u64 platform_ack_preserve;
	u64 platform_ack_write;
} __packed;
_Static_assert(sizeof(struct acpi_pcct_hw_reduced_2) == 90, "Wrong PCCT subspace size");

/*
 * Fill the PCCT flags and the subspaces after the header and return the address
 * after the last subspace, or 0 to signal that no PCCT should be emitted.
 */
uintptr_t acpi_soc_fill_pcct(struct acpi_pcct *pcct, uintptr_t current);

#endif /* __ACPI_PCCT_H__ */
