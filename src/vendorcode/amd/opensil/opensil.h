/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _OPENSIL_H_
#define _OPENSIL_H_

#include <acpi/acpi.h>
#include <device/device.h>
#include <types.h>

void SIL_STATUS_report(const char *function, const int status);
// Add the memory map to dev, starting at index idx, returns last use idx
void add_opensil_memmap(struct device *dev, unsigned long *idx);

void opensil_fill_fadt(acpi_fadt_t *fadt);
unsigned long add_opensil_acpi_table(unsigned long current, acpi_rsdp_t *rsdp);

uintptr_t opensil_get_low_usable_dram_address(void);

void setup_opensil(void);
void opensil_xSIM_timepoint_1(void);
void opensil_xSIM_timepoint_2(void);
void opensil_xSIM_timepoint_3(void);

void opensil_mpio_global_config(void);
void opensil_mpio_per_device_config(struct device *dev);

#endif
