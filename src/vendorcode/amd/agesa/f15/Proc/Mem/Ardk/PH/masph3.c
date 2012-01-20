/* $NoKeywords:$ */
/*
 * @file
 *
 * masph3.c
 *
 * Platform specific settings for PH DDR3 SO-dimms
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Ardk/PH)
 * @e \$Revision: 52286 $ @e \$Date: 2011-05-04 03:48:21 -0600 (Wed, 04 May 2011) $
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

/* This file contains routine that add platform specific support S1g4 */


#include "AGESA.h"
#include "mport.h"
#include "ma.h"
#include "OptionMemory.h"
#include "PlatformMemoryConfiguration.h"
#include "mu.h"
#include "Ids.h"
#include "cpuFamRegisters.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_MEM_ARDK_PH_MASPH3_FILECODE
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

STATIC CONST UINT8 ROMDATA PhSDdr3CLKDis[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
// Even chip select maps to M[B,A]_CKE[0]
// Odd chip select maps to M[B,A]_CKE[1]
STATIC CONST UINT8 ROMDATA PhSDdr3CKETri[] = {0x55, 0xAA};
// Bit 0: M[B,A]0_ODT[0]
// Bit 1: M[B,A]1_ODT[0]
// Bit 2: M[B,A]0_ODT[1]
// Bit 3: M[B,A]1_ODT[1]
STATIC CONST UINT8 ROMDATA PhSDdr3ODTTri[] = {0x01, 0x04, 0x02, 0x08};
// Bit 0: M[B,A]0_CS_H/L[0]
// Bit 1: M[B,A]0_CS_H/L[1]
// Bit 2: M[B,A]0_CS_H/L[2]
// Bit 3: M[B,A]0_CS_H/L[3]
STATIC CONST UINT8 ROMDATA PhSDdr3CSTri[] = {0x01, 0x02, 0x04, 0x08, 0x00, 0x00, 0x00, 0x00};

/* -----------------------------------------------------------------------------*/
/**
 *
 *         This is function sets the platform specific settings for PH DDR3 SO-dimms
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
MemAGetPsCfgSPh3 (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  )
{
  STATIC CONST PSCFG_ENTRY PSCfg[] = {
    {DDR800_FREQUENCY, 0xFF, 0x00000000, 0x00113222},
    {DDR1066_FREQUENCY, 0xFF, 0x00000000, 0x10113222},
    {DDR1333_FREQUENCY, 0xFF, 0x00000000, 0x20113222},
  };

  STATIC CONST ADV_R_PSCFG_WL_ODT_ENTRY PSCfgDIMMWlODT[] = {
    {SR_DIMM1,            {0x00, 0x02, 0x00, 0x00}, 1},
    {DR_DIMM1,            {0x00, 0x08, 0x00, 0x00}, 1},
    {SR_DIMM0 + DR_DIMM0 + SR_DIMM1 + DR_DIMM1, {0x03, 0x03, 0x00, 0x00}, 2}
  };

  UINT16 i;
  UINT16 j;
  UINT8 Loads;
  UINT8 Dimms;
  UINT16 Speed;
  UINT32 AddrTmgCTL;
  UINT32 DctOdcCtl;
  UINT8 PhyWLODT[4];
  BOOLEAN SlowMode;
  UINT8  MaxDimmPerCH;
  UINT8 *DimmsPerChPtr;
  UINT16 DIMMRankType;
  UINT16 _DIMMRankType;
  UINT8 DimmTpMatch;

  ASSERT (MemData != 0);
  ASSERT (CurrentChannel != 0);

  AddrTmgCTL = 0;
  DctOdcCtl = 0;
  PhyWLODT[0] = 0x0F;
  PhyWLODT[1] = 0x0F;
  PhyWLODT[2] = 0x0F;
  PhyWLODT[3] = 0x0F;
  SlowMode = FALSE;  // 1T
  if ((CurrentChannel->MCTPtr->LogicalCpuid.Family & AMD_FAMILY_10_PH) == 0) {
    return AGESA_UNSUPPORTED;
  }
  if (CurrentChannel->TechType != DDR3_TECHNOLOGY) {
    return AGESA_UNSUPPORTED;
  }
  if (CurrentChannel->SODimmPresent != CurrentChannel->ChDimmValid) {
    return AGESA_UNSUPPORTED;
  }
  // Prepare inputs
  Loads = CurrentChannel->Loads;
  Dimms = CurrentChannel->Dimms;
  Speed = CurrentChannel->DCTPtr->Timings.Speed;

  DIMMRankType = MemAGetPsRankType (CurrentChannel);

  DimmsPerChPtr = FindPSOverrideEntry (MemData->ParameterListPtr->PlatformMemoryConfiguration, PSO_MAX_DIMMS, SocketID, CurrentChannel->ChannelID, 0, NULL, NULL);
  if (DimmsPerChPtr != NULL) {
    MaxDimmPerCH = *DimmsPerChPtr;
  } else {
    MaxDimmPerCH = 2;
  }

  for (i = 0; i < GET_SIZE_OF (PSCfg); i++) {
    if (Speed == PSCfg[i].Speed) {
      if (Loads <= PSCfg[i].Loads) {
        AddrTmgCTL = PSCfg[i].AddrTmg;
        DctOdcCtl = PSCfg[i].Odc;
        break;
      }
    }
  }

  // WL ODT
  for (i = 0; i < GET_SIZE_OF (PSCfgDIMMWlODT); i++) {
    if (Dimms != PSCfgDIMMWlODT[i].Dimms) {
      continue;
    }
    DimmTpMatch = 0;
    _DIMMRankType = DIMMRankType & PSCfgDIMMWlODT[i].DIMMRankType;
    for (j = 0; j < MAX_DIMMS_PER_CHANNEL; j++) {
      if ((_DIMMRankType & (UINT16) 0x0F << (j << 2)) != 0) {
        DimmTpMatch++;
      }
    }
    if (DimmTpMatch == PSCfgDIMMWlODT[i].Dimms) {
      PhyWLODT[0] = PSCfgDIMMWlODT[i].PhyWrLvOdt[0];
      PhyWLODT[1] = PSCfgDIMMWlODT[i].PhyWrLvOdt[1];
      PhyWLODT[2] = PSCfgDIMMWlODT[i].PhyWrLvOdt[2];
      PhyWLODT[3] = PSCfgDIMMWlODT[i].PhyWrLvOdt[3];
      break;
    }
  }

  //
  // Overrides and/or exceptions
  //
  if (MaxDimmPerCH == 2) {
    if (Dimms == 2) {
      DctOdcCtl = 0x20223323;
      SlowMode = TRUE;
      if (Speed == DDR800_FREQUENCY) {
        AddrTmgCTL = 0x00000039;
      } else if (Speed == DDR1066_FREQUENCY) {
        AddrTmgCTL = 0x00000037;
      }
    } else {
      DctOdcCtl = 0x20113222;
    }
  } else {
    if (CurrentChannel->DimmSRPresent != 0) {
      PhyWLODT[0] = 1;
    } else if (CurrentChannel->DimmDrPresent != 0) {
      PhyWLODT[0] = 4;
    }
  }

  CurrentChannel->MemClkDisMap = (UINT8 *) PhSDdr3CLKDis;
  CurrentChannel->CKETriMap = (UINT8 *) PhSDdr3CKETri;
  CurrentChannel->ODTTriMap = (UINT8 *) PhSDdr3ODTTri;
  CurrentChannel->ChipSelTriMap = (UINT8 *) PhSDdr3CSTri;

  CurrentChannel->DctAddrTmg = AddrTmgCTL;
  CurrentChannel->DctOdcCtl = DctOdcCtl;
  for (i = 0; i < sizeof (CurrentChannel->PhyWLODT); i++) {
    CurrentChannel->PhyWLODT[i] = PhyWLODT[i];
  }
  CurrentChannel->SlowMode = SlowMode;

  return AGESA_SUCCESS;
}
