/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018-present Facebook, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __LAME_STRING_H__
#define __LAME_STRING_H__

long int strtol(const char *nptr, char **endptr, int base);
long long int strtoll(const char *nptr, char **endptr, int base);
unsigned long int strtoul(const char *nptr, char **endptr, int base);
unsigned long long int strtoull(const char *nptr, char **endptr, int base);
int str_to_hex(const char *str, int64_t *val);
int str_to_int(const char *str, int64_t *val);

#endif
