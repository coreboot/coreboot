/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __AMDBLOCKS_ACPI_H__
#define __AMDBLOCKS_ACPI_H__

#include <stdint.h>

/* ACPI MMIO registers 0xfed80800 */
#define MMIO_ACPI_PM1_STS		0x00
#define MMIO_ACPI_PM1_EN		0x02
#define MMIO_ACPI_PM1_CNT_BLK		0x04
	  /* sleep types defined in arch/x86/include/arch/acpi.h */
#define   ACPI_PM1_CNT_SCIEN		BIT(0)
#define MMIO_ACPI_PM_TMR_BLK		0x08
#define MMIO_ACPI_CPU_CONTROL		0x0c
#define MMIO_ACPI_GPE0_STS		0x14
#define MMIO_ACPI_GPE0_EN		0x18

void acpi_clear_pm1_status(void);

/*
 * If a system reset is about to be requested, modify the PM1 register so it
 * will never be misinterpreted as an S3 resume.
 */
void set_pm1cnt_s5(void);
void acpi_enable_sci(void);
void acpi_disable_sci(void);

#endif /* __AMDBLOCKS_ACPI_H__ */
