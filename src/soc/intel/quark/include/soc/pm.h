/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef _SOC_PM_H_
#define _SOC_PM_H_

#include <stdint.h>
#include <arch/acpi.h>

struct chipset_power_state {
	uint32_t prev_sleep_state;
} __packed;

struct chipset_power_state *get_power_state(void);
int fill_power_state(void);

/* STM Support */
uint16_t get_pmbase(void);

#endif /* _SOC_PM_H_ */
