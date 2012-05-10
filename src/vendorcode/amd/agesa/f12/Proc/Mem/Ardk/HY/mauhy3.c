/* $NoKeywords:$ */
/**
 * @file
 *
 * mauhy3.c
 *
 * Platform specific settings for HY DDR3 unbuffered dimms
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Ardk)
 * @e \$Revision: 44324 $ @e \$Date: 2010-12-22 17:16:51 +0800 (Wed, 22 Dec 2010) $
 *
 **/
/*****************************************************************************
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
  * ***************************************************************************
  *
 */


/* This file contains routine that add platform specific support AM3 */


#include "AGESA.h"
#include "mport.h"
#include "ma.h"
#include "Ids.h"
#include "cpuFamRegisters.h"
#include "OptionMemory.h"
#include "PlatformMemoryConfiguration.h"
#include "mu.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_MEM_ARDK_HY_MAUHY3_FILECODE
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

STATIC CONST UINT8 ROMDATA HyUDdr3CLKDis[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
// Even chip select maps to M[B,A]_CKE[0]
// Odd chip select maps to M[B,A]_CKE[1]
STATIC CONST UINT8 ROMDATA HyUDdr3CKETri[] = {0x55, 0xAA};
// Bit 0: M[B,A]0_ODT[0]
// Bit 1: M[B,A]1_ODT[0]
// Bit 2: M[B,A]0_ODT[1]
// Bit 3: M[B,A]1_ODT[1]
STATIC CONST UINT8 ROMDATA HyUDdr3ODTTri2D[] = {0x01, 0x04, 0x02, 0x08};
// 3 dimms per channel
// Dimm 0: BP_MEMODTx[0]
// Dimm 1: BP_MEMODTx[3,1]
// Dimm 2: BP_MEMODTx[2]
STATIC CONST UINT8 ROMDATA HyUDdr3ODTTri3D[] = {0xFF, 0xFF, 0xFF, 0xFF};
// Bit 0: M[B,A]0_CS_H/L[0]
// Bit 1: M[B,A]0_CS_H/L[1]
// Bit 2: M[B,A]0_CS_H/L[2]
// Bit 3: M[B,A]0_CS_H/L[3]
STATIC CONST UINT8 ROMDATA HyUDdr3CSTri[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

/* -----------------------------------------------------------------------------*/
/**
 *
 *         This is function sets the platform specific settings for HY DDR3 unbuffered dimms
 *
 *
 *     @param[in,out]   *MemData           Pointer to MEM_DATA_STRUCTURE
 *     @param[in]       SocketID          Socket number
 *     @param[in,out]   *CurrentChannel       Pointer to CH_DEF_STRUCT
 *
 *     @return          AGESA_SUCCESS
 *     @return          CurrentChannel->MemClkDisMap     Points this pointer to HY MemClkDis table
 *     @return          CurrentChannel->ChipSelTriMap    Points this pointer to HY CS table
 *     @return          CurrentChannel->CKETriMap        Points this pointer to HY ODT table
 *     @return          CurrentChannel->ODTTriMap        Points this pointer to HY CKE table
 *     @return          CurrentChannel->DctEccDQSLike      Indicates the bytes that should be averaged for ECC
 *     @return          CurrentChannel->DctEccDQSScale     Indicates the scale that should be used for Averaging ECC byte
 *     @return          CurrentChannel->DctAddrTmg        Address Command Timing Settings for specified channel
 *     @return          CurrentChannel->DctOdcCtl         Drive Strength settings for specified channel
 *     @return          CurrentChannel->SlowMode             Slow Mode
 *
 *
 */

AGESA_STATUS
MemAGetPsCfgUHy3 (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  )
{
  STATIC CONST PSCFG_ENTRY PSCfg[] = {
    {DDR800_FREQUENCY, 0xFF, 0x00390039, 0x20223323},
    {DDR1066_FREQUENCY, 0xFF, 0x00350037, 0x20223323},
    {DDR1333_FREQUENCY, 0xFF, 0x00000035, 0x20223323},
    {DDR1600_FREQUENCY, 0xFF, 0x00000033, 0x20223323}
  };

  STATIC CONST ADV_PSCFG_ODT_ENTRY PSCfg2DIMMsODT[] = {
    {SR_DIMM0, 0x00000000, 0x00000000, 0x00000001, 0x00000000, 1},
    {DR_DIMM0, 0x00000000, 0x00000000, 0x00000104, 0x00000000, 1},
    {SR_DIMM1, 0x00000000,0x00000000,0x00020000, 0x00000000, 1},
    {DR_DIMM1, 0x00000000,0x00000000,0x02080000, 0x00000000, 1},
    {SR_DIMM0 + DR_DIMM0 + SR_DIMM1 + DR_DIMM1, 0x01010202,0x00000000,0x09030603, 0x00000000, 2},
  };

  STATIC CONST ADV_R_PSCFG_WL_ODT_ENTRY PSCfg2DIMMsWlODT[] = {
    {SR_DIMM0,            {0x01, 0x00, 0x00, 0x00}, 1},
    {DR_DIMM0,            {0x04, 0x00, 0x00, 0x00}, 1},
    {SR_DIMM1,            {0x00, 0x02, 0x00, 0x00}, 1},
    {DR_DIMM1,            {0x00, 0x08, 0x00, 0x00}, 1},
    {SR_DIMM0 + DR_DIMM0 + SR_DIMM1 + DR_DIMM1, {0x03, 0x03, 0x00, 0x00}, 2}
  };

  STATIC CONST ADV_PSCFG_ODT_ENTRY PSCfg3DIMMsODT[] = {
    {SR_DIMM2 + DR_DIMM2, 0x00000000, 0x00000000, 0x00000000, 0x00000404, 1},
    //{SR_DIMM0 + DR_DIMM0, 0x00000000, 0x00000000, 0x00000101, 0x00000000, 1},
    {SR_DIMM0 + DR_DIMM0 + SR_DIMM2 + DR_DIMM2, 0x00000404, 0x00000101, 0x00000405, 0x00000105, 2},
    //{SR_DIMM0 + DR_DIMM0 + SR_DIMM1 + DR_DIMM1 + SR_DIMM2 + DR_DIMM2, 0x05050606, 0x00000303, 0x0D070607, 0x00000307, 3},
  };

  STATIC CONST ADV_R_PSCFG_WL_ODT_ENTRY PSCfg3DIMMsWlODT[] = {
    {SR_DIMM2 + DR_DIMM2, {0x00, 0x00, 0x04, 0x00}, 1},
    //{SR_DIMM0 + DR_DIMM0, {0x01, 0x02, 0x00, 0x00}, 1},
    {SR_DIMM0 + DR_DIMM0 + SR_DIMM2 + DR_DIMM2, {0x05, 0x00, 0x05, 0x00}, 2},
    //{SR_DIMM0 + DR_DIMM0 + SR_DIMM1 + DR_DIMM1 + SR_DIMM2 + DR_DIMM2, {0x07, 0x07, 0x07, 0x00}, 3},
  };

  UINT16 i;
  UINT16 j;
  UINT8  MaxDimmPerCH;
  UINT8 Loads;
  UINT8 Dimms;
  UINT16 Speed;
  UINT16 DIMMRankType;
  UINT16 _DIMMRankType;
  UINT32 AddrTmgCTL;
  UINT32 DctOdcCtl;
  UINT32 PhyRODTCSLow;
  UINT32 PhyRODTCSHigh;
  UINT32 PhyWODTCSLow;
  UINT32 PhyWODTCSHigh;
  UINT8 PhyWLODT[4];
  UINT8 PSCfgODTSize;
  UINT8 PSCfgWlODTSize;
  BOOLEAN SlowMode;
  UINT8 DimmTpMatch;
  CONST ADV_PSCFG_ODT_ENTRY *PSCfgODTPtr;
  CONST ADV_R_PSCFG_WL_ODT_ENTRY *PSCfgWlODTPtr;
  UINT8 *DimmsPerChPtr;

  ASSERT (MemData != NULL);
  ASSERT (CurrentChannel != NULL);

  AddrTmgCTL = 0;
  DctOdcCtl = 0;
  PhyRODTCSLow = 0;
  PhyRODTCSHigh = 0;
  PhyWODTCSLow = 0;
  PhyWODTCSHigh = 0;
  PhyWLODT[0] = 0x0F;
  PhyWLODT[1] = 0x0F;
  PhyWLODT[2] = 0x0F;
  PhyWLODT[3] = 0x0F;

  if ((CurrentChannel->MCTPtr->LogicalCpuid.Family & AMD_FAMILY_10_HY) == 0) {
    return AGESA_UNSUPPORTED;
  }
  if (CurrentChannel->TechType != DDR3_TECHNOLOGY) {
    return AGESA_UNSUPPORTED;
  }
  if (CurrentChannel->RegDimmPresent) {
    return AGESA_UNSUPPORTED;
  }
  // Prepare inputs

  DimmsPerChPtr = FindPSOverrideEntry (MemData->ParameterListPtr->PlatformMemoryConfiguration, PSO_MAX_DIMMS, SocketID, CurrentChannel->ChannelID);
  if (DimmsPerChPtr != NULL) {
    MaxDimmPerCH = *DimmsPerChPtr;
  } else {
    MaxDimmPerCH = 2;
  }

  Loads = CurrentChannel->Loads;
  Dimms = CurrentChannel->Dimms;
  Speed = CurrentChannel->DCTPtr->Timings.Speed;

  DIMMRankType = MemAGetPsRankType (CurrentChannel);

  if ((Speed == DDR1333_FREQUENCY || Speed == DDR1600_FREQUENCY) && (Dimms == 2)) {
    SlowMode = TRUE;   // 2T
  } else {
    SlowMode = FALSE;  // 1T
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

  ASSERT (i < GET_SIZE_OF (PSCfg));

  if (MaxDimmPerCH == 3) {
    PSCfgODTPtr = PSCfg3DIMMsODT;
    PSCfgWlODTPtr = PSCfg3DIMMsWlODT;
    PSCfgODTSize = sizeof (PSCfg3DIMMsODT) / sizeof (ADV_PSCFG_ODT_ENTRY);
    PSCfgWlODTSize = sizeof (PSCfg3DIMMsWlODT) / sizeof (ADV_R_PSCFG_WL_ODT_ENTRY);
  } else {
    PSCfgODTPtr = PSCfg2DIMMsODT;
    PSCfgWlODTPtr = PSCfg2DIMMsWlODT;
    PSCfgODTSize = sizeof (PSCfg2DIMMsODT) / sizeof (ADV_PSCFG_ODT_ENTRY);
    PSCfgWlODTSize = sizeof (PSCfg2DIMMsWlODT) / sizeof (ADV_R_PSCFG_WL_ODT_ENTRY);
  }

  // Programmable ODT
  for (i = 0; i  < PSCfgODTSize; i++, PSCfgODTPtr++) {
    if (Dimms != PSCfgODTPtr->Dimms) {
      continue;
    }
    DimmTpMatch = 0;
    _DIMMRankType = DIMMRankType & PSCfgODTPtr->DIMMRankType;
    for (j = 0; j < MAX_DIMMS_PER_CHANNEL; j++) {
      if ((_DIMMRankType & (UINT16) 0x0F << (j << 2)) != 0) {
        DimmTpMatch++;
      }
    }
    if (DimmTpMatch == PSCfgODTPtr->Dimms) {
      PhyRODTCSLow = PSCfgODTPtr->PhyRODTCSLow;
      PhyRODTCSHigh = PSCfgODTPtr->PhyRODTCSHigh;
      PhyWODTCSLow = PSCfgODTPtr->PhyWODTCSLow;
      PhyWODTCSHigh = PSCfgODTPtr->PhyWODTCSHigh;
      break;
    }
  }

  // WL ODT
  for (i = 0; i  < PSCfgWlODTSize; i++, PSCfgWlODTPtr++) {
    if (Dimms != PSCfgWlODTPtr->Dimms) {
      continue;
    }
    DimmTpMatch = 0;
    _DIMMRankType = DIMMRankType & PSCfgWlODTPtr->DIMMRankType;
    for (j = 0; j < MAX_DIMMS_PER_CHANNEL; j++) {
      if ((_DIMMRankType & (UINT16) 0x0F << (j << 2)) != 0) {
        DimmTpMatch++;
      }
    }
    if (DimmTpMatch == PSCfgWlODTPtr->Dimms) {
      PhyWLODT[0] = PSCfgWlODTPtr->PhyWrLvOdt[0];
      PhyWLODT[1] = PSCfgWlODTPtr->PhyWrLvOdt[1];
      PhyWLODT[2] = PSCfgWlODTPtr->PhyWrLvOdt[2];
      PhyWLODT[3] = PSCfgWlODTPtr->PhyWrLvOdt[3];
      break;
    }
  }

  //
  // Overrides and/or exceptions
  //
  if (Dimms == 1) {
    if (Loads >= 16) {
      if (Speed == DDR800_FREQUENCY) {
        AddrTmgCTL = 0x003B0000;
      } else if (Speed == DDR1066_FREQUENCY) {
        AddrTmgCTL = 0x00380000;
      } else if (Speed == DDR1333_FREQUENCY) {
        AddrTmgCTL = 0x00360000;
      } else {
        AddrTmgCTL = 0x00340000;
        SlowMode = TRUE;
      }
    } else {
      AddrTmgCTL = 0;
    }
    DctOdcCtl = 0x20113222;
  }

  CurrentChannel->MemClkDisMap = (UINT8 *) HyUDdr3CLKDis;
  CurrentChannel->CKETriMap = (UINT8 *) HyUDdr3CKETri;
  CurrentChannel->ChipSelTriMap = (UINT8 *) HyUDdr3CSTri;

  switch (MaxDimmPerCH) {
  case 3:
    CurrentChannel->ODTTriMap = (UINT8 *) HyUDdr3ODTTri3D;
    break;
  default:
    CurrentChannel->ODTTriMap = (UINT8 *) HyUDdr3ODTTri2D;  // Most conservative
  }

  CurrentChannel->DctEccDqsLike = 0x0403;
  CurrentChannel->DctEccDqsScale = 0x70;
  CurrentChannel->DctAddrTmg = AddrTmgCTL;
  CurrentChannel->DctOdcCtl = DctOdcCtl;
  CurrentChannel->PhyRODTCSLow  = PhyRODTCSLow;
  CurrentChannel->PhyRODTCSHigh = PhyRODTCSHigh;
  CurrentChannel->PhyWODTCSLow  = PhyWODTCSLow;
  CurrentChannel->PhyWODTCSHigh = PhyWODTCSHigh;
  for (i = 0; i < sizeof (CurrentChannel->PhyWLODT); i++) {
    CurrentChannel->PhyWLODT[i] = PhyWLODT[i];
  }
  CurrentChannel->SlowMode = SlowMode;

  return AGESA_SUCCESS;
}
