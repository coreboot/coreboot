/* SPDX-License-Identifier: GPL-2.0-only */

#include "panel.h"

static struct panel_description flapjack_panels[] = {
	[0] = { .name = "BOE_TV101WUM_NG0", .orientation = LB_FB_ORIENTATION_NORMAL},
	[1] = { .name = "BOE_TV080WUM_NG0", .orientation = LB_FB_ORIENTATION_NORMAL},
	[2] = { .name = "INX_OTA7290D10P", .orientation = LB_FB_ORIENTATION_NORMAL},
	[3] = { .name = "AUO_NT51021D8P", .orientation = LB_FB_ORIENTATION_NORMAL},
};

struct panel_description *get_panel_description(int panel_id)
{
	if (panel_id < 0 || panel_id >= ARRAY_SIZE(flapjack_panels))
		return NULL;

	return get_panel_from_cbfs(&flapjack_panels[panel_id]);
}
