/* SPDX-License-Identifier: GPL-2.0-or-later */

/* TODO: Check if this is still correct */

#ifndef AMD_MENDOCINO_ACPI_H
#define AMD_MENDOCINO_ACPI_H

#include <acpi/acpi.h>
#include <amdblocks/acpi.h>
#include <device/device.h>
#include <stdint.h>

#define ACPI_SCI_IRQ 9

/* RTC Registers */
#define RTC_DATE_ALARM		0x0d
#define RTC_ALT_CENTURY		0x32
#define RTC_CENTURY		0x48

uintptr_t agesa_write_acpi_tables(const struct device *device, uintptr_t current,
				  acpi_rsdp_t *rsdp);

#endif /* AMD_MENDOCINO_ACPI_H */
