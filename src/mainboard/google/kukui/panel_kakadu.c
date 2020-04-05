/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include "panel.h"

static struct panel_description kakadu_panels[] = {
	[1] = { .name = "BOE_TV105WUM_NW0", },
};

struct panel_description *get_panel_description(int panel_id)
{
	if (panel_id < 0 || panel_id >= ARRAY_SIZE(kakadu_panels))
		return NULL;

	return get_panel_from_cbfs(&kakadu_panels[panel_id]);
}
