/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_QUALCOMM_CALYPSO_EDP_CTRL_H__
#define __SOC_QUALCOMM_CALYPSO_EDP_CTRL_H__

#include <types.h>

enum cb_err edp_ctrl_init(struct edid *edid);
enum cb_err edp_backlight_aux(uint16_t level, uint16_t max_level);

#endif
