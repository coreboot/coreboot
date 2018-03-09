/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2003-2017  Cavium Inc. (support@cavium.com). All rights
 * reserved.
 * Copyright 2018-present Facebook, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * string.c: hastily cobbled-together string functions
 */

#include <assert.h>
#include <string.h>
#include <lame_string.h>

static int char_to_val(char c)
{
	if (c >= '0' && c <= '9')	/* digits */
		return c - '0';
	if (c >= 'A' && c <= 'F')	/* uppercase */
		return c - 'A' + 10;
	if (c >= 'a' && c <= 'f')	/* lowercase */
		return c - 'a' + 10;
	return -1;
}

unsigned long long int strtoull(const char *nptr, char **endptr, int base)
{
	unsigned long long int val;
	size_t i, error = 0;

	/* TODO: enforce lameness of this API for now... */
	assert((base == 0) || (base == 16) || base == 10);

	if (!nptr)
		return 0;

	/* Trim whitespace */
	for (i = 0; i < strlen(nptr); i++)
		if (nptr[i] != ' ')
			break;

	if (base == 0) {
		/* Autodetect base */
		if (strlen(&nptr[i]) >= 2 && ((nptr[i] == '0') &&
		    ((nptr[i + 1] == 'x') || (nptr[i + 1] == 'X')))) {
			base = 16;
			i += 2;	/* start loop after prefix */
		} else
			base = 10;
	}

	val = 0;
	for (; i < strlen(nptr); i++) {
		if (base == 16) {
			if (!isxdigit(nptr[i])) {
				if (*endptr)
					*endptr = (char *)&nptr[i];
				error = 1;
				break;
			}
		} else {
			if (!isdigit(nptr[i])) {
				if (*endptr)
					*endptr = (char *)&nptr[i];
				error = 1;
				break;
			}
		}

		val *= base;
		val += char_to_val(nptr[i]);
	}

	if (error) {
		printk(BIOS_ERR, "Failed to convert string '%s', base %d to "
		       "int\n", nptr, base);
		return 0;
	}
	return val;
}

unsigned long int strtoul(const char *nptr, char **endptr, int base)
{
	unsigned long long int u = strtol(nptr, endptr, base);
	/* FIXME: check for overflow (u > max) */
	return (unsigned long int)u;
}

long int strtol(const char *nptr, char **endptr, int base)
{
	unsigned long long int u;
	int is_neg = 0;
	const char *p;
	long int ret;

	if (nptr[0] == '-') {
		is_neg = 1;
		p = &nptr[1];
	} else {
		p = &nptr[0];
	}
	u = strtoull(p, NULL, base);
	/* FIXME: check for overflow (u > max) */
	if (is_neg)
		ret = 0 - (long int)u;
	else
		ret = (long int)u;
	return ret;
}

long long int strtoll(const char *nptr, char **endptr, int base)
{
	unsigned long long int u;
	int is_neg = 0;
	const char *p;
	long long int ret;

	if (nptr[0] == '-') {
		is_neg = 1;
		p = &nptr[1];
	} else {
		p = &nptr[0];
	}
	u = strtoull(p, NULL, base);
	/* FIXME: check for overflow (sign-bit set) */
	if (is_neg)
		ret = 0 - (long long int)u;
	else
		ret = (long long int)u;
	return ret;
}

/* FIXME: replace sscanf() usage for bdk_config_get_int. returns number of
 * strings converted, so 1 if successful and 0 if not */
int str_to_int(const char *str, int64_t *val)
{
	*val = strtol(str, NULL, 10);
	return 1;
}

/* FIXME: replace sscanf() usage for bdk_config_get_int. returns number of
 * strings converted, so 1 if successful and 0 if not */
int str_to_hex(const char *str, int64_t *val)
{
	*val = strtol(str, NULL, 16);
	return 1;
}
