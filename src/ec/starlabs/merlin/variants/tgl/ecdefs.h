/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <stdint.h>

/*
 * EC communication interface for ITE Embedded Controller
 */

#ifndef _EC_STARLABS_TGL_EC_DEFS_H
#define _EC_STARLABS_TGL_EC_DEFS_H

/* IT5570 chip ID byte values */
#define ITE_CHIPID_VAL		0x5570

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

#endif
