/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
 * Copyright (C) 2016 Damien Zammit <damien@zamaudio.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef MCTI_H
#define MCTI_H

#include <inttypes.h>
#include <stdlib.h>
#include <pc80/mc146818rtc.h>

struct DCTStatStruc;
struct MCTStatStruc;

#define SERVER		0
#define DESKTOP		1
//#define MOBILE	2
#define REV_F		0
#define REV_DR		1
#define REV_FDR		2

/*----------------------------------------------------------------------------
COMMENT OUT ALL BUT 1
----------------------------------------------------------------------------*/
//#define BUILD_VERSION   REV_F		/*BIOS supports rev F only*/
//#define BUILD_VERSION   REV_DR	/*BIOS supports rev 10 only*/
//#define BUILD_VERSION   REV_FDR	/*BIOS supports both rev F and 10*/

/*----------------------------------------------------------------------------
COMMENT OUT ALL BUT 1
----------------------------------------------------------------------------*/
#ifndef SYSTEM_TYPE
#define    SYSTEM_TYPE	    SERVER
//#define    SYSTEM_TYPE     DESKTOP
//#define    SYSTEM_TYPE     MOBILE
#endif

/*----------------------------------------------------------------------------
UPDATE AS NEEDED
----------------------------------------------------------------------------*/
#ifndef MAX_NODES_SUPPORTED
#define MAX_NODES_SUPPORTED		8
#endif

#ifndef MAX_DIMMS_SUPPORTED
#if IS_ENABLED(CONFIG_DIMM_DDR3)
 #define MAX_DIMMS_SUPPORTED		6
#else
 #define MAX_DIMMS_SUPPORTED		8
#endif
#endif

#ifndef MAX_CS_SUPPORTED
#define MAX_CS_SUPPORTED		8
#endif

#ifndef MCT_DIMM_SPARE_NO_WARM
#define MCT_DIMM_SPARE_NO_WARM	0
#endif

#ifndef MEM_MAX_LOAD_FREQ
#if IS_ENABLED(CONFIG_DIMM_DDR3)
 #define MEM_MAX_LOAD_FREQ			933
 #define MEM_MIN_PLATFORM_FREQ_FAM10		400
 #define MEM_MIN_PLATFORM_FREQ_FAM15		333
#else						 /* AMD_FAM10_DDR2 */
 #define MEM_MAX_LOAD_FREQ			400
 #define MEM_MIN_PLATFORM_FREQ_FAM10		200
 /* DDR2 not available on Family 15h */
 #define MEM_MIN_PLATFORM_FREQ_FAM15		0
#endif
#endif

#define MCT_TRNG_KEEPOUT_START		0x00000C00
#define MCT_TRNG_KEEPOUT_END		0x00000CFF

#define NVRAM_DDR2_800 0
#define NVRAM_DDR2_667 1
#define NVRAM_DDR2_533 2
#define NVRAM_DDR2_400 3

#define NVRAM_DDR3_1600 0
#define NVRAM_DDR3_1333 1
#define NVRAM_DDR3_1066 2
#define NVRAM_DDR3_800  3

/* The recommended maximum GFX Upper Memory Area
 * size is 256M, however, to be on the safe side
 * move TOM down by 512M.
 */
#define MAXIMUM_GFXUMA_SIZE 0x20000000

/* Do not allow less than 16M of DRAM in 32-bit space.
 * This number is not hardware constrained and can be
 * changed as needed.
 */
#define MINIMUM_DRAM_BELOW_4G 0x1000000

static const uint16_t ddr2_limits[4] = {400, 333, 266, 200};
static const uint16_t ddr3_limits[16] = {933, 800, 666, 533, 400, 333, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

#if IS_ENABLED(CONFIG_DIMM_DDR3)
  #include <northbridge/amd/amdmct/mct_ddr3/mct_d.h>
#else
  #include <northbridge/amd/amdmct/mct/mct_d.h>
#endif

#if IS_ENABLED(CONFIG_DIMM_DDR2)
void mctSaveDQSSigTmg_D(void);
void mctGetDQSSigTmg_D(void);
u8 mctSetNodeBoundary_D(void);
#endif
u16 mctGet_NVbits(u8 index);
void mctHookAfterDIMMpre(void);
void mctGet_MaxLoadFreq(struct DCTStatStruc *pDCTstat);
void mctAdjustAutoCycTmg_D(void);
void mctHookAfterAutoCycTmg(void);
void mctGetCS_ExcludeMap(void);
void mctHookBeforeECC(void);
void mctHookAfterECC(void);
void mctHookAfterAutoCfg(void);
void mctHookAfterPSCfg(void);
void mctHookAfterHTMap(void);
void mctHookAfterCPU(void);
void mctInitMemGPIOs_A_D(void);
void mctNodeIDDebugPort_D(void);
void mctWarmReset_D(void);
void mctHookBeforeDramInit(void);
void mctHookAfterDramInit(void);
void mctHookBeforeAnyTraining(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstatA);
void mctHookAfterAnyTraining(void);
uint64_t mctGetLogicalCPUID_D(u8 node);

#if IS_ENABLED(CONFIG_DIMM_DDR3)
void vErratum372(struct DCTStatStruc *pDCTstat);
void vErratum414(struct DCTStatStruc *pDCTstat);
u32 mct_AdjustSPDTimings(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstatA, u32 val);
#endif

#endif
