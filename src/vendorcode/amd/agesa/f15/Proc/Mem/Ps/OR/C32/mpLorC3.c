/* $NoKeywords:$ */
/**
 * @file
 *
 * mpLorC3.c
 *
 * Platform specific settings for OR C32 DDR3 LRDIMM system
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Ps/OR/C32)
 * @e \$Revision: 56315 $ @e \$Date: 2011-07-11 15:59:14 -0600 (Mon, 11 Jul 2011) $
 *
 **/
/*****************************************************************************
  *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Advanced Micro Devices, Inc. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  * ***************************************************************************
  *
 */

#include "AGESA.h"
#include "AdvancedApi.h"
#include "mport.h"
#include "ma.h"
#include "Ids.h"
#include "cpuFamRegisters.h"
#include "cpuRegisters.h"
#include "mm.h"
#include "mn.h"
#include "mp.h"
#include "mu.h"
#include "GeneralServices.h"
#include "OptionMemory.h"
#include "PlatformMemoryConfiguration.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)


#define FILECODE PROC_MEM_PS_OR_C32_MPLORC3_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
/*
 *-----------------------------------------------------------------------------
 *                                EXPORTED FUNCTIONS
 *
 *-----------------------------------------------------------------------------
 */
// Slow mode, Address timing and Output drive compensation
// Format :
// DimmPerCh,   DDRrate,   VDDIO,   Dimm0,   Dimm1,   Dimm2,   SlowMode,   AddTmgCtl,  ODC
//
STATIC CONST PSCFG_SAO_ENTRY OrC32LRDdr3SAO[] = {
  {1, DDR667, VOLT_ALL, DIMM_LR, NP, NP, 0, 0x00000000, 0x00112222},
  {1, DDR800, VOLT_ALL, DIMM_LR, NP, NP, 0, 0x00000000, 0x10112222},
  {1, DDR1066, VOLT_ALL, DIMM_LR, NP, NP, 0, 0x003C3C3C, 0x20112222},
  {1, DDR1333, VOLT_ALL, DIMM_LR, NP, NP, 0, 0x003A3A3A, 0x30112222},
  {1, DDR1600, V1_5, DIMM_LR, NP, NP, 0, 0x00393939, 0x30112222},
  {1, DDR1600, V1_35, DIMM_LR, NP, NP, 0, 0x00393939, 0x30112222},
  {1, DDR1866, V1_5, DIMM_LR, NP, NP, 0, 0x00393939, 0x30332222},
  {2, DDR667, VOLT_ALL, NP, DIMM_LR, NP, 0, 0x00000000, 0x00112222},
  {2, DDR667, VOLT_ALL, DIMM_LR, DIMM_LR, NP, 0, 0x00000000, 0x10222222},
  {2, DDR800, VOLT_ALL, NP, DIMM_LR, NP, 0, 0x00000000, 0x10112222},
  {2, DDR800, VOLT_ALL, DIMM_LR, DIMM_LR, NP, 0, 0x00000000, 0x20222222},
  {2, DDR1066, VOLT_ALL, NP, DIMM_LR, NP, 0, 0x00393C39, 0x20112222},
  {2, DDR1066, VOLT_ALL, DIMM_LR, DIMM_LR, NP, 0, 0x003A3C3A, 0x30222222},
  {2, DDR1333, VOLT_ALL, NP, DIMM_LR, NP, 0, 0x00373A37, 0x30112222},
  {2, DDR1600, V1_5, NP, DIMM_LR, NP, 0, 0x00363936, 0x30112222},
  {2, DDR1333, V1_5 + V1_35, DIMM_LR, DIMM_LR, NP, 0, 0x00383A38, 0x30222222},
  {3, DDR667, VOLT_ALL, NP, NP, DIMM_LR, 0, 0x00000000, 0x00332222},
  {3, DDR667, VOLT_ALL, DIMM_LR, NP, DIMM_LR, 0, 0x00000000, 0x20222222},
  {3, DDR667, VOLT_ALL, DIMM_LR, DIMM_LR, DIMM_LR, 0, 0x00380038, 0x30112222},
  {3, DDR800, VOLT_ALL, NP, NP, DIMM_LR, 0, 0x00390039, 0x10332222},
  {3, DDR800, VOLT_ALL, DIMM_LR, NP, DIMM_LR, 0, 0x003A003A, 0x30222222},
  {3, DDR800, V1_5 + V1_35, DIMM_LR, DIMM_LR, DIMM_LR, 0, 0x00360036, 0x30112222},
  {3, DDR1066, VOLT_ALL, NP, NP, DIMM_LR, 0, 0x00373C37, 0x20332222},
  {3, DDR1066, VOLT_ALL, DIMM_LR, NP, DIMM_LR, 0, 0x00383C38, 0x30222222},
  {3, DDR1333, VOLT_ALL, NP, NP, DIMM_LR, 0, 0x00353A35, 0x30332222},
  {3, DDR1600, V1_5, NP, NP, DIMM_LR, 0, 0x00333933, 0x30332222},
  {3, DDR800, V1_25, DIMM_LR, DIMM_LR, DIMM_LR, 0, 0x00360036, 0x30112222},
  {3, DDR1066, V1_5, DIMM_LR, DIMM_LR, DIMM_LR, 0, 0x00333C33, 0x30112222},
  {3, DDR1333, V1_5 + V1_35, DIMM_LR, NP, DIMM_LR, 0, 0x00363A36, 0x30222222},
};
CONST PSC_TBL_ENTRY SAOTblEntLRC32 = {
   {PSCFG_SAO, LRDIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_OR, AMD_F15_ALL}, OR_SOCKET_C32, DDR3_TECHNOLOGY},
   sizeof (OrC32LRDdr3SAO) / sizeof (PSCFG_SAO_ENTRY),
   (VOID *)&OrC32LRDdr3SAO
};
// training configuratrions
// Format :
// DimmPerCh,   DDRrate,   VDDIO,   Dimm0,   Dimm1,   Dimm2,   2D
//
STATIC CONST PSCFG_S___ENTRY OrC32LRDdr3S__[] = {
  // DimmPerCh,Frequency,VDDIO,DIMM0,DIMM1,DIMM2,Enable__Training
  {1, DDR667 + DDR800 + DDR1066 + DDR1333, VOLT_ALL, DIMM_LR, NP, NP, 1},
  {1, DDR1600, V1_5, DIMM_LR, NP, NP, 1},
  {1, DDR1600, V1_35, DIMM_LR, NP, NP, 1},
  {1, DDR1866, V1_5, DIMM_LR, NP, NP, 1},
  // DimmPerCh,Frequency,VDDIO,DIMM0,DIMM1,DIMM2,Enable__Training
  {2, DDR667 + DDR800 + DDR1066, VOLT_ALL, NP + DIMM_LR, DIMM_LR, NP, 1},
  {2, DDR1333, VOLT_ALL, NP, DIMM_LR, NP, 1},
  {2, DDR1600, V1_5, NP, DIMM_LR, NP, 1},
  {2, DDR1333, V1_5 + V1_35, DIMM_LR, DIMM_LR, NP, 1},
  // DimmPerCh,Frequency,VDDIO,DIMM0,DIMM1,DIMM2,Enable__Training
  {3, DDR667 + DDR1333, VOLT_ALL, NP, NP, DIMM_LR, 1},
  {3, DDR667, VOLT_ALL, DIMM_LR, NP + DIMM_LR, DIMM_LR, 1},
  {3, DDR800, V1_5 + V1_35, NP, NP, DIMM_LR, 1},
  {3, DDR800, V1_5 + V1_35, DIMM_LR, NP + DIMM_LR, DIMM_LR, 1},
  {3, DDR800, V1_25, NP + DIMM_LR, NP, DIMM_LR, 1},
  {3, DDR1066, VOLT_ALL, NP + DIMM_LR, NP, DIMM_LR, 1},
  {3, DDR1600, V1_5, NP, NP, DIMM_LR, 1},
  {3, DDR800, V1_25, DIMM_LR, DIMM_LR, DIMM_LR, 1},
  {3, DDR1066, V1_5, DIMM_LR, DIMM_LR, DIMM_LR, 1},
  {3, DDR1333, V1_5 + V1_35, DIMM_LR, NP, DIMM_LR, 1},
  };
CONST PSC_TBL_ENTRY S__TblEntLRC32 = {
   {PSCFG_S__, LRDIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_OR, AMD_F15_ALL}, OR_SOCKET_C32, DDR3_TECHNOLOGY},
   sizeof (OrC32LRDdr3S__) / sizeof (PSCFG_S___ENTRY),
   (VOID *)&OrC32LRDdr3S__
};
// ODT pattern for 1 DPC
// Format:
//  Dimm0,   RdODTCSHigh,   RdODTCSLow,   WrODTCSHigh,   WrODTCSLow
//
STATIC CONST PSCFG_1D_ODTPAT_ENTRY Or1LRDdr3OdtPat[] = {
  {DIMM_LR, 0x00000000, 0x00000000, 0x00000101, 0x00000101}
};
CONST PSC_TBL_ENTRY OdtPat1DTblEntLRC32 = {
   {PSCFG_ODT_PAT_1D, LRDIMM_TYPE, _1DIMM, {AMD_FAMILY_15_OR, AMD_F15_ALL}, OR_SOCKET_C32, DDR3_TECHNOLOGY},
   sizeof (Or1LRDdr3OdtPat) / sizeof (PSCFG_1D_ODTPAT_ENTRY),
   (VOID *)&Or1LRDdr3OdtPat
};

// ODT pattern for 2 DPC
// Format:
//  Dimm0,   Dimm1,   RdODTCSHigh,   RdODTCSLow,   WrODTCSHigh,   WrODTCSLow
//
STATIC CONST PSCFG____ODTPAT_ENTRY Or2LRDdr3OdtPat[] = {
  {NP, DIMM_LR, 0x00000000, 0x00000000, 0x02020000, 0x02020000},
  {DIMM_LR, DIMM_LR, 0x01010202, 0x01010202, 0x03030303, 0x03030303}
};
CONST PSC_TBL_ENTRY OdtPat2DTblEntLRC32 = {
   {PSCFG_ODT_PAT___, LRDIMM_TYPE, _2DIMM, {AMD_FAMILY_15_OR, AMD_F15_ALL}, OR_SOCKET_C32, DDR3_TECHNOLOGY},
   sizeof (Or2LRDdr3OdtPat) / sizeof (PSCFG____ODTPAT_ENTRY),
   (VOID *)&Or2LRDdr3OdtPat
};

// ODT pattern for 3 DPC
// Format:
//  Dimm0,   Dimm1,   Dimm2,   RdODTCSHigh,   RdODTCSLow,   WrODTCSHigh,   WrODTCSLow
//
STATIC CONST PSCFG_3D_ODTPAT_ENTRY Or3LRDdr3OdtPat[] = {
  {NP, NP, DIMM_LR, 0x00000000, 0x00000000, 0x00000404, 0x00000000},
  {DIMM_LR, NP, DIMM_LR, 0x00000101, 0x00000404, 0x00000505, 0x00000505},
  {DIMM_LR, DIMM_LR, DIMM_LR, 0x00000303, 0x05050606, 0x00000707, 0x07070707}
};
CONST PSC_TBL_ENTRY OdtPat3DTblEntLRC32 = {
   {PSCFG_ODT_PAT_3D, LRDIMM_TYPE, _3DIMM, {AMD_FAMILY_15_OR, AMD_F15_ALL}, OR_SOCKET_C32, DDR3_TECHNOLOGY},
   sizeof (Or3LRDdr3OdtPat) / sizeof (PSCFG_3D_ODTPAT_ENTRY),
   (VOID *)&Or3LRDdr3OdtPat
};

// Dram Term and Dynamic Dram Term
// Format :
// DimmPerCh,   DDRrate,   VDDIO,   Dimm0,   Dimm1,   Dimm2,  RttNom,   RttWr
//
STATIC CONST PSCFG_LR_RTT_ENTRY DramTermOrC32LRDIMM[] = {
  {1, DDR667 + DDR800, VOLT_ALL, DIMM_LR, NP, NP, 2, 0},
  {1, DDR1066 + DDR1333, VOLT_ALL, DIMM_LR, NP, NP, 1, 0},
  {1, DDR1600, V1_5, DIMM_LR, NP, NP, 3, 0},
  {1, DDR1600, V1_35, DIMM_LR, NP, NP, 3, 0},
  {1, DDR1866, V1_5, DIMM_LR, NP, NP, 3, 0},
  {2, DDR667 + DDR800, VOLT_ALL, NP, DIMM_LR, NP, 2, 0},
  {2, DDR667 + DDR800 + DDR1066, VOLT_ALL, DIMM_LR, DIMM_LR, NP, 3, 2},
  {2, DDR1066 + DDR1333, VOLT_ALL, NP, DIMM_LR, NP, 1, 0},
  {2, DDR1600, V1_5, NP, DIMM_LR, NP, 3, 0},
  {2, DDR1333, V1_5 + V1_35, DIMM_LR, DIMM_LR, NP, 5, 2},
  {3, DDR667 + DDR800, VOLT_ALL, NP, NP, DIMM_LR, 0, 2},
  {3, DDR667, VOLT_ALL, DIMM_LR, NP + DIMM_LR, DIMM_LR, 3, 2},
  {3, DDR800 + DDR1066, VOLT_ALL, DIMM_LR, NP, DIMM_LR, 3, 2},
  {3, DDR800, V1_5 + V1_35, DIMM_LR, DIMM_LR, DIMM_LR, 5, 2},
  {3, DDR1066 + DDR1333, VOLT_ALL, NP, NP, DIMM_LR, 0, 1},
  {3, DDR1600, V1_5, NP, NP, DIMM_LR, 0, 1},
  {3, DDR800, V1_25, DIMM_LR, DIMM_LR, DIMM_LR, 5, 2},
  {3, DDR1066, V1_5, DIMM_LR, DIMM_LR, DIMM_LR, 5, 2},
  {3, DDR1333, V1_5 + V1_35, DIMM_LR, NP, DIMM_LR, 5, 2},
};
CONST PSC_TBL_ENTRY DramTermTblEntLRC32 = {
   {PSCFG_LR_RTT, LRDIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_OR, AMD_F15_ALL}, OR_SOCKET_C32, DDR3_TECHNOLOGY},
   sizeof (DramTermOrC32LRDIMM) / sizeof (PSCFG_LR_RTT_ENTRY),
   (VOID *)&DramTermOrC32LRDIMM
};
// Max Freq.
// Format :
// DimmPerCh,   Dimms,  LR,   Speed1_5V,   Speed1_35V,   Speed1_25V
//
STATIC CONST PSCFG_LR_MAXFREQ_ENTRY ROMDATA MaxFreqOrC32LRDIMM[] = {
  {{1, 1, 1, DDR1600_FREQUENCY, DDR1333_FREQUENCY, DDR1333_FREQUENCY}},
  {{2, 1, 1, DDR1600_FREQUENCY, DDR1333_FREQUENCY, DDR1333_FREQUENCY}},
  {{2, 2, 2, DDR1066_FREQUENCY, DDR1066_FREQUENCY, DDR1066_FREQUENCY}},
  {{3, 1, 1, DDR1600_FREQUENCY, DDR1333_FREQUENCY, DDR1333_FREQUENCY}},
  {{3, 2, 2, DDR1066_FREQUENCY, DDR1066_FREQUENCY, DDR1066_FREQUENCY}},
  {{3, 3, 3, DDR800_FREQUENCY, DDR800_FREQUENCY, DDR667_FREQUENCY}}
};
CONST PSC_TBL_ENTRY MaxFreqTblEntLRC32 = {
   {PSCFG_LR_MAXFREQ, LRDIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_OR, AMD_F15_ALL}, OR_SOCKET_C32, DDR3_TECHNOLOGY},
   sizeof (MaxFreqOrC32LRDIMM) / sizeof (PSCFG_LR_MAXFREQ_ENTRY),
   (VOID *)&MaxFreqOrC32LRDIMM
};

// IBT
// Format :
// DimmPerCh,   DDRrate,   VDDIO,   Dimm0,   Dimm1,   Dimm2,   F0RC8,   F1RC0,   F1RC1,   F1RC2
//
STATIC CONST PSCFG_L_IBT_ENTRY OrLRDdr3IBT[] = {
  {1, DDR667 + DDR800, VOLT_ALL, DIMM_LR, NP, NP, 1, 1, 1, 1},
  {1, DDR1066 + DDR1333, VOLT_ALL, DIMM_LR, NP, NP, 0, 0, 0, 0},
  {1, DDR1600, V1_5, DIMM_LR, NP, NP, 0, 0, 0, 0},
  {1, DDR1600, V1_35, DIMM_LR, NP, NP, 0, 0, 0, 0},
  {1, DDR1866, V1_5, DIMM_LR, NP, NP, 0, 0, 0, 0},
  {2, DDR667 + DDR800, VOLT_ALL, NP + DIMM_LR, DIMM_LR, NP, 1, 1, 1, 1},
  {2, DDR1066 + DDR1333, VOLT_ALL, NP, DIMM_LR, NP, 0, 0, 0, 0},
  {2, DDR1066, VOLT_ALL, DIMM_LR, DIMM_LR, NP, 1, 1, 1, 1},
  {2, DDR1600, V1_5, NP, DIMM_LR, NP, 0, 0, 0, 0},
  {2, DDR1333, V1_5 + V1_35, DIMM_LR, DIMM_LR, NP, 1, 1, 1, 1},
  {3, DDR667, VOLT_ALL, NP, NP, DIMM_LR, 1, 1, 1, 1},
  {3, DDR667, VOLT_ALL, DIMM_LR, NP + DIMM_LR, DIMM_LR, 1, 1, 1, 1},
  {3, DDR800 + DDR1066 + DDR1333, VOLT_ALL, NP, NP, DIMM_LR, 0, 0, 0, 0},
  {3, DDR800, V1_5 + V1_35, DIMM_LR, NP + DIMM_LR, DIMM_LR, 1, 1, 1, 1},
  {3, DDR800, V1_25, DIMM_LR, NP, DIMM_LR, 1, 1, 1, 1},
  {3, DDR1066, VOLT_ALL, DIMM_LR, NP, DIMM_LR, 1, 1, 1, 1},
  {3, DDR1600, V1_5, NP, NP, DIMM_LR, 0, 0, 0, 0},
  {3, DDR800, V1_25, DIMM_LR, DIMM_LR, DIMM_LR, 1, 1, 1, 1},
  {3, DDR1066, V1_5, DIMM_LR, DIMM_LR, DIMM_LR, 1, 1, 1, 1},
  {3, DDR1333, V1_5 + V1_35, DIMM_LR, NP, DIMM_LR, 1, 1, 1, 1},
};
CONST PSC_TBL_ENTRY IBTTblEntLRC32 = {
   {PSCFG_LR_IBT, LRDIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_OR, AMD_F15_ALL}, OR_SOCKET_C32, DDR3_TECHNOLOGY},
   sizeof (OrLRDdr3IBT) / sizeof (PSCFG_L_IBT_ENTRY),
   (VOID *)&OrLRDdr3IBT
};

//
// MemClkDis
//
STATIC CONST UINT8 ROMDATA Or3LRDdr3CLKDis[] = {0x03, 0x0C, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00};
CONST PSC_TBL_ENTRY ClkDisMapEntLRC32 = {
   {PSCFG_CLKDIS, LRDIMM_TYPE, _1DIMM + _2DIMM + _3DIMM, {AMD_FAMILY_15_OR, AMD_F15_ALL}, OR_SOCKET_C32, DDR3_TECHNOLOGY},
   sizeof (Or3LRDdr3CLKDis) / sizeof (UINT8),
   (VOID *)&Or3LRDdr3CLKDis
};

//
// WL pass1 seed
//
// Format :
// DimmPerCh in bit map,   Channel #,   Seed value
STATIC CONST PSCFG_SEED_ENTRY ROMDATA WLPas1SeedOrC32LRDIMM[] = {
  {_1DIMM + _2DIMM + _3DIMM, CH_ALL, 0xF7}
};
CONST PSC_TBL_ENTRY WLPass1SeedEntLRC32 = {
   {PSCFG_WL_PASS1_SEED, LRDIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_OR, AMD_F15_ALL}, OR_SOCKET_C32, DDR3_TECHNOLOGY},
   sizeof (WLPas1SeedOrC32LRDIMM) / sizeof (PSCFG_SEED_ENTRY),
   (VOID *)&WLPas1SeedOrC32LRDIMM
};

//
// HW RxEn pass1 seed
//
// Format :
// DimmPerCh in bit map,   Channel #,   Seed value
STATIC CONST PSCFG_SEED_ENTRY ROMDATA HWRxEnPas1SeedOrC32LRDIMM[] = {
  {_1DIMM + _2DIMM + _3DIMM, CH_A, 0x132},
  {_1DIMM + _2DIMM + _3DIMM, CH_B, 0x122}
};
CONST PSC_TBL_ENTRY HWRxEnPass1SeedEntLRC32 = {
   {PSCFG_HWRXEN_PASS1_SEED, LRDIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_OR, AMD_F15_ALL}, OR_SOCKET_C32, DDR3_TECHNOLOGY},
   sizeof (HWRxEnPas1SeedOrC32LRDIMM) / sizeof (PSCFG_SEED_ENTRY),
   (VOID *)&HWRxEnPas1SeedOrC32LRDIMM
};