/* $NoKeywords:$ */
/**
 * @file
 *
 * mpStn.c
 *
 * Platform specific settings for TN DDR3 SO-DIMM system
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Ps/TN)
 * @e \$Revision: 63425 $ @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 **/
/*****************************************************************************
  *
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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

#define FILECODE PROC_MEM_PS_TN_MPSTN3_FILECODE
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
// Slow mode, Address timing and Output drive compensation for normal SODIMM configuration
// Format :
// DimmPerCh,   DDRrate,   VDDIO,   Dimm0,   Dimm1,   Dimm2,   SlowMode,   AddTmgCtl,   ODC
//
STATIC CONST PSCFG_SAO_ENTRY TNSODdr3SAO[] = {
  {_1DIMM, DDR667 + DDR800, VOLT_ALL, DIMM_SR + DIMM_DR, NP, NP, 0, 0x00000000, 0x00002222},
  {_1DIMM, DDR1066, VOLT_ALL, DIMM_SR, NP, NP, 0, 0x003D3D3D, 0x10002222},
  {_1DIMM, DDR1066, VOLT_ALL, DIMM_DR, NP, NP, 0, 0x00000000, 0x10002222},
  {_1DIMM, DDR1333, VOLT_ALL, DIMM_SR, NP, NP, 0, 0x003D3D3D, 0x20002222},
  {_1DIMM, DDR1333, VOLT_ALL, DIMM_DR, NP, NP, 0, 0x00003D3D, 0x20002222},
  {_1DIMM, DDR1600, V1_5 + V1_35, DIMM_SR, NP, NP, 0, 0x003C3C3C, 0x30112222},
  {_1DIMM, DDR1600, V1_5 + V1_35, DIMM_DR, NP, NP, 1, 0x00003C3C, 0x30112222},
  {_1DIMM, DDR1866, V1_5, DIMM_SR, NP, NP, 0, 0x003C3C3C, 0x30112222},
  {_1DIMM, DDR1866, V1_5, DIMM_DR, NP, NP, 1, 0x00003C3C, 0x30112222},
  {_2DIMM, DDR667 + DDR800, VOLT_ALL, NP, DIMM_SR + DIMM_DR, NP, 0, 0x00000000, 0x00002222},
  {_2DIMM, DDR667, VOLT_ALL, DIMM_SR + DIMM_DR, DIMM_SR + DIMM_DR, NP, 1, 0x00000039, 0x10222323},
  {_2DIMM, DDR800, VOLT_ALL, DIMM_SR + DIMM_DR, DIMM_SR + DIMM_DR, NP, 1, 0x00000039, 0x20222323},
  {_2DIMM, DDR1066, VOLT_ALL, NP, DIMM_SR, NP, 0, 0x003D3D3D, 0x10002222},
  {_2DIMM, DDR1066, VOLT_ALL, NP, DIMM_DR, NP, 0, 0x00000000, 0x10002222},
  {_2DIMM, DDR1066, VOLT_ALL, DIMM_SR + DIMM_DR, DIMM_SR + DIMM_DR, NP, 1, 0x00000037, 0x30222323},
  {_2DIMM, DDR1333, VOLT_ALL, NP, DIMM_SR, NP, 0, 0x003D3D3D, 0x20002222},
  {_2DIMM, DDR1333, VOLT_ALL, NP, DIMM_DR, NP, 0, 0x00003D3D, 0x20002222},
  {_2DIMM, DDR1333, VOLT_ALL, DIMM_SR, DIMM_SR, NP, 1, 0x00000035, 0x30222323},
  {_2DIMM, DDR1333, V1_5 + V1_35, DIMM_SR + DIMM_DR, DIMM_SR + DIMM_DR, NP, 1, 0x00000035, 0x30222323},
  {_2DIMM, DDR1600, V1_5 + V1_35, NP, DIMM_SR, NP, 0, 0x003C3C3C, 0x30112222},
  {_2DIMM, DDR1600, V1_5 + V1_35, NP, DIMM_DR, NP, 1, 0x00003C3C, 0x30112222},
};
CONST PSC_TBL_ENTRY TNSAOTblEntSO3 = {
   {PSCFG_SAO, SODIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_TN, (AMD_F15_OR_ALL | AMD_F15_TN_ALL | 0x0000000000100000ull)  }, PT_DONT_CARE, DDR3_TECHNOLOGY},
   sizeof (TNSODdr3SAO) / sizeof (PSCFG_SAO_ENTRY),
   (VOID *)&TNSODdr3SAO
};

// Dram Term and Dynamic Dram Term for normal SODIMM configuration
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
STATIC CONST PSCFG_RTT_ENTRY DramTermTNSODIMM[] = {
  {_1DIMM, DDR667 + DDR800 + DDR1066, VOLT_ALL, DIMM_SR + DIMM_DR, NP, NP, DIMM_SR + DIMM_DR, R0 + R1, 2, 0},
  {_1DIMM, DDR1333, VOLT_ALL, DIMM_SR + DIMM_DR, NP, NP, DIMM_SR + DIMM_DR, R0 + R1, 1, 0},
  {_1DIMM, DDR1600, V1_5 + V1_35, DIMM_SR + DIMM_DR, NP, NP, DIMM_SR + DIMM_DR, R0 + R1, 3, 0},
  {_1DIMM, DDR1866, V1_5, DIMM_SR + DIMM_DR, NP, NP, DIMM_SR + DIMM_DR, R0 + R1, 3, 0},
  {_2DIMM, DDR667, VOLT_ALL, NP, DIMM_SR + DIMM_DR, NP, DIMM_SR + DIMM_DR, R0 + R1, 2, 0},
  {_2DIMM, DDR667, VOLT_ALL, DIMM_SR + DIMM_DR, DIMM_SR + DIMM_DR, NP, DIMM_SR + DIMM_DR, R0 + R1, 3, 2},
  {_2DIMM, DDR800, VOLT_ALL, NP, DIMM_SR + DIMM_DR, NP, DIMM_SR + DIMM_DR, R0 + R1, 2, 0},
  {_2DIMM, DDR800, VOLT_ALL, DIMM_SR + DIMM_DR, DIMM_SR + DIMM_DR, NP, DIMM_SR + DIMM_DR, R0 + R1, 3, 2},
  {_2DIMM, DDR1066, VOLT_ALL, NP, DIMM_SR + DIMM_DR, NP, DIMM_SR + DIMM_DR, R0 + R1, 2, 0},
  {_2DIMM, DDR1066, VOLT_ALL, DIMM_SR + DIMM_DR, DIMM_SR + DIMM_DR, NP, DIMM_SR + DIMM_DR, R0 + R1, 5, 2},
  {_2DIMM, DDR1333, VOLT_ALL, NP, DIMM_SR + DIMM_DR, NP, DIMM_SR + DIMM_DR, R0 + R1, 1, 0},
  {_2DIMM, DDR1333, VOLT_ALL, DIMM_SR, DIMM_SR, NP, DIMM_SR, R0, 5, 2},
  {_2DIMM, DDR1333, V1_5 + V1_35, DIMM_SR + DIMM_DR, DIMM_SR + DIMM_DR, NP, DIMM_SR + DIMM_DR, R0 + R1, 5, 2},
  {_2DIMM, DDR1600, V1_5 + V1_35, NP, DIMM_SR + DIMM_DR, NP, DIMM_SR + DIMM_DR, R0 + R1, 3, 0},
  {_2DIMM, DDR1866, V1_5, NP, DIMM_SR + DIMM_DR, NP, DIMM_SR + DIMM_DR, R0 + R1, 3, 0}
};
CONST PSC_TBL_ENTRY TNDramTermTblEntSO = {
   {PSCFG_RTT, SODIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_TN, (AMD_F15_OR_ALL | AMD_F15_TN_ALL | 0x0000000000100000ull)  }, PT_DONT_CARE, DDR3_TECHNOLOGY},
   sizeof (DramTermTNSODIMM) / sizeof (PSCFG_RTT_ENTRY),
   (VOID *)&DramTermTNSODIMM
};

// Max Freq. for normal SODIMM configuration
// Format :
// DimmPerCh,   Dimms,   SR,   DR,   QR,   Speed1_5V,   Speed1_35V,   Speed1_25V
//
STATIC CONST PSCFG_MAXFREQ_ENTRY ROMDATA MaxFreqTNSODIMM[] = {
  {{_1DIMM, 1, 1, 0, 0, DDR1866_FREQUENCY, DDR1600_FREQUENCY, DDR1333_FREQUENCY}},
  {{_1DIMM, 1, 0, 1, 0, DDR1866_FREQUENCY, DDR1600_FREQUENCY, DDR1333_FREQUENCY}},
  {{_2DIMM, 1, 1, 0, 0, DDR1600_FREQUENCY, DDR1600_FREQUENCY, DDR1333_FREQUENCY}},
  {{_2DIMM, 1, 0, 1, 0, DDR1600_FREQUENCY, DDR1600_FREQUENCY, DDR1333_FREQUENCY}},
  {{_2DIMM, 2, 2, 0, 0, DDR1600_FREQUENCY, DDR1333_FREQUENCY, DDR1333_FREQUENCY}},
  {{_2DIMM, 2, 1, 1, 0, DDR1333_FREQUENCY, DDR1333_FREQUENCY, DDR1066_FREQUENCY}},
  {{_2DIMM, 2, 0, 2, 0, DDR1333_FREQUENCY, DDR1333_FREQUENCY, DDR1066_FREQUENCY}}
};
CONST PSC_TBL_ENTRY TNMaxFreqTblEntSO = {
   {PSCFG_MAXFREQ, SODIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_TN, (AMD_F15_OR_ALL | AMD_F15_TN_ALL | 0x0000000000100000ull)  }, PT_DONT_CARE, DDR3_TECHNOLOGY},
   sizeof (MaxFreqTNSODIMM) / sizeof (PSCFG_MAXFREQ_ENTRY),
   (VOID *)&MaxFreqTNSODIMM
};


