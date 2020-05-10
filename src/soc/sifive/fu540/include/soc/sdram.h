/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_SIFIVE_FU540_SDRAM_H__
#define __SOC_SIFIVE_FU540_SDRAM_H__

#include <types.h>

void sdram_init(void);
size_t sdram_size_mb(void);

#endif
