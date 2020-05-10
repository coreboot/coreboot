/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef FLASHMAP_LIB_VALSTR_H__
#define FLASHMAP_LIB_VALSTR_H__

#include <inttypes.h>

/* value + string structure for common conversions */
struct valstr {
	uint32_t val;		/* field value */
	const char *str;	/* field description */
};

/*
 * val2str_default  -  convert value to string
 *
 * @val:	value to convert
 * @vs:		value-string data
 * @def_str:	default string to return if no matching value found
 *
 * returns pointer to string
 * returns def_str if no matching value found
 */
const char *val2str_default(uint32_t val, const struct valstr *vs,
			    const char *def_str);

/*
 * val2str  -  convert value to string
 *
 * @val:	value to convert
 * @vs:		value-string data
 *
 * returns pointer to string
 * returns pointer to "unknown" static string if not found
 */
const char *val2str(uint32_t val, const struct valstr *vs);

/*
 * str2val  -  convert string to value
 *
 * @str:	string to convert
 * @vs:		value-string data
 *
 * returns value for string
 * returns value for last entry in value-string data if not found
 */
uint32_t str2val(const char *str, const struct valstr *vs);

#endif	/* FLASHMAP_LIB_VALSTR_H__ */
