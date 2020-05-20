/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __PICASSO_SOC_UTIL_H__
#define __PICASSO_SOC_UTIL_H__

#include <types.h>

enum socket_type {
	SOCKET_FP5 = 0,
	SOCKET_AM4 = 2,
	SOCKET_FT5 = 3,
};

void print_socket_type(void);

bool soc_is_pollock(void);
bool soc_is_dali(void);
bool soc_is_picasso(void);
bool soc_is_raven2(void);
bool soc_is_zen_plus(void);

#endif /* __PICASSO_SOC_UTIL_H__ */
