/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef __SOC_CAVIUM_CN81XX_SDRAM_H__
#define __SOC_CAVIUM_CN81XX_SDRAM_H__

#include <types.h>

size_t sdram_size_mb(void);
void sdram_init(void);

#endif	/* !__SOC_CAVIUM_CN81XX_SDRAM_H__ */
