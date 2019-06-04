/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018-2019 Eltan B.V.
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

#include <soc/ramstage.h>
#include "mainboard.h"

void mainboard_silicon_init_params(SILICON_INIT_UPD *params)
{
	if (CONFIG(FSP1_1_DISPLAY_LOGO)) {
		size_t logo_len;
		void *logo = NULL;

		logo = load_logo(&logo_len);

		if (logo) {
			params->PcdLogoPtr = (u32)logo;
			params->PcdLogoSize = logo_len;
		}
	}
}
