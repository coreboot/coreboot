/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOUTHBRIDGE_INTEL_COMMON_HPET_H
#define SOUTHBRIDGE_INTEL_COMMON_HPET_H

#include <stdint.h>

void enable_hpet(void);
void hpet_udelay(u32 delay);

#endif /* SOUTHBRIDGE_INTEL_COMMON_HPET_H */
