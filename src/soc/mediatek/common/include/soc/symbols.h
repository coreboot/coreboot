/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef _SOC_MEDIATEK_COMMON_SYMBOLS_H_
#define _SOC_MEDIATEK_COMMON_SYMBOLS_H_

#include <symbols.h>

DECLARE_REGION(dram_dma)
DECLARE_REGION(resv_mem_optee)
DECLARE_REGION(resv_mem_gpu)
DECLARE_REGION(resv_mem_gpueb)
DECLARE_REGION(cpucooler_reserved)
DECLARE_REGION(dvfs2_reserved)
DECLARE_REGION(ptp1_reserved)
DECLARE_REGION(thermal_reserved)
DECLARE_OPTIONAL_REGION(mcufw_reserved)

#endif /* _SOC_MEDIATEK_COMMON_SYMBOLS_H_ */
