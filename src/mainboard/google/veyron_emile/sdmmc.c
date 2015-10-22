/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "board.h"

#include <soc/rk808.h>

static void sdmmc_power(int enable)
{
	rk808_configure_ldo(4, enable ? 3300 : 0); /* VCC33_SD_LDO */
	rk808_configure_ldo(5, enable ? 3300 : 0); /* VCCIO_SD */
}

void sdmmc_power_off(void)
{
	sdmmc_power(0);
}

void sdmmc_power_on(void)
{
	sdmmc_power(1);
}
