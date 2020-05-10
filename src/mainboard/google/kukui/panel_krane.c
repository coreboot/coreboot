/* SPDX-License-Identifier: GPL-2.0-only */

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
