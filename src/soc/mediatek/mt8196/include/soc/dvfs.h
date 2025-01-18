/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef __SOC_MEDIATEK_MT8196_DVFS_H__
#define __SOC_MEDIATEK_MT8196_DVFS_H__

#include <soc/addressmap.h>

#define CACHE_LINE	64
#define CSRAM_OFFSET	0x0001BC00
#define CSRAM_BASE	(SRAM_BASE + CSRAM_OFFSET)

void dvfs_init(void);

#endif
