/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <stdint.h>

/*
 * EC communication interface for Nuvoton Embedded Controller.
 */

#ifndef _EC_STARLABS_GLKR_EC_DEFS_H
#define _EC_STARLABS_GLKR_EC_DEFS_H

/* Nuvoton chip ID byte values. */
#define NUVOTON_CHIPID_VAL	0x0004

/* EC RAM offsets. */
#define ECRAM_TRACKPAD_STATE	0x14
#define ECRAM_KBL_STATE		0x16
#define ECRAM_KBL_BRIGHTNESS	0x17
#define ECRAM_KBL_TIMEOUT	0x12
#define ECRAM_FN_LOCK_STATE	0x15
#define ECRAM_FN_CTRL_REVERSE	0x13
#define ECRAM_MAX_CHARGE	dead_code_t(uint8_t)
#define ECRAM_FAN_MODE		dead_code_t(uint8_t)
#define ECRAM_FAST_CHARGE       0x18

#endif
