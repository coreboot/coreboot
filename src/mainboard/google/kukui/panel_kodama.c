/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include "panel.h"

static struct panel_description kodama_panels[] = {
	[1] = { .name = "AUO_B101UAN08_3", },
	[2] = { .name = "BOE_TV101WUM_N53", },
};

struct panel_description *get_panel_description(int panel_id)
{
	if (panel_id < 0 || panel_id >= ARRAY_SIZE(kodama_panels))
		return NULL;

	return get_panel_from_cbfs(&kodama_panels[panel_id]);
}
