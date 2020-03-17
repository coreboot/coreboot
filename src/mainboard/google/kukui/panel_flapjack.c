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

#include "panel.h"

static struct panel_description flapjack_panels[] = {
	[0] = { .name = "BOE_TV101WUM_NG0", },
	[1] = { .name = "BOE_TV080WUM_NG0", },
	[2] = { .name = "INX_OTA7290D10P", },
	[3] = { .name = "AUO_NT51021D8P", },
};

struct panel_description *get_panel_description(int panel_id)
{
	if (panel_id < 0 || panel_id >= ARRAY_SIZE(flapjack_panels))
		return NULL;

	return get_panel_from_cbfs(&flapjack_panels[panel_id]);
}
