/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _EC_STARLABS_IT8987_CHIP_H
#define _EC_STARLABS_IT8987_CHIP_H

struct ec_starlabs_it8987_config {
	u8 cpuhot_limit;	/* temperature in °C which asserts PROCHOT# */
};

#endif /* _EC_STARLABS_IT8987_CHIP_H */
