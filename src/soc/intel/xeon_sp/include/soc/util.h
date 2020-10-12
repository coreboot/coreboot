/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _XEON_SP_SOC_UTIL_H_
#define _XEON_SP_SOC_UTIL_H_

#include <hob_iiouds.h>

void get_cpubusnos(uint32_t *bus0, uint32_t *bus1, uint32_t *bus2, uint32_t *bus3);
void unlock_pam_regions(void);
void get_stack_busnos(uint32_t *bus);

#endif
