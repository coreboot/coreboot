/* SPDX-License-Identifier: GPL-2.0-only */

#include "panel.h"

static struct panel_description krane_panels[] = {
	[0] = { .name = "AUO_KD101N80_45NA", .orientation = LB_FB_ORIENTATION_LEFT_UP},
	/* [1] is only Rev3, can be reused. */
	[1] = { .name = "BOE_TV101WUM_NL6", .orientation = LB_FB_ORIENTATION_LEFT_UP},
	[11] = { .name = "BOE_TV101WUM_NL6", .orientation = LB_FB_ORIENTATION_LEFT_UP},
};

struct panel_description *get_panel_description(int panel_id)
{
	if (panel_id < 0 || panel_id >= ARRAY_SIZE(krane_panels))
		return NULL;

	return get_panel_from_cbfs(&krane_panels[panel_id]);
}
