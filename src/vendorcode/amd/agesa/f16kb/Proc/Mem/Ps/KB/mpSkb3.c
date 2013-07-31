/* $NoKeywords:$ */
/**
 * @file
 *
 * mpSkb.c
 *
 * Platform specific settings for KB DDR3 SO-DIMM system
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Ps/KB)
 * @e \$Revision: 84150 $ @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 **/
/*****************************************************************************
  *
 * Copyright (c) 2008 - 2013, Advanced Micro Devices, Inc.
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
#include "OptionMemory.h"
#include "PlatformMemoryConfiguration.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_MEM_PS_KB_MPSKB3_FILECODE
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
// Slow mode, POdtOff, Address timing and Output drive compensation for SODIMM ONLY configuration
// Format :
// DimmPerCh,   DDRrate,   VDDIO,   Dimm0,   Dimm1,   Dimm2,   SlowMode,   AddTmgCtl,   ODC,  POdtOff
//
STATIC CONST PSCFG_SAO_ENTRY KBSODdr3SAO[] = {
  {_1DIMM, DDR667 + DDR800, VOLT_ALL, DIMM_SR + DIMM_DR, NP, NP, 0, 0x00000000, 0x00002222, 0},
  {_1DIMM, DDR1066, VOLT_ALL, DIMM_SR, NP, NP, 0, 0x003D3D3D, 0x10002222, 0},
  {_1DIMM, DDR1066, VOLT_ALL, DIMM_DR, NP, NP, 0, 0x00000000, 0x10002222, 0},
  {_1DIMM, DDR1333, VOLT_ALL, DIMM_SR, NP, NP, 0, 0x003D3D3D, 0x20112222, 0},
  {_1DIMM, DDR1333, VOLT_ALL, DIMM_DR, NP, NP, 0, 0x00003D3D, 0x20112222, 0},
  {_1DIMM, DDR1600, V1_5 + V1_35, DIMM_SR, NP, NP, 0, 0x003C3C3C, 0x30332222, 0},
  {_1DIMM, DDR1600, V1_5 + V1_35, DIMM_DR, NP, NP, 1, 0x00003C3C, 0x30332222, 0},
  {_2DIMM, DDR667 + DDR800, VOLT_ALL, NP, DIMM_SR + DIMM_DR, NP, 0, 0x00000000, 0x00002222, 0},
  {_2DIMM, DDR667, VOLT_ALL, DIMM_SR + DIMM_DR, DIMM_SR + DIMM_DR, NP, 1, 0x00000000, 0x10222323, 0},
  {_2DIMM, DDR800, VOLT_ALL, DIMM_SR + DIMM_DR, DIMM_SR + DIMM_DR, NP, 1, 0x00000000, 0x20222323, 0},
  {_2DIMM, DDR1066, VOLT_ALL, NP, DIMM_SR, NP, 0, 0x003D3D3D, 0x10002222, 0},
  {_2DIMM, DDR1066, VOLT_ALL, NP, DIMM_DR, NP, 0, 0x00000000, 0x10002222, 0},
  {_2DIMM, DDR1066 + DDR1333, VOLT_ALL, DIMM_SR + DIMM_DR, DIMM_SR + DIMM_DR, NP, 1, 0x00000000, 0x30222323, 0},
  {_2DIMM, DDR1333, VOLT_ALL, NP, DIMM_SR, NP, 0, 0x003D3D3D, 0x20112222, 0},
  {_2DIMM, DDR1333, VOLT_ALL, NP, DIMM_DR, NP, 0, 0x00003D3D, 0x20112222, 0},
  {_2DIMM, DDR1600, V1_5 + V1_35, NP, DIMM_SR, NP, 0, 0x003C3C3C, 0x30332222, 0},
  {_2DIMM, DDR1600, V1_5 + V1_35, NP, DIMM_DR, NP, 1, 0x00003C3C, 0x30332222, 0},
  {_2DIMM, DDR1600, V1_5, DIMM_SR + DIMM_DR, DIMM_SR + DIMM_DR, NP, 1, 0x00000000, 0x30222323, 0},
  {_2DIMM, DDR1600, V1_35, DIMM_SR, DIMM_SR, NP, 1, 0x00000000, 0x30222323, 0},
};
CONST PSC_TBL_ENTRY KBSAOTblEntSO3 = {
   {PSCFG_SAO, SODIMM_TYPE, _1DIMM + _2DIMM, {AMD_FAMILY_16_KB, AMD_F16_ALL}, PT_DONT_CARE, DDR3_TECHNOLOGY},
   sizeof (KBSODdr3SAO) / sizeof (PSCFG_SAO_ENTRY),
   (VOID *)&KBSODdr3SAO
};

// Slow mode, POdtOff, Address timing and Output drive compensation for SODIMM plus Solder-down DRAM configuration
// Format :
// DimmPerCh,   DDRrate,   VDDIO,   Dimm0,   Dimm1,   Dimm2,   SlowMode,   AddTmgCtl,   ODC,   POdtOff
//
STATIC CONST PSCFG_SAO_ENTRY KBSoDwnPlusSODIMMDdr3SAO[] = {
  {_1DIMM, DDR667 + DDR800, VOLT_ALL, NP, DIMM_SR + DIMM_DR, NP, 0, 0x00000000, 0x00002222, 0},
  {_1DIMM, DDR667, VOLT_ALL, DIMM_SR + DIMM_DR, DIMM_SR + DIMM_DR, NP, 1, 0x00000000, 0x10222323, 0},
  {_1DIMM, DDR800, VOLT_ALL, DIMM_SR + DIMM_DR, DIMM_SR + DIMM_DR, NP, 1, 0x00000000, 0x20222323, 0},
  {_1DIMM, DDR1066, VOLT_ALL, NP, DIMM_SR, NP, 0, 0x003D3D3D, 0x10002222, 0},
  {_1DIMM, DDR1066, VOLT_ALL, NP, DIMM_DR, NP, 0, 0x00000000, 0x10002222, 0},
  {_1DIMM, DDR1066, VOLT_ALL, DIMM_SR + DIMM_DR, DIMM_SR + DIMM_DR, NP, 1, 0x00000000, 0x30222323, 0},
  {_1DIMM, DDR1333, V1_5 + V1_35, NP, DIMM_SR, NP, 0, 0x003D3D3D, 0x20112222, 0},
  {_1DIMM, DDR1333, V1_5 + V1_35, NP, DIMM_DR, NP, 0, 0x00003D3D, 0x20112222, 0},
  {_1DIMM, DDR1333, V1_5, DIMM_SR + DIMM_DR, DIMM_SR + DIMM_DR, NP, 1, 0x00000000, 0x30222323, 0},
  {_1DIMM, DDR1333, V1_35, DIMM_SR, DIMM_SR, NP, 1, 0x00000000, 0x30222323, 0},
};
CONST PSC_TBL_ENTRY KBSAOTblEntSoDwnPlusSODIMM3 = {
   {PSCFG_SAO, SODWN_SODIMM_TYPE, _1DIMM, {AMD_FAMILY_16_KB, AMD_F16_ALL}, PT_DONT_CARE, DDR3_TECHNOLOGY},
   sizeof (KBSoDwnPlusSODIMMDdr3SAO) / sizeof (PSCFG_SAO_ENTRY),
   (VOID *)&KBSoDwnPlusSODIMMDdr3SAO
};

// Slow mode, POdtOff, Address timing and Output drive compensation for Solder-down DRAM ONLY configuration
// Format :
// DimmPerCh,   DDRrate,   VDDIO,   Dimm0,   Dimm1,   Dimm2,   SlowMode,   AddTmgCtl,   ODC,   POdtOff
//
STATIC CONST PSCFG_SAO_ENTRY KBSoDwnDdr3SAO[] = {
  {_DIMM_NONE, DDR667, VOLT_ALL, DIMM_SR + DIMM_DR, NP, NP, 0, 0x00000000, 0x00000000, 1},
  {_DIMM_NONE, DDR800, VOLT_ALL, DIMM_SR + DIMM_DR, NP, NP, 0, 0x00000000, 0x00000000, 0},
  {_DIMM_NONE, DDR1066, VOLT_ALL, DIMM_SR, NP, NP, 0, 0x003D3D3D, 0x10000000, 0},
  {_DIMM_NONE, DDR1066, VOLT_ALL, DIMM_DR, NP, NP, 0, 0x00000000, 0x10000000, 0},
  {_DIMM_NONE, DDR1333, VOLT_ALL, DIMM_SR, NP, NP, 0, 0x003D3D3D, 0x20000000, 0},
  {_DIMM_NONE, DDR1333, VOLT_ALL, DIMM_DR, NP, NP, 0, 0x00003D3D, 0x20110000, 0},
  {_DIMM_NONE, DDR1600, V1_5 + V1_35, DIMM_SR, NP, NP, 0, 0x003C3C3C, 0x30110000, 0},
  {_DIMM_NONE, DDR1600, V1_5 + V1_35, DIMM_DR, NP, NP, 1, 0x00003C3C, 0x30110000, 0},
};
CONST PSC_TBL_ENTRY KBSAOTblEntSoDwn3 = {
   {PSCFG_SAO, SODWN_SODIMM_TYPE, _DIMM_NONE, {AMD_FAMILY_16_KB, AMD_F16_ALL}, PT_DONT_CARE, DDR3_TECHNOLOGY},
   sizeof (KBSoDwnDdr3SAO) / sizeof (PSCFG_SAO_ENTRY),
   (VOID *)&KBSoDwnDdr3SAO
};

// Dram Term and Dynamic Dram Term for SODIMM ONLY configuration
// Format :
// DimmPerCh,   DDRrate,   VDDIO,   Dimm0,   Dimm1,   Dimm2,   Dimm,   Rank,   RttNom,    RttWr
//
// RttNom:
// 0 On die termination disabled
// 1 60ohms
// 2 120ohms
// 3 40ohms
// 4 20ohms
// 5 30ohms
// RttWr:
// 0 Dynamic termination for writes disabled.
// 1 60ohms
// 2 120ohms
STATIC CONST PSCFG_RTT_ENTRY KBDramTermSODIMM3[] = {
  {_1DIMM, DDR667 + DDR800 + DDR1066, VOLT_ALL, DIMM_SR + DIMM_DR, NP, NP, DIMM_SR + DIMM_DR, R0 + R1, 2, 0},
  {_1DIMM, DDR1333, VOLT_ALL, DIMM_SR + DIMM_DR, NP, NP, DIMM_SR + DIMM_DR, R0 + R1, 1, 0},
  {_1DIMM, DDR1600, V1_5 + V1_35, DIMM_SR + DIMM_DR, NP, NP, DIMM_SR + DIMM_DR, R0 + R1, 1, 0},
  {_1DIMM, DDR1866, V1_5, DIMM_SR + DIMM_DR, NP, NP, DIMM_SR + DIMM_DR, R0 + R1, 1, 0},
  {_2DIMM, DDR667 + DDR800 + DDR1066, VOLT_ALL, NP, DIMM_SR + DIMM_DR, NP, DIMM_SR + DIMM_DR, R0 + R1, 2, 0},
  {_2DIMM, DDR667 + DDR800, VOLT_ALL, DIMM_SR + DIMM_DR, DIMM_SR + DIMM_DR, NP, DIMM_SR + DIMM_DR, R0 + R1, 3, 2},
  {_2DIMM, DDR1066 + DDR1333, VOLT_ALL, DIMM_SR + DIMM_DR, DIMM_SR + DIMM_DR, NP, DIMM_SR + DIMM_DR, R0 + R1, 5, 2},
  {_2DIMM, DDR1333, VOLT_ALL, NP, DIMM_SR + DIMM_DR, NP, DIMM_SR + DIMM_DR, R0 + R1, 1, 0},
  {_2DIMM, DDR1600, V1_5 + V1_35, NP, DIMM_SR + DIMM_DR, NP, DIMM_SR + DIMM_DR, R0 + R1, 1, 0},
  {_2DIMM, DDR1600, V1_5, DIMM_SR + DIMM_DR, DIMM_SR + DIMM_DR, NP, DIMM_SR + DIMM_DR, R0 + R1, 4, 1},
  {_2DIMM, DDR1600, V1_35, DIMM_SR, DIMM_SR, NP, DIMM_SR + DIMM_DR, R0 + R1, 4, 1},
};
CONST PSC_TBL_ENTRY KBDramTermTblEntSO3 = {
   {PSCFG_RTT, SODIMM_TYPE, _1DIMM + _2DIMM, {AMD_FAMILY_16_KB, AMD_F16_ALL}, PT_DONT_CARE, DDR3_TECHNOLOGY},
   sizeof (KBDramTermSODIMM3) / sizeof (PSCFG_RTT_ENTRY),
   (VOID *)&KBDramTermSODIMM3
};

// Dram Term and Dynamic Dram Term for SODIMM plus Solder-down DRAM configuration
// Format :
// DimmPerCh,   DDRrate,   VDDIO,   Dimm0,   Dimm1,   Dimm2,   Dimm,   Rank,   RttNom,    RttWr
//
// RttNom:
// 0 On die termination disabled
// 1 60ohms
// 2 120ohms
// 3 40ohms
// 4 20ohms
// 5 30ohms
// RttWr:
// 0 Dynamic termination for writes disabled.
// 1 60ohms
// 2 120ohms
STATIC CONST PSCFG_RTT_ENTRY KBDramTermSoDwnPlusSODIMM3[] = {
  {_1DIMM, DDR667 + DDR800 + DDR1066, VOLT_ALL, NP, DIMM_SR + DIMM_DR, NP, DIMM_SR + DIMM_DR, R0 + R1, 2, 0},
  {_1DIMM, DDR667 + DDR800, VOLT_ALL, DIMM_SR + DIMM_DR, DIMM_SR + DIMM_DR, NP, DIMM_SR + DIMM_DR, R0 + R1, 3, 2},
  {_1DIMM, DDR1066, VOLT_ALL, DIMM_SR + DIMM_DR, DIMM_SR + DIMM_DR, NP, DIMM_SR + DIMM_DR, R0 + R1, 5, 2},
  {_1DIMM, DDR1333, V1_5 + V1_35, NP, DIMM_SR + DIMM_DR, NP, DIMM_SR + DIMM_DR, R0 + R1, 1, 0},
  {_1DIMM, DDR1333, V1_5, DIMM_SR + DIMM_DR, DIMM_SR + DIMM_DR, NP, DIMM_SR + DIMM_DR, R0 + R1, 5, 2},
  {_1DIMM, DDR1333, V1_35, DIMM_SR, DIMM_SR, NP, DIMM_SR + DIMM_DR, R0 + R1, 5, 2},
};
CONST PSC_TBL_ENTRY KBDramTermTblEntSoDwnPlusSODIMM3 = {
   {PSCFG_RTT, SODWN_SODIMM_TYPE, _1DIMM, {AMD_FAMILY_16_KB, AMD_F16_ALL}, PT_DONT_CARE, DDR3_TECHNOLOGY},
   sizeof (KBDramTermSoDwnPlusSODIMM3) / sizeof (PSCFG_RTT_ENTRY),
   (VOID *)&KBDramTermSoDwnPlusSODIMM3
};

// Dram Term and Dynamic Dram Term for Solder-down DRAM ONLY configuration
// Format :
// DimmPerCh,   DDRrate,   VDDIO,   Dimm0,   Dimm1,   Dimm2,   Dimm,   Rank,   RttNom,    RttWr
//
// RttNom:
// 0 On die termination disabled
// 1 60ohms
// 2 120ohms
// 3 40ohms
// 4 20ohms
// 5 30ohms
// RttWr:
// 0 Dynamic termination for writes disabled.
// 1 60ohms
// 2 120ohms
STATIC CONST PSCFG_RTT_ENTRY KBDramTermSoDwn3[] = {
  {_DIMM_NONE, DDR667, VOLT_ALL, DIMM_SR + DIMM_DR, NP, NP, DIMM_SR + DIMM_DR, R0 + R1, 0, 0},
  {_DIMM_NONE, DDR800 + DDR1066, VOLT_ALL, DIMM_SR + DIMM_DR, NP, NP, DIMM_SR + DIMM_DR, R0 + R1, 2, 0},
  {_DIMM_NONE, DDR1333, VOLT_ALL, DIMM_SR + DIMM_DR, NP, NP, DIMM_SR + DIMM_DR, R0 + R1, 1, 0},
  {_DIMM_NONE, DDR1600, V1_5 + V1_35, DIMM_SR + DIMM_DR, NP, NP, DIMM_SR + DIMM_DR, R0 + R1, 1, 0},
};
CONST PSC_TBL_ENTRY KBDramTermTblEntSoDwn3 = {
   {PSCFG_RTT, SODWN_SODIMM_TYPE, _DIMM_NONE, {AMD_FAMILY_16_KB, AMD_F16_ALL}, PT_DONT_CARE, DDR3_TECHNOLOGY},
   sizeof (KBDramTermSoDwn3) / sizeof (PSCFG_RTT_ENTRY),
   (VOID *)&KBDramTermSoDwn3
};

// Max Freq. for SODIMM ONLY <6-layer Motherboard Design> configuration
// Format :
// DimmPerCh,   Dimms,   SR,   DR,   QR,   Speed1_5V,   Speed1_35V,   Speed1_25V
//
STATIC CONST PSCFG_MAXFREQ_ENTRY ROMDATA KBMaxFreqSODIMM6L[] = {
  {{_1DIMM, 1, 1, 0, 0, DDR1600_FREQUENCY, DDR1600_FREQUENCY, DDR1333_FREQUENCY}},
  {{_1DIMM, 1, 0, 1, 0, DDR1600_FREQUENCY, DDR1600_FREQUENCY, DDR1333_FREQUENCY}},
  {{_2DIMM, 1, 1, 0, 0, DDR1600_FREQUENCY, DDR1600_FREQUENCY, DDR1333_FREQUENCY}},
  {{_2DIMM, 1, 0, 1, 0, DDR1600_FREQUENCY, DDR1600_FREQUENCY, DDR1333_FREQUENCY}},
  {{_2DIMM, 2, 2, 0, 0, DDR1600_FREQUENCY, DDR1600_FREQUENCY, DDR1333_FREQUENCY}},
  {{_2DIMM, 2, 1, 1, 0, DDR1600_FREQUENCY, DDR1333_FREQUENCY, DDR1333_FREQUENCY}},
  {{_2DIMM, 2, 0, 2, 0, DDR1600_FREQUENCY, DDR1333_FREQUENCY, DDR1333_FREQUENCY}},
};
CONST PSC_TBL_ENTRY KBMaxFreqTblEntSO6L = {
   {PSCFG_MAXFREQ, SODIMM_TYPE, _1DIMM + _2DIMM, {AMD_FAMILY_16_KB, AMD_F16_ALL}, PT_DONT_CARE, DDR3_TECHNOLOGY},
   sizeof (KBMaxFreqSODIMM6L) / sizeof (PSCFG_MAXFREQ_ENTRY),
   (VOID *)&KBMaxFreqSODIMM6L
};

// Max Freq. for SODIMM ONLY <4-layer Motherboard Design> configuration
// Format :
// DimmPerCh,   Dimms,   SR,   DR,   QR,   Speed1_5V,   Speed1_35V,   Speed1_25V
//
STATIC CONST PSCFG_MAXFREQ_ENTRY ROMDATA KBMaxFreqSODIMM4L[] = {
  {{_1DIMM, 1, 1, 0, 0, DDR1600_FREQUENCY, DDR1333_FREQUENCY, DDR1066_FREQUENCY}},
  {{_1DIMM, 1, 0, 1, 0, DDR1600_FREQUENCY, DDR1333_FREQUENCY, DDR1066_FREQUENCY}},
  {{_2DIMM, 1, 1, 0, 0, DDR1333_FREQUENCY, DDR1333_FREQUENCY, DDR1066_FREQUENCY}},
  {{_2DIMM, 1, 0, 1, 0, DDR1333_FREQUENCY, DDR1333_FREQUENCY, DDR1066_FREQUENCY}},
  {{_2DIMM, 2, 2, 0, 0, DDR1333_FREQUENCY, DDR1333_FREQUENCY, DDR1066_FREQUENCY}},
  {{_2DIMM, 2, 1, 1, 0, DDR1333_FREQUENCY, DDR1333_FREQUENCY, DDR1066_FREQUENCY}},
  {{_2DIMM, 2, 0, 2, 0, DDR1333_FREQUENCY, DDR1333_FREQUENCY, DDR1066_FREQUENCY}},
};
CONST PSC_TBL_ENTRY KBMaxFreqTblEntSO4L = {
   {PSCFG_MAXFREQ, SODIMM_TYPE, _1DIMM + _2DIMM, {AMD_FAMILY_16_KB, AMD_F16_ALL}, PT_DONT_CARE, DDR3_TECHNOLOGY},
   sizeof (KBMaxFreqSODIMM4L) / sizeof (PSCFG_MAXFREQ_ENTRY),
   (VOID *)&KBMaxFreqSODIMM4L
};

// Max Freq. for SODIMM plus Solder-down DRAM <6-layer Motherboard Design> configuration
// Format :
// DimmPerCh,   Dimms,   SR,   DR,   QR,   Speed1_5V,   Speed1_35V,   Speed1_25V
//
STATIC CONST PSCFG_MAXFREQ_ENTRY ROMDATA KBMaxFreqSoDwnPlusSODIMM6L[] = {
  {{_1DIMM, 1, 1, 0, 0, DDR1333_FREQUENCY, DDR1333_FREQUENCY, DDR1066_FREQUENCY}},
  {{_1DIMM, 1, 0, 1, 0, DDR1333_FREQUENCY, DDR1333_FREQUENCY, DDR1066_FREQUENCY}},
  {{_1DIMM, 2, 2, 0, 0, DDR1333_FREQUENCY, DDR1333_FREQUENCY, DDR1066_FREQUENCY}},
  {{_1DIMM, 2, 1, 1, 0, DDR1333_FREQUENCY, DDR1066_FREQUENCY, DDR1066_FREQUENCY}},
  {{_1DIMM, 2, 0, 2, 0, DDR1333_FREQUENCY, DDR1066_FREQUENCY, DDR1066_FREQUENCY}},
};
CONST PSC_TBL_ENTRY KBMaxFreqTblEntSoDwnPlusSODIMM6L = {
   {PSCFG_MAXFREQ, SODWN_SODIMM_TYPE, _1DIMM, {AMD_FAMILY_16_KB, AMD_F16_ALL}, PT_DONT_CARE, DDR3_TECHNOLOGY},
   sizeof (KBMaxFreqSoDwnPlusSODIMM6L) / sizeof (PSCFG_MAXFREQ_ENTRY),
   (VOID *)&KBMaxFreqSoDwnPlusSODIMM6L
};

// Max Freq. for SODIMM plus Solder-down DRAM <4-layer Motherboard Design> configuration
// Format :
// DimmPerCh,   Dimms,   SR,   DR,   QR,   Speed1_5V,   Speed1_35V,   Speed1_25V
//
STATIC CONST PSCFG_MAXFREQ_ENTRY ROMDATA KBMaxFreqSoDwnPlusSODIMM4L[] = {
  {{_1DIMM, 1, 1, 0, 0, DDR1066_FREQUENCY, DDR1066_FREQUENCY, DDR1066_FREQUENCY}},
  {{_1DIMM, 1, 0, 1, 0, DDR1066_FREQUENCY, DDR1066_FREQUENCY, DDR1066_FREQUENCY}},
  {{_1DIMM, 2, 2, 0, 0, DDR1066_FREQUENCY, DDR1066_FREQUENCY, DDR1066_FREQUENCY}},
  {{_1DIMM, 2, 1, 1, 0, DDR1066_FREQUENCY, DDR1066_FREQUENCY, DDR1066_FREQUENCY}},
  {{_1DIMM, 2, 0, 2, 0, DDR1066_FREQUENCY, DDR1066_FREQUENCY, DDR1066_FREQUENCY}},
};
CONST PSC_TBL_ENTRY KBMaxFreqTblEntSoDwnPlusSODIMM4L = {
   {PSCFG_MAXFREQ, SODWN_SODIMM_TYPE, _1DIMM, {AMD_FAMILY_16_KB, AMD_F16_ALL}, PT_DONT_CARE, DDR3_TECHNOLOGY},
   sizeof (KBMaxFreqSoDwnPlusSODIMM4L) / sizeof (PSCFG_MAXFREQ_ENTRY),
   (VOID *)&KBMaxFreqSoDwnPlusSODIMM4L
};

// Max Freq. for Solder-down DRAM ONLY <4-layer/6-layer Motherboard Design> configuration
// Format :
// DimmPerCh,   Dimms,   SR,   DR,   QR,   Speed1_5V,   Speed1_35V,   Speed1_25V
//
STATIC CONST PSCFG_MAXFREQ_ENTRY ROMDATA KBMaxFreqSoDwn[] = {
  {{_DIMM_NONE, 1, 1, 0, 0, DDR1600_FREQUENCY, DDR1600_FREQUENCY, DDR1333_FREQUENCY}},
  {{_DIMM_NONE, 1, 0, 1, 0, DDR1600_FREQUENCY, DDR1600_FREQUENCY, DDR1333_FREQUENCY}},
};
CONST PSC_TBL_ENTRY KBMaxFreqTblEntSoDwn = {
   {PSCFG_MAXFREQ, SODWN_SODIMM_TYPE, _DIMM_NONE, {AMD_FAMILY_16_KB, AMD_F16_ALL}, PT_DONT_CARE, DDR3_TECHNOLOGY},
   sizeof (KBMaxFreqSoDwn) / sizeof (PSCFG_MAXFREQ_ENTRY),
   (VOID *)&KBMaxFreqSoDwn
};
