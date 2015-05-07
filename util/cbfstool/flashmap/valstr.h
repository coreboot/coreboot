/*
 * Copyright 2010, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
