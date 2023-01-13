/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef AMD_PICASSO_ACPI_H
#define AMD_PICASSO_ACPI_H

#include <acpi/acpi.h>
#include <amdblocks/acpi.h>
#include <device/device.h>
#include <stdint.h>

#define ACPI_SCI_IRQ 9

/* RTC Registers */
#define RTC_DATE_ALARM		0x0d

uintptr_t agesa_write_acpi_tables(const struct device *device, uintptr_t current,
				  acpi_rsdp_t *rsdp);

#endif /* AMD_PICASSO_ACPI_H */
