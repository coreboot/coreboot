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

static struct panel_description krane_panels[] = {
	[0] = { .name = "AUO_KD101N80_45NA", },
	[1] = { .name = "BOE_TV101WUM_NL6", }, /* Only Rev3, can be reused. */
	[11] = { .name = "BOE_TV101WUM_NL6", },
};

struct panel_description *get_panel_description(int panel_id)
{
	if (panel_id < 0 || panel_id >= ARRAY_SIZE(krane_panels))
		return NULL;

	return get_panel_from_cbfs(&krane_panels[panel_id]);
}
