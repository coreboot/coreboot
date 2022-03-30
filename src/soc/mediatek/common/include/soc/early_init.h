/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_EARLY_INIT_H
#define SOC_MEDIATEK_EARLY_INIT_H

#include <soc/symbols.h>
#include <stdint.h>
#include <symbols.h>
#include <timer.h>

DECLARE_REGION(early_init_data);

enum early_init_type {
	EARLY_INIT_PCIE,
	EARLY_INIT_MAX,
};

struct early_init_data {
	struct mono_time init_time[EARLY_INIT_MAX];
};

void early_init_clear(void);
void early_init_save_time(enum early_init_type init_type);
long early_init_get_elapsed_time_us(enum early_init_type init_type);

#endif /* SOC_MEDIATEK_EARLY_INIT_H */
