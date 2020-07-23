/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_ACPI_H_
#define _SOC_ACPI_H_

#include <acpi/acpi.h>
#include <soc/nvs.h>

#define MEM_BLK_COUNT      0x140
typedef struct {
	uint8_t   buf[32];
} MEM_BLK;

unsigned long northbridge_write_acpi_tables(const struct device *device,
	unsigned long current, struct acpi_rsdp *rsdp);

void motherboard_fill_fadt(acpi_fadt_t *fadt);

#endif /* _SOC_ACPI_H_ */
