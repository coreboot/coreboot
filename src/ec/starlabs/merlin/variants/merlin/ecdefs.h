/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <stdint.h>

/*
 * EC communication interface for ITE Embedded Controller
 */

#ifndef _EC_STARLABS_MERLIN_EC_DEFS_H
#define _EC_STARLABS_MERLIN_EC_DEFS_H

/* EC RAM offsets */
#define ECRAM_KBL_BRIGHTNESS	0x09
#define ECRAM_KBL_TIMEOUT	0x10
#define ECRAM_KBL_STATE		0x0a
#define ECRAM_TRACKPAD_STATE	0x0c
#define ECRAM_FN_LOCK_STATE	0x0f
#define ECRAM_FN_CTRL_REVERSE	0x17
#define ECRAM_MAX_CHARGE	0x1a
#define ECRAM_FAN_MODE		0x1b
#define ECRAM_FAST_CHARGE	dead_code_t(uint8_t)
#define ECRAM_CHARGING_SPEED	0x1d
#define ECRAM_LID_SWITCH	0x1e
#define ECRAM_POWER_LED		0x1f

#endif
