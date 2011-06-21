/* $NoKeywords:$ */
/*
 * @file
 *
 * masda2.c
 *
 * Platform specific settings for DA DDR2 SO-dimms
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Ardk)
 * @e \$Revision: 35136 $ @e \$Date: 2010-07-16 11:29:48 +0800 (Fri, 16 Jul 2010) $
 *
 **/
/*
 *****************************************************************************
 *
 * Copyright (c) 2011, Advanced Micro Devices, Inc.
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

/* This file contains routine that add platform specific support S1g3 */


#include "AGESA.h"
#include "mport.h"
#include "PlatformMemoryConfiguration.h"
#include "ma.h"
#include "Ids.h"
#include "cpuFamRegisters.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_MEM_ARDK_DA_MASDA2_FILECODE
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

STATIC CONST UINT8 ROMDATA DASDdr2CLKDis[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
// Even chip select maps to M[B,A]_CKE[0]
// Odd chip select maps to M[B,A]_CKE[1]
STATIC CONST UINT8 ROMDATA DASDdr2CKETri[] = {0x55, 0xAA};
// Bit 0: M[B,A]0_ODT[0]
// Bit 1: M[B,A]1_ODT[0]
// Bit 2: M[B,A]0_ODT[1]
// Bit 3: M[B,A]1_ODT[1]
STATIC CONST UINT8 ROMDATA DASDdr2ODTTri[] = {0x01, 0x04, 0x02, 0x08};
// Bit 0: M[B,A]0_CS_H/L[0]
// Bit 1: M[B,A]0_CS_H/L[1]
// Bit 2: M[B,A]0_CS_H/L[2]
// Bit 3: M[B,A]0_CS_H/L[3]
STATIC CONST UINT8 ROMDATA DASDdr2CSTri[] = {0x01, 0x02, 0x04, 0x08, 0x00, 0x00, 0x00, 0x00};

/* -----------------------------------------------------------------------------*/
/**
 *
 *         This is function sets the platform specific settings for DA DDR2 SO-dimms
 *
 *
 *     @param[in,out]   *MemData           Pointer to MEM_DATA_STRUCTURE
 *     @param[in]       SocketID          Socket number
 *     @param[in,out]   *CurrentChannel       Pointer to CH_DEF_STRUCT
 *
 *     @return          AGESA_SUCCESS
 *     @return          CurrentChannel->MemClkDisMap     Points this pointer to RB MemClkDis table
 *     @return          CurrentChannel->ChipSelTriMap    Points this pointer to RB CS table
 *     @return          CurrentChannel->CKETriMap        Points this pointer to RB ODT table
 *     @return          CurrentChannel->ODTTriMap        Points this pointer to RB CKE table
 *     @return          CurrentChannel->DctEccDQSLike      Indicates the bytes that should be averaged for ECC
 *     @return          CurrentChannel->DctEccDQSScale     Indicates the scale that should be used for Averaging ECC byte
 *     @return          CurrentChannel->DctAddrTmg        Address Command Timing Settings for specified channel
 *     @return          CurrentChannel->DctOdcCtl         Drive Strength settings for specified channel
 *     @return          CurrentChannel->SlowMode             Slow Mode
 *
 *
 */

AGESA_STATUS
MemAGetPsCfgSDA2 (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  )
{
  STATIC CONST PSCFG_ENTRY PSCfg[] = {
    {DDR400_FREQUENCY, 0xFF, 0x002F2F2F, 0x10111222},
    {DDR533_FREQUENCY, 0xFF, 0x002F2F2F, 0x10111222},
    {DDR667_FREQUENCY, 0xFF, 0x002A2A2A, 0x10111222},
    {DDR800_FREQUENCY, 0xFF, 0x002A2A2A, 0x10111222},
  };

  UINT16 i;
  UINT8 Loads;
  UINT8 Ranks;
  UINT16 Speed;
  UINT32 AddrTmgCTL;
  UINT32 DctOdcCtl;
  BOOLEAN SlowMode;

  ASSERT (MemData != 0);
  ASSERT (CurrentChannel != 0);

  AddrTmgCTL = 0;
  DctOdcCtl = 0;

  if ((CurrentChannel->MCTPtr->LogicalCpuid.Family & (AMD_FAMILY_10_DA | AMD_FAMILY_10_BL)) == 0) {
    return AGESA_UNSUPPORTED;
  }
  if (CurrentChannel->TechType != DDR2_TECHNOLOGY) {
    return AGESA_UNSUPPORTED;
  }
  if (CurrentChannel->SODimmPresent != CurrentChannel->ChDimmValid) {
    return AGESA_UNSUPPORTED;
  }

  // Prepare inputs
  Loads = CurrentChannel->Loads;
  Ranks = CurrentChannel->Ranks;
  Speed = CurrentChannel->DCTPtr->Timings.Speed;
  SlowMode = FALSE;  // 1T

  for (i = 0; i < GET_SIZE_OF (PSCfg); i++) {
    if (Speed == PSCfg[i].Speed) {
      if (Loads <= PSCfg[i].Loads) {
        AddrTmgCTL = PSCfg[i].AddrTmg;
        DctOdcCtl = PSCfg[i].Odc;
        break;
      }
    }
  }
  ASSERT (i < GET_SIZE_OF (PSCfg));

  //
  // Overrides and/or exceptions
  //
  if (Loads == 16) {
    if ((Speed == DDR533_FREQUENCY) && (Ranks == 2)) {
      AddrTmgCTL = 0x002C2C2C;
    } else if ((Speed == DDR667_FREQUENCY) && (Ranks == 1)) {
      AddrTmgCTL = 0x00272727;
    } else if ((Speed == DDR667_FREQUENCY) && (Ranks == 2)) {
      AddrTmgCTL = 0x00002828;
      SlowMode = TRUE;   // 2T
    } else if ((Speed == DDR800_FREQUENCY) && (Ranks == 1)) {
      AddrTmgCTL = 0x00292929;
    } else if ((Speed == DDR800_FREQUENCY) && (Ranks == 2)) {
      AddrTmgCTL = 0x00002F2F;
      SlowMode = TRUE;   // 2T
    }
  }
  CurrentChannel->MemClkDisMap = (UINT8 *) DASDdr2CLKDis;
  CurrentChannel->CKETriMap = (UINT8 *) DASDdr2CKETri;
  CurrentChannel->ODTTriMap = (UINT8 *) DASDdr2ODTTri;
  CurrentChannel->ChipSelTriMap = (UINT8 *) DASDdr2CSTri;
  CurrentChannel->DctAddrTmg = AddrTmgCTL;
  CurrentChannel->DctOdcCtl = DctOdcCtl;
  CurrentChannel->SlowMode = SlowMode;

  return AGESA_SUCCESS;
}
