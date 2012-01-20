/* $NoKeywords:$ */
/**
 * @file
 *
 * mpUorA3.c
 *
 * Platform specific settings for OR AM3 DDR3 U-DIMM system
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Ps/OR/AM3)
 * @e \$Revision: 55134 $ @e \$Date: 2011-06-16 15:27:02 -0600 (Thu, 16 Jun 2011) $
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
#include "PlatformMemoryConfiguration.h"
#include "ma.h"
#include "Ids.h"
#include "cpuFamRegisters.h"
#include "cpuRegisters.h"
#include "mm.h"
#include "mn.h"
#include "mp.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_MEM_PS_OR_AM3_MPUORA3_FILECODE
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
// DimmPerCh,   DDRrate,   VDDIO,   Dimm0,   Dimm1,   Dimm2,   SlowMode,   AddTmgCtl,   ODC
//
STATIC CONST PSCFG_SAO_ENTRY OrAM3UDdr3SAO[] = {
  {1, DDR667 + DDR800, V1_5, DIMM_SR, NP, NP, 0, 0x00000000, 0x00112222},
  {1, DDR667 + DDR800, V1_5, DIMM_DR, NP, NP, 0, 0x003B0000, 0x00112222},
  {1, DDR1066, V1_5, DIMM_SR, NP, NP, 0, 0x00000000, 0x10112222},
  {1, DDR1066, V1_5, DIMM_DR, NP, NP, 0, 0x00380000, 0x10112222},
  {1, DDR1333, V1_5, DIMM_SR, NP, NP, 0, 0x00000000, 0x20112222},
  {1, DDR1333, V1_5, DIMM_DR, NP, NP, 0, 0x00360000, 0x20112222},
  {1, DDR1600, V1_5, DIMM_SR, NP, NP, 0, 0x00000000, 0x30112222},
  {1, DDR1600, V1_5, DIMM_DR, NP, NP, 1, 0x00000000, 0x30112222},
  {1, DDR1866, V1_5, DIMM_SR, NP, NP, 0, 0x00000000, 0x30332222},
  {1, DDR1866, V1_5, DIMM_DR, NP, NP, 1, 0x00000000, 0x30332222},
  {2, DDR667 + DDR800, V1_5, NP, DIMM_SR, NP, 0, 0x00000000, 0x00112222},
  {2, DDR667 + DDR800, V1_5, NP, DIMM_DR, NP, 0, 0x003B0000, 0x00112222},
  {2, DDR667, V1_5, DIMM_SR + DIMM_DR, DIMM_SR + DIMM_DR, NP, 0, 0x00390039, 0x10222322},
  {2, DDR800, V1_5, DIMM_SR + DIMM_DR, DIMM_SR + DIMM_DR, NP, 0, 0x00390039, 0x20222322},
  {2, DDR1066, V1_5, NP, DIMM_SR, NP, 0, 0x00000000, 0x10112222},
  {2, DDR1066, V1_5, NP, DIMM_DR, NP, 0, 0x00380000, 0x10112222},
  {2, DDR1066, V1_5, DIMM_SR + DIMM_DR, DIMM_SR + DIMM_DR, NP, 0, 0x00350037, 0x30222322},
  {2, DDR1333, V1_5, NP, DIMM_SR, NP, 0, 0x00000000, 0x20112222},
  {2, DDR1333, V1_5, NP, DIMM_DR, NP, 0, 0x00360000, 0x20112222},
  {2, DDR1333, V1_5, DIMM_SR + DIMM_DR, DIMM_SR + DIMM_DR, NP, 1, 0x00000035, 0x30222322},
  {2, DDR1600, V1_5, NP, DIMM_SR, NP, 0, 0x00000000, 0x30112222},
  {2, DDR1600, V1_5, NP, DIMM_DR, NP, 1, 0x00000000, 0x30112222},
  {2, DDR1600, V1_5, DIMM_SR, DIMM_SR, NP, 1, 0x00000033, 0x30222322},
  {2, DDR1600, V1_5, DIMM_SR, DIMM_DR, NP, 1, 0x00000033, 0x30222322},
  {2, DDR1600, V1_5, DIMM_DR, DIMM_SR + DIMM_DR, NP, 1, 0x00000033, 0x30222322},
  {2, DDR1866, V1_5, NP, DIMM_SR, NP, 0, 0x00000000, 0x30332222},
  {2, DDR1866, V1_5, NP, DIMM_DR, NP, 1, 0x00000000, 0x30332222},
};
CONST PSC_TBL_ENTRY SAOTblEntUAM3 = {
   {PSCFG_SAO, UDIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_OR, AMD_F15_ALL}, OR_SOCKET_AM3, DDR3_TECHNOLOGY},
   sizeof (OrAM3UDdr3SAO) / sizeof (PSCFG_SAO_ENTRY),
   (VOID *)&OrAM3UDdr3SAO
};
// training configuratrions
// Format :
// DimmPerCh,   DDRrate,   VDDIO,   Dimm0,   Dimm1,   Dimm2,   2D
//
STATIC CONST PSCFG_S___ENTRY OrAM3UDdr3S__[] = {
  // DimmPerCh,Frequency,VDDIO,DIMM0,DIMM1,DIMM2,Enable__Training
  {1, DDR667 + DDR800 + DDR1066 + DDR1333 + DDR1600 + DDR1866, V1_5, DIMM_SR + DIMM_DR, NP, NP, 0},
  // DimmPerCh,Frequency,VDDIO,DIMM0,DIMM1,DIMM2,Enable__Training
  {2, DDR667 + DDR800 + DDR1066 + DDR1333, V1_5, NP + DIMM_SR + DIMM_DR, DIMM_SR + DIMM_DR, NP, 0},
  {2, DDR1600, V1_5, NP, DIMM_SR + DIMM_DR, NP, 0},
  {2, DDR1600, V1_5, DIMM_SR, DIMM_SR, NP, 0},
  {2, DDR1600, V1_5, DIMM_SR, DIMM_DR, NP, 0},
  {2, DDR1600, V1_5, DIMM_DR, DIMM_SR + DIMM_DR, NP, 0},
  {2, DDR1866, V1_5, NP, DIMM_SR + DIMM_DR, NP, 0},  };
CONST PSC_TBL_ENTRY S__TblEntUAM3 = {
   {PSCFG_S__, UDIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_OR, AMD_F15_ALL}, OR_SOCKET_AM3, DDR3_TECHNOLOGY},
   sizeof (OrAM3UDdr3S__) / sizeof (PSCFG_S___ENTRY),
   (VOID *)&OrAM3UDdr3S__
};
// ODT pattern for 1 DPC
// Format:
//  Dimm0,   RdODTCSHigh,   RdODTCSLow,   WrODTCSHigh,   WrODTCSLow
//
STATIC CONST PSCFG_1D_ODTPAT_ENTRY Or1UDdr3OdtPat[] = {
  {DIMM_SR, 0x00000000, 0x00000000, 0x00000000, 0x00000001},
  {DIMM_DR, 0x00000000, 0x00000000, 0x00000000, 0x00000401}
};
CONST PSC_TBL_ENTRY OdtPat1DTblEntUAM3 = {
   {PSCFG_ODT_PAT_1D, UDIMM_TYPE, _1DIMM, {AMD_FAMILY_15_OR, AMD_F15_ALL}, OR_SOCKET_AM3, DDR3_TECHNOLOGY},
   sizeof (Or1UDdr3OdtPat) / sizeof (PSCFG_1D_ODTPAT_ENTRY),
   (VOID *)&Or1UDdr3OdtPat
};

// ODT pattern for 2 DPC
// Format:
//  Dimm0,   Dimm1,   ,RdODTCSHigh,   RdODTCSLow,   WrODTCSHigh,   WrODTCSLow
//
STATIC CONST PSCFG____ODTPAT_ENTRY Or2UDdr3OdtPat[] = {
  {NP, DIMM_SR, 0x00000000, 0x00000000, 0x00000000, 0x00020000},
  {NP, DIMM_DR, 0x00000000, 0x00000000, 0x00000000, 0x08020000},
  {DIMM_SR + DIMM_DR, DIMM_SR + DIMM_DR, 0x00000000, 0x01010202, 0x00000000, 0x09030603}
};
CONST PSC_TBL_ENTRY OdtPat2DTblEntUAM3 = {
   {PSCFG_ODT_PAT___, UDIMM_TYPE, _2DIMM, {AMD_FAMILY_15_OR, AMD_F15_ALL}, OR_SOCKET_AM3, DDR3_TECHNOLOGY},
   sizeof (Or2UDdr3OdtPat) / sizeof (PSCFG____ODTPAT_ENTRY),
   (VOID *)&Or2UDdr3OdtPat
};

// ODT pattern for 3 DPC
// Format:
//  Dimm0,   Dimm1,   Dimm2,   RdODTCSHigh,   RdODTCSLow,   WrODTCSHigh,   WrODTCSLow
//
STATIC CONST PSCFG_3D_ODTPAT_ENTRY Or3UDdr3OdtPat[] = {
  {NP, NP, DIMM_SR + DIMM_DR, 0x00000000, 0x00000000, 0x00000004, 0x00000000},
  {DIMM_SR + DIMM_DR, NP, DIMM_SR + DIMM_DR, 0x00000101, 0x00000404, 0x00000105, 0x00000405}
};
CONST PSC_TBL_ENTRY OdtPat3DTblEntUAM3 = {
   {PSCFG_ODT_PAT_3D, UDIMM_TYPE, _3DIMM, {AMD_FAMILY_15_OR, AMD_F15_ALL}, OR_SOCKET_AM3, DDR3_TECHNOLOGY},
   sizeof (Or3UDdr3OdtPat) / sizeof (PSCFG_3D_ODTPAT_ENTRY),
   (VOID *)&Or3UDdr3OdtPat
};

// Dram Term and Dynamic Dram Term
// Format :
// DimmPerCh,   DDRrate,   VDDIO,   Dimm0,   Dimm1,   Dimm2,   Dimm,   Rank,   RttNom,    RttWr
//
STATIC CONST PSCFG_RTT_ENTRY DramTermOrAM3UDIMM[] = {
  {1, DDR667 + DDR800 + DDR1066, V1_5, DIMM_SR, NP, NP, DIMM_SR, R0, 2, 0},
  {1, DDR667 + DDR800 + DDR1066, V1_5, DIMM_DR, NP, NP, DIMM_DR, R0 + R1, 2, 0},
  {1, DDR1333, V1_5, DIMM_SR, NP, NP, DIMM_SR, R0, 1, 0},
  {1, DDR1333, V1_5, DIMM_DR, NP, NP, DIMM_DR, R0 + R1, 1, 0},
  {1, DDR1600 + DDR1866, V1_5, DIMM_SR, NP, NP, DIMM_SR, R0, 3, 0},
  {1, DDR1600 + DDR1866, V1_5, DIMM_DR, NP, NP, DIMM_DR, R0 + R1, 3, 0},
  {2, DDR667 + DDR800 + DDR1066, V1_5, NP, DIMM_SR, NP, DIMM_SR, R0, 2, 0},
  {2, DDR667 + DDR800 + DDR1066, V1_5, NP, DIMM_DR, NP, DIMM_DR, R0 + R1, 2, 0},
  {2, DDR667 + DDR800 + DDR1066, V1_5, DIMM_SR, DIMM_SR + DIMM_DR, NP, DIMM_SR, R0, 3, 2},
  {2, DDR667 + DDR800 + DDR1066, V1_5, DIMM_SR, DIMM_DR, NP, DIMM_DR, R0 + R1, 3, 2},
  {2, DDR667 + DDR800 + DDR1066, V1_5, DIMM_DR, DIMM_SR, NP, DIMM_SR, R0, 3, 2},
  {2, DDR667 + DDR800 + DDR1066, V1_5, DIMM_DR, DIMM_SR + DIMM_DR, NP, DIMM_DR, R0 + R1, 3, 2},
  {2, DDR1333, V1_5, NP, DIMM_SR, NP, DIMM_SR, R0, 1, 0},
  {2, DDR1333, V1_5, NP, DIMM_DR, NP, DIMM_DR, R0 + R1, 1, 0},
  {2, DDR1333, V1_5, DIMM_SR, DIMM_SR + DIMM_DR, NP, DIMM_SR, R0, 5, 2},
  {2, DDR1333, V1_5, DIMM_SR, DIMM_DR, NP, DIMM_DR, R0 + R1, 5, 2},
  {2, DDR1333, V1_5, DIMM_DR, DIMM_SR, NP, DIMM_SR, R0, 5, 2},
  {2, DDR1333, V1_5, DIMM_DR, DIMM_SR + DIMM_DR, NP, DIMM_DR, R0 + R1, 5, 2},
  {2, DDR1600, V1_5, NP, DIMM_SR, NP, DIMM_SR, R0, 3, 0},
  {2, DDR1600, V1_5, NP, DIMM_DR, NP, DIMM_DR, R0 + R1, 3, 0},
  {2, DDR1600, V1_5, DIMM_SR, DIMM_SR, NP, DIMM_SR, R0, 4, 1},
  {2, DDR1600, V1_5, DIMM_SR, DIMM_DR, NP, DIMM_SR, R0, 4, 1},
  {2, DDR1600, V1_5, DIMM_SR, DIMM_DR, NP, DIMM_DR, R0 + R1, 4, 1},
  {2, DDR1600, V1_5, DIMM_DR, DIMM_SR, NP, DIMM_SR, R0, 4, 1},
  {2, DDR1600, V1_5, DIMM_DR, DIMM_SR + DIMM_DR, NP, DIMM_DR, R0 + R1, 4, 1},
  {2, DDR1866, V1_5, NP, DIMM_SR, NP, DIMM_SR, R0, 3, 0},
  {2, DDR1866, V1_5, NP, DIMM_DR, NP, DIMM_DR, R0 + R1, 3, 0},
};
CONST PSC_TBL_ENTRY DramTermTblEntUAM3 = {
   {PSCFG_RTT, UDIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_OR, AMD_F15_ALL}, OR_SOCKET_AM3, DDR3_TECHNOLOGY},
   sizeof (DramTermOrAM3UDIMM) / sizeof (PSCFG_RTT_ENTRY),
   (VOID *)&DramTermOrAM3UDIMM
};

// Max Freq.
// Format :
// DimmPerCh,   Dimms,   SR,   DR,   QR,   Speed1_5V,   Speed1_35V,   Speed1_25V
//
STATIC CONST PSCFG_MAXFREQ_ENTRY ROMDATA MaxFreqOrAM3UDIMM[] = {
  {1, 1, 1, 0, 0, DDR1866_FREQUENCY, 0, 0},
  {1, 1, 0, 1, 0, DDR1866_FREQUENCY, 0, 0},
  {2, 1, 1, 0, 0, DDR1600_FREQUENCY, 0, 0},
  {2, 1, 0, 1, 0, DDR1600_FREQUENCY, 0, 0},
  {2, 2, 2, 0, 0, DDR1600_FREQUENCY, 0, 0},
  {2, 2, 1, 1, 0, DDR1333_FREQUENCY, 0, 0},
  {2, 2, 0, 2, 0, DDR1333_FREQUENCY, 0, 0}
};
CONST PSC_TBL_ENTRY MaxFreqTblEntUAM3 = {
   {PSCFG_MAXFREQ, UDIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_OR, AMD_F15_ALL}, OR_SOCKET_AM3, DDR3_TECHNOLOGY},
   sizeof (MaxFreqOrAM3UDIMM) / sizeof (PSCFG_MAXFREQ_ENTRY),
   (VOID *)&MaxFreqOrAM3UDIMM
};

//
// MemClkDis
//
STATIC CONST UINT8 ROMDATA OrUDdr3CLKDis[] = {0x02, 0x01, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00};
CONST PSC_TBL_ENTRY ClkDisMapEntUAM3 = {
   {PSCFG_CLKDIS, UDIMM_TYPE, _1DIMM + _2DIMM, {AMD_FAMILY_15_OR, AMD_F15_ALL}, OR_SOCKET_AM3, DDR3_TECHNOLOGY},
   sizeof (OrUDdr3CLKDis) / sizeof (UINT8),
   (VOID *)&OrUDdr3CLKDis
};

//
// WL pass1 seed
//
// Format :
// DimmPerCh in bit map,   Channel #,   Seed value
STATIC CONST PSCFG_SEED_ENTRY ROMDATA WLPas1SeedOrAM3UDIMM[] = {
  {_1DIMM + _2DIMM + _3DIMM, CH_ALL, 0x0F}
};
CONST PSC_TBL_ENTRY WLPass1SeedEntUAM3 = {
   {PSCFG_WL_PASS1_SEED, UDIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_OR, AMD_F15_ALL}, OR_SOCKET_AM3, DDR3_TECHNOLOGY},
   sizeof (WLPas1SeedOrAM3UDIMM) / sizeof (PSCFG_SEED_ENTRY),
   (VOID *)&WLPas1SeedOrAM3UDIMM
};

//
// HW RxEn pass1 seed
//
// Format :
// DimmPerCh in bit map,   Channel #,   Seed value
STATIC CONST PSCFG_SEED_ENTRY ROMDATA HWRxEnPas1SeedOrAM3UDIMM[] = {
  {_1DIMM + _2DIMM, CH_A + CH_B, 0x3A},
};
CONST PSC_TBL_ENTRY HWRxEnPass1SeedEntUAM3 = {
   {PSCFG_HWRXEN_PASS1_SEED, UDIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_OR, AMD_F15_ALL}, OR_SOCKET_AM3, DDR3_TECHNOLOGY},
   sizeof (HWRxEnPas1SeedOrAM3UDIMM) / sizeof (PSCFG_SEED_ENTRY),
   (VOID *)&HWRxEnPas1SeedOrAM3UDIMM
};
