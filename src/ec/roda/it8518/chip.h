/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _EC_RODA_IT8518_CHIP_H
#define _EC_RODA_IT8518_CHIP_H

struct ec_roda_it8518_config {
	u8 cpuhot_limit;	/* temperature in °C which asserts PROCHOT# */
};

#endif /* _EC_RODA_IT8518_CHIP_H */
