/* SPDX-License-Identifier: GPL-2.0-only */

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
