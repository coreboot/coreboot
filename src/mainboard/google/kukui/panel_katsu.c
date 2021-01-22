/* SPDX-License-Identifier: GPL-2.0-only */

#include "panel.h"

static struct panel_description katsu_panels[] = {
	[1] = { .name = "BOE_TV105WUM_NW0", },
	[2] = { .name = "STA_2081101QFH032011_53G", },
};

struct panel_description *get_panel_description(int panel_id)
{
	if (panel_id < 0 || panel_id >= ARRAY_SIZE(katsu_panels))
		return NULL;

	return get_panel_from_cbfs(&katsu_panels[panel_id]);
}
