/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __CPU_TI_AM335X_DMTIMER_H__
#define __CPU_TI_AM335X_DMTIMER_H__

#include <stdint.h>

#define OSC_HZ 24000000

void dmtimer_start(int num);
uint64_t dmtimer_raw_value(int num);

#endif
