/*
 * @file
 *
 * mauNi3.c
 *
 * Platform specific settings for Ni DDR3 unbuffered dimms
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Ardk)
 * @e \$Revision: 44323 $ @e \$Date: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
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
  * 
  * ***************************************************************************
  *
 */

/* This file contains routine that add platform specific support AM3 */


#include "AGESA.h"
#include "mport.h"
#include "PlatformMemoryConfiguration.h"
#include "ma.h"
#include "Ids.h"
#include "cpuFamRegisters.h"
#include "Filecode.h"
#define FILECODE PROC_MEM_ARDK_NI_MAUNI3_FILECODE
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

STATIC CONST UINT8 ROMDATA NiUDdr3CLKDis[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
// Even chip select maps to M[B,A]_CKE[0]
// Odd chip select maps to M[B,A]_CKE[1]
STATIC CONST UINT8 ROMDATA NiUDdr3CKETri[] = {0x55, 0xAA};
// Bit 0: M[B,A]0_ODT[0]
// Bit 1: M[B,A]1_ODT[0]
// Bit 2: M[B,A]0_ODT[1]
// Bit 3: M[B,A]1_ODT[1]
STATIC CONST UINT8 ROMDATA NiUDdr3ODTTri[] = {0x01, 0x04, 0x02, 0x08};
// Bit 0: M[B,A]0_CS_H/L[0]
// Bit 1: M[B,A]0_CS_H/L[1]
// Bit 2: M[B,A]0_CS_H/L[2]
// Bit 3: M[B,A]0_CS_H/L[3]
STATIC CONST UINT8 ROMDATA NiUDdr3CSTri[] = {0x01, 0x02, 0x04, 0x08, 0x00, 0x00, 0x00, 0x00};

/* -----------------------------------------------------------------------------*/
/**
 *
 *         This is function sets the platform specific settings for Ni DDR3 Unbuffered dimms
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
 */

AGESA_STATUS
MemAGetPsCfgUNi3 (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  )
{
  STATIC CONST PSCFG_ENTRY PSCfg1Dimm[] = {
    {DDR800_FREQUENCY, 0x10, 0x003B0000, 0x20113222},
    {DDR1066_FREQUENCY, 0x10, 0x00380000, 0x20113222},
    {DDR1333_FREQUENCY, 0x10, 0x00360000, 0x20113222},
    {DDR1600_FREQUENCY, 0x10, 0x00340000, 0x20113222}
  };
  STATIC CONST PSCFG_ENTRY PSCfg2Dimm[] = {
    {DDR800_FREQUENCY, 0xFF, 0x00390039, 0x20223323},
    {DDR1066_FREQUENCY, 0xFF, 0x00350037, 0x20223323},
    {DDR1333_FREQUENCY, 0xFF, 0x00000035, 0x20223323},
    {DDR1600_FREQUENCY, 0xFF, 0x00000033, 0x20223323}
  };

  STATIC CONST ADV_R_PSCFG_WL_ODT_ENTRY PSCfgDIMMWlODT[] = {
    {SR_DIMM0,            {0x01, 0x00, 0x00, 0x00}, 1},
    {DR_DIMM0,            {0x04, 0x00, 0x00, 0x00}, 1},
    {SR_DIMM1,            {0x00, 0x02, 0x00, 0x00}, 1},
    {DR_DIMM1,            {0x00, 0x08, 0x00, 0x00}, 1},
    {SR_DIMM0 + DR_DIMM0 + SR_DIMM1 + DR_DIMM1, {0x03, 0x03, 0x00, 0x00}, 2}
  };

  UINT16 i;
  UINT16 j;
  UINT8 Loads;
  UINT8 Dimms;
  UINT16 Speed;
  UINT16 DIMMRankType;
  UINT16 _DIMMRankType;
  UINT32 AddrTmgCTL;
  UINT32 DctOdcCtl;
  UINT8 PhyWLODT[4];
  BOOLEAN SlowMode;
  UINT8 DimmTpMatch;

  ASSERT (MemData != 0);
  ASSERT (CurrentChannel != 0);

  AddrTmgCTL = 0;
  DctOdcCtl = 0;
  PhyWLODT[0] = 0x0F;
  PhyWLODT[1] = 0x0F;
  PhyWLODT[2] = 0x0F;
  PhyWLODT[3] = 0x0F;

  if ((CurrentChannel->MCTPtr->LogicalCpuid.Family & (AMD_FAMILY_10_DA | AMD_FAMILY_10_BL)) == 0) {
    return AGESA_UNSUPPORTED;
  }
  if (CurrentChannel->TechType != DDR3_TECHNOLOGY) {
    return AGESA_UNSUPPORTED;
  }
  if ((CurrentChannel->RegDimmPresent != 0) || (CurrentChannel->SODimmPresent != 0)) {
    return AGESA_UNSUPPORTED;
  }
  // Prepare inputs
  Loads = CurrentChannel->Loads;
  Dimms = CurrentChannel->Dimms;
  Speed = CurrentChannel->DCTPtr->Timings.Speed;
  DIMMRankType = MemAGetPsRankType (CurrentChannel);

  if ((Speed == DDR1333_FREQUENCY || Speed == DDR1600_FREQUENCY) && (Dimms == 2)) {
    SlowMode = TRUE;   // 2T
  } else if ((Speed == DDR1600_FREQUENCY) && (Dimms == 1) && (Loads >= 16)) {
    SlowMode = TRUE;   // 2T
  } else {
    SlowMode = FALSE;  // 1T
  }

  if (Dimms == 1) {
    for (i = 0; i < GET_SIZE_OF (PSCfg1Dimm); i++) {
      if (Speed == PSCfg1Dimm[i].Speed) {
        if (Loads >= PSCfg1Dimm[i].Loads) {
          AddrTmgCTL = PSCfg1Dimm[i].AddrTmg;
          DctOdcCtl = PSCfg1Dimm[i].Odc;
        } else {
          DctOdcCtl = 0x20113222;
        }
        break;
      }
    }
    ASSERT (i < GET_SIZE_OF (PSCfg1Dimm));
  } else {
    for (i = 0; i < GET_SIZE_OF (PSCfg2Dimm); i++) {
      if (Speed == PSCfg2Dimm[i].Speed) {
        if (Loads <= PSCfg2Dimm[i].Loads) {
          AddrTmgCTL = PSCfg2Dimm[i].AddrTmg;
          DctOdcCtl = PSCfg2Dimm[i].Odc;
          break;
        }
      }
    }
    ASSERT (i < GET_SIZE_OF (PSCfg2Dimm));
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

  CurrentChannel->MemClkDisMap = (UINT8 *) NiUDdr3CLKDis;
  CurrentChannel->CKETriMap = (UINT8 *) NiUDdr3CKETri;
  CurrentChannel->ODTTriMap = (UINT8 *) NiUDdr3ODTTri;
  CurrentChannel->ChipSelTriMap = (UINT8 *) NiUDdr3CSTri;

  CurrentChannel->DctEccDqsLike = 0x0403;
  CurrentChannel->DctEccDqsScale = 0x70;
  CurrentChannel->DctAddrTmg = AddrTmgCTL;
  CurrentChannel->DctOdcCtl = DctOdcCtl;
  for (i = 0; i < sizeof (CurrentChannel->PhyWLODT); i++) {
    CurrentChannel->PhyWLODT[i] = PhyWLODT[i];
  }
  CurrentChannel->SlowMode = SlowMode;

  return AGESA_SUCCESS;
}
