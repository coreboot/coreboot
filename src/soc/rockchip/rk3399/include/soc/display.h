/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_ROCKCHIP_RK3399_DISPLAY_H__
#define __SOC_ROCKCHIP_RK3399_DISPLAY_H__

#include <edid.h>
#include <soc/mipi.h>

#define REF_CLK_24M (0x1 << 0)

void rk_display_init(struct device *dev);
void mainboard_power_on_backlight(void);
const struct mipi_panel_data *mainboard_get_mipi_mode
				(struct edid_mode *edid_mode);
#endif
