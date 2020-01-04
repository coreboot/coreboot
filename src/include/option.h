/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _OPTION_H_
#define _OPTION_H_

#include <types.h>

void sanitize_cmos(void);

enum cb_err cmos_set_option(const char *name, void *val);
enum cb_err cmos_get_option(void *dest, const char *name);

static inline enum cb_err set_option(const char *name, void *val)
{
	if (CONFIG(USE_OPTION_TABLE))
		return cmos_set_option(name, val);

	return CB_CMOS_OTABLE_DISABLED;
}

static inline enum cb_err get_option(void *dest, const char *name)
{
	if (CONFIG(USE_OPTION_TABLE))
		return cmos_get_option(dest, name);

	return CB_CMOS_OTABLE_DISABLED;
}

#endif /* _OPTION_H_ */
