/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _PANEL_H_
#define _PANEL_H_

#include <types.h>

struct panel_data {
	uint8_t lanes;
	struct mipi_dsi_cmd *init_cmd;
	uint32_t init_cmd_count;
};

void panel_power_on(void);
const struct panel_data *get_panel_config(struct edid *edid);

#endif /*_PANEL_H_ */
