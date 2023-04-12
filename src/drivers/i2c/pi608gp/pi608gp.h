/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _I2C_PI608GP_H_
#define _I2C_PI608GP_H_

/* Struct to store fixed-point millivolt values */
struct deemph_lvl { uint32_t lvl, lvl_10; };

#define AMP_LVL_MV(_LVL)		(_LVL)
#define DEEMPH_LVL_MV(_LVL, _LVL_10)	{ .lvl = _LVL, .lvl_10 = _LVL_10 }

#endif
