/* SPDX-License-Identifier: GPL-2.0-only */

/* This file contains macro definitions for Qualcomm SoC memlayout.ld linker scripts. */

#ifndef _SOC_QUALCOMM_MEMLAYOUT_H_
#define _SOC_QUALCOMM_MEMLAYOUT_H_

#define SSRAM_START(addr) REGION_START(ssram, addr)
#define SSRAM_END(addr) REGION_END(ssram, addr)

#define BSRAM_START(addr) REGION_START(bsram, addr)
#define BSRAM_END(addr) REGION_END(bsram, addr)

#define AOPSRAM_START(addr) REGION_START(aopsram, addr)
#define AOPSRAM_END(addr) REGION_END(aopsram, addr)

#endif  /* _SOC_QUALCOMM_MEMLAYOUT_H_ */
