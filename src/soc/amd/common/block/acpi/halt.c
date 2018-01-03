/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Google Inc.
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

#include <arch/acpi.h>
#include <soc/southbridge.h>
#include <halt.h>

void poweroff(void)
{
	outl((SLP_TYP_S5 << SLP_TYP_SHIFT) | SLP_EN, pm_acpi_pm_cnt_blk());
	halt();
}
