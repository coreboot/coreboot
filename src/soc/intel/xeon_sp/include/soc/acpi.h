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
unsigned long xeonsp_acpi_create_madt_lapics(unsigned long current);
unsigned long acpi_fill_cedt(unsigned long current);
unsigned long acpi_fill_hmat(unsigned long current);
unsigned long cxl_fill_srat(unsigned long current);

#endif /* _SOC_ACPI_H_ */
