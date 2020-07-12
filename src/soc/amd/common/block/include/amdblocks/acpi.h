/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __AMDBLOCKS_ACPI_H__
#define __AMDBLOCKS_ACPI_H__

#include <types.h>

/* ACPI MMIO registers 0xfed80800 */
#define MMIO_ACPI_PM1_STS		0x00
#define MMIO_ACPI_PM1_EN		0x02
#define MMIO_ACPI_PM1_CNT_BLK		0x04
	  /* sleep types defined in arch/x86/include/acpi/acpi.h */
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
