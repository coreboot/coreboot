/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_COMMON_EINT_EVENT_H
#define SOC_MEDIATEK_COMMON_EINT_EVENT_H

#include <stddef.h>
#include <stdint.h>

struct eint_event_info {
	uintptr_t reg_base;
	size_t eint_num;
};

/* An element { 0, 0 } indicates the end of array. */
extern const struct eint_event_info eint_event[];

/* Unmask eint event, which can be waken up by SPM. */
void unmask_eint_event_mask(void);

#endif
