/*
 * Copyright (c) 2018 Facebook Inc.
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

#ifndef __PLAT_PARAMS_H__
#define __PLAT_PARAMS_H__

#include <stdint.h>

/* param type */
enum {
	PARAM_NONE = 0,
	PARAM_FDT,
	PARAM_COREBOOT_TABLE,
};

/* common header for all plat parameter type */
struct bl31_plat_param {
	uint64_t type;
	void *next;
};

struct bl31_fdt_param {
	struct bl31_plat_param h;
	uint64_t fdt_ptr;
};

struct bl31_u64_param {
	struct bl31_plat_param h;
	uint64_t value;
};

void params_early_setup(void *ptr);

#endif /* __PLAT_PARAMS_H__ */
