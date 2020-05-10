/* SPDX-License-Identifier: BSD-3-Clause */

#include <inttypes.h>
#include <string.h>
#include <strings.h>

#include <valstr.h>

const char *val2str_default(uint32_t val, const struct valstr *vs,
			    const char *def_str)
{
	int i;

	for (i = 0; vs[i].str; i++) {
		if (vs[i].val == val)
			return vs[i].str;
	}

	return def_str;
}

const char *val2str(uint32_t val, const struct valstr *vs)
{
	return val2str_default(val, vs, "Unknown");
}

uint32_t str2val(const char *str, const struct valstr *vs)
{
	int i;

	for (i = 0; vs[i].str; i++) {
		if (strcasecmp(vs[i].str, str) == 0)
			return vs[i].val;
	}

	return vs[i].val;
}
