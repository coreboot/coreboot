/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <stdint.h>
#include <arch/io.h>
#include <arch/acpi.h>
#include "hudson.h"

int acpi_get_sleep_type(void)
{
	u16 tmp = inw(ACPI_PM1_CNT_BLK);
	tmp = ((tmp & (7 << 10)) >> 10);
	return (int)tmp;
}
