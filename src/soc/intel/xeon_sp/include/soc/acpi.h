/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_ACPI_H_
#define _SOC_ACPI_H_

#include <acpi/acpi.h>

/**
 Select C-state map set in config cstate_states
 **/
enum acpi_cstate_mode {
	CSTATES_ALL = 0,
	CSTATES_C1C6
};

#define MEM_BLK_COUNT      0x140
typedef struct {
	uint8_t   buf[32];
} MEM_BLK;

unsigned long northbridge_write_acpi_tables(const struct device *device,
	unsigned long current, struct acpi_rsdp *rsdp);
void uncore_inject_dsdt(const struct device *device);

#endif /* _SOC_ACPI_H_ */
