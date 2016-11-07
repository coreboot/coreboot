/*
 * Copyright (c) 2013-2016, The Regents of the University of California
 *(Regents).
 * All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Regents nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * IN NO EVENT SHALL REGENTS BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT,
 * SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS,
 * ARISING
 * OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF REGENTS HAS
 * BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE. THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED
 * HEREUNDER IS PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE
 * MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */

#ifndef RISCV_CONFIG_STRING_H
#define RISCV_CONFIG_STRING_H

#include <stddef.h>
#include <stdint.h>

static const char *skip_whitespace(const char *str)
{
	while (*str && *str <= ' ')
		str++;
	return str;
}

static const char *skip_string(const char *str)
{
	while (*str && *str++ != '"')
		;
	return str;
}

static int is_hex(char ch)
{
	return (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') ||
	       (ch >= 'A' && ch <= 'F');
}

static int parse_hex(char ch)
{
	return (ch >= '0' && ch <= '9') ? ch - '0' : (ch >= 'a' && ch <= 'f')
							 ? ch - 'a' + 10
							 : ch - 'A' + 10;
}

static const char *skip_key(const char *str)
{
	while (*str >= 35 && *str <= 122 && *str != ';')
		str++;
	return str;
}

typedef struct {
	const char *start;
	const char *end;
} query_result;

static inline query_result query_config_string(const char *str, const char *k)
{
	size_t ksize = 0;
	while (k[ksize] && k[ksize] != '{')
		ksize++;
	int last = !k[ksize];

	query_result res = {0, 0};
	while (1) {
		const char *key_start = str = skip_whitespace(str);
		const char *key_end = str = skip_key(str);
		int match = (size_t)(key_end - key_start) == ksize;
		if (match)
			for (size_t i = 0; i < ksize; i++)
				if (key_start[i] != k[i])
					match = 0;
		const char *value_start = str = skip_whitespace(str);
		while (*str != ';') {
			if (!*str) {
				return res;
			} else if (*str == '"') {
				str = skip_string(str + 1);
			} else if (*str == '{') {
				const char *search_key =
				    match && !last ? k + ksize + 1 : "";
				query_result inner_res =
				    query_config_string(str + 1, search_key);
				if (inner_res.start)
					return inner_res;
				str = inner_res.end + 1;
			} else {
				str = skip_key(str);
			}
			str = skip_whitespace(str);
		}
		res.end = str;
		if (match && last) {
			res.start = value_start;
			return res;
		}
		str = skip_whitespace(str + 1);
		if (*str == '}') {
			res.end = str;
			return res;
		}
	}
}

static void parse_string(query_result r, char *buf)
{
	if (r.start < r.end) {
		if (*r.start == '"') {
			for (const char *p = r.start + 1;
			     p < r.end && *p != '"'; p++) {
				char ch = p[0];
				if (ch == '\\' && p[1] == 'x' && is_hex(p[2])) {
					ch = parse_hex(p[2]);
					if (is_hex(p[3])) {
						ch =
						    (ch << 4) + parse_hex(p[3]);
						p++;
					}
					p += 2;
				}
				*buf++ = ch;
			}
		} else {
			for (const char *p = r.start; p < r.end && *p > ' ';
			     p++)
				*buf++ = *p;
		}
	}
	*buf = 0;
}

static uint64_t __get_uint_hex(const char *s)
{
	uint64_t res = 0;
	while (*s) {
		if (is_hex(*s))
			res = (res << 4) + parse_hex(*s);
		else if (*s != '_')
			break;
		s++;
	}
	return res;
}

static uint64_t __get_uint_dec(const char *s)
{
	uint64_t res = 0;
	while (*s) {
		if (*s >= '0' && *s <= '9')
			res = res * 10 + (*s - '0');
		else
			break;
		s++;
	}
	return res;
}

static uint64_t __get_uint(const char *s)
{
	if (s[0] == '0' && s[1] == 'x')
		return __get_uint_hex(s + 2);
	return __get_uint_dec(s);
}

// This is nasty. But the original code used a much nastier thing,
// an array declared as an automatic, which triggered gcc warnings
// as it was sized by the res.end - res.start. We'll need a better
// solution, long term. Malloc is probably not it.
static char name[1024];
static inline uint64_t get_uint(query_result res)
{
	uint64_t v;
	parse_string(res, name);
	v = __get_uint(name);
	return v;
}

static inline int64_t get_sint(query_result res)
{
	int64_t v;
	parse_string(res, name);
	if (name[0] == '-')
		return -__get_uint(name + 1);
	v = __get_uint(name);
	return v;
}

const char *configstring(void);
void query_mem(const char *config_string, uintptr_t *base, size_t *size);
void query_rtc(const char *config_string, uintptr_t *mtime);
#endif
