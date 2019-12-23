/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 Intel Corp.
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

#include <intelblocks/smbus.h>
#include <intelblocks/tco.h>
#include <soc/romstage.h>

void romstage_pch_init(void)
{
	/* Program TCO_BASE_ADDRESS and TCO Timer Halt */
	tco_configure();

	/* Program SMBUS_BASE_ADDRESS and enable it */
	smbus_common_init();
}
