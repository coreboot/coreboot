/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _OPTION_H_
#define _OPTION_H_

#include <types.h>

void sanitize_cmos(void);

enum cb_err cmos_set_option(const char *name, void *val);
enum cb_err cmos_get_option(void *dest, const char *name);

static inline enum cb_err set_int_option(const char *name, int value)
{
	if (CONFIG(USE_OPTION_TABLE))
		return cmos_set_option(name, &value);

	return CB_CMOS_OTABLE_DISABLED;
}

static inline int get_int_option(const char *name, const int fallback)
{
	if (CONFIG(USE_OPTION_TABLE)) {
		int value = 0;
		if (cmos_get_option(&value, name) == CB_SUCCESS)
			return value;
	}
	return fallback;
}

#endif /* _OPTION_H_ */
