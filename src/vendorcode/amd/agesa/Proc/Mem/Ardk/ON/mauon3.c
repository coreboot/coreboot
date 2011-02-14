/* $NoKeywords:$ */
/*
 * @file
 *
 * mauon3.c
 *
 * Platform specific settings for ON DDR3 unbuffered dimms
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Ardk/ON)
 * @e \$Revision: 38442 $ @e \$Date: 2010-09-24 06:39:57 +0800 (Fri, 24 Sep 2010) $
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

/* This file contains routine that add platform specific support AM3 */


#include "AGESA.h"
#include "PlatformMemoryConfiguration.h"
#include "mport.h"
#include "ma.h"
#include "Ids.h"
#include "cpuFamRegisters.h"
#include "Filecode.h"
#define FILECODE PROC_MEM_ARDK_ON_MAUON3_FILECODE
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

STATIC CONST UINT8 ROMDATA OnUDdr3CLKDis[] = {0x01, 0x02, 0x04, 0x08, 0x00, 0x00, 0x00, 0x00};
STATIC CONST UINT8 ROMDATA OnUDdr3CKETri[] = {0xFF, 0xFF};
STATIC CONST UINT8 ROMDATA OnUDdr3ODTTri[] = {0x01, 0x02, 0x04, 0x08};
STATIC CONST UINT8 ROMDATA OnUDdr3CSTri[] = {0x01, 0x02, 0x04, 0x08, 0x00, 0x00, 0x00, 0x00};

/* -----------------------------------------------------------------------------*/
/**
 *
 *         This is function sets the platform specific settings for ON DDR3 Unbuffered dimms
 *
 *
 *     @param[in,out]   *MemData           Pointer to MEM_DATA_STRUCTURE
 *     @param[in]       SocketID          Socket number
 *     @param[in,out]   *CurrentChannel       Pointer to CH_DEF_STRUCT
 *
 *     @return          AGESA_SUCCESS
 *     @return          CurrentChannel->MemClkDisMap     Points this pointer to LN MemClkDis table
 *     @return          CurrentChannel->ChipSelTriMap    Points this pointer to LN CS table
 *     @return          CurrentChannel->CKETriMap        Points this pointer to LN ODT table
 *     @return          CurrentChannel->ODTTriMap        Points this pointer to LN CKE table
 *     @return          CurrentChannel->DctEccDQSLike      Indicates the bytes that should be averaged for ECC
 *     @return          CurrentChannel->DctEccDQSScale     Indicates the scale that should be used for Averaging ECC byte
 *     @return          CurrentChannel->DctAddrTmg        Address Command Timing Settings for specified channel
 *     @return          CurrentChannel->DctOdcCtl         Drive Strength settings for specified channel
 *     @return          CurrentChannel->SlowMode             Slow Mode
 *
 */

AGESA_STATUS
MemAGetPsCfgUON3 (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  )
{
  CONST ADV_PSCFG_ENTRY PSCfg[] = {
    {DDR800_FREQUENCY, SR_DIMM0 + SR_DIMM1, 0x00000000, 0x00112223, 1},
    {DDR800_FREQUENCY, DR_DIMM0 + DR_DIMM1, 0x003B0000, 0x00112223, 1},
    {DDR800_FREQUENCY, ANY_, 0x00390039, 0x20222323, 2},
    {DDR1066_FREQUENCY, SR_DIMM0 + SR_DIMM1, 0x00000000, 0x10112223, 1},
    {DDR1066_FREQUENCY, DR_DIMM0 + DR_DIMM1, 0x00380000, 0x10112223, 1},
    {DDR1066_FREQUENCY, ANY_, 0x00350037, 0x30222323, 2},
    {DDR1333_FREQUENCY, SR_DIMM0 + SR_DIMM1, 0x00000000, 0x30112223, 1},
    {DDR1333_FREQUENCY, DR_DIMM0 + DR_DIMM1, 0x00360000, 0x30112223, 1},
    {DDR1333_FREQUENCY, ANY_, 0x00000035, 0x30222323, 2}
  };

  //
  // DIMM ODT Pattern
  //
  // Dimm Config ,
  // Fn2_F4 180, Fn2_F4 181, Fn2_F4 182, Fn2_F4 183, # Dimms to match
  //
  STATIC CONST ADV_PSCFG_ODT_ENTRY PSCfgDIMMsODT[] = {
    {SR_DIMM0, \
      0x00000000, 0x00000000, 0x00000001, 0x00000000, 1},
    {DR_DIMM0, \
      0x00000000, 0x00000000, 0x00000201, 0x00000000, 1},
    {SR_DIMM1, \
      0x00000000, 0x00000000, 0x00040000, 0x00000000, 1},
    {DR_DIMM1, \
      0x00000000, 0x00000000, 0x08040000, 0x00000000, 1},
    {SR_DIMM0 + DR_DIMM0 + SR_DIMM1 + DR_DIMM1, \
      0x01010404, 0x00000000, 0x09050605, 0x00000000, 2}
  };

  UINT16 i;
  UINT16 j;
  UINT8 Dimms;
  UINT16 Speed;
  UINT16 DIMMRankType;
  UINT16 _DIMMRankType;
  UINT32 AddrTmgCTL;
  UINT32 DctOdcCtl;
  UINT8 PhyWLODT[4];
  UINT32 PhyRODTCS;
  UINT32 PhyWODTCS;
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
  PhyRODTCS = 0;
  PhyWODTCS = 0;

  if ((CurrentChannel->MCTPtr->LogicalCpuid.Family & AMD_FAMILY_ON) == 0) {
    return AGESA_UNSUPPORTED;
  }
  if (CurrentChannel->TechType != DDR3_TECHNOLOGY) {
    return AGESA_UNSUPPORTED;
  }
  if ((CurrentChannel->RegDimmPresent != 0) || (CurrentChannel->SODimmPresent != 0)) {
    return AGESA_UNSUPPORTED;
  }

  // Prepare inputs
  Dimms = CurrentChannel->Dimms;
  Speed = CurrentChannel->DCTPtr->Timings.Speed;

  DIMMRankType = MemAGetPsRankType (CurrentChannel);

  if ((Speed == DDR1333_FREQUENCY) && (Dimms == 2)) {
    SlowMode = TRUE;   // 2T
  } else {
    SlowMode = FALSE;  // 1T
  }

  for (i = 0; i < GET_SIZE_OF (PSCfg); i++) {
    if ((PSCfg[i].Dimms == ANY_) || (PSCfg[i].Dimms == Dimms)) {
      if ((PSCfg[i].Speed == ANY_) || (PSCfg[i].Speed == Speed)) {
        if ((PSCfg[i].Loads == ANY_) || ((PSCfg[i].Loads & DIMMRankType) != 0)) {
          AddrTmgCTL = PSCfg[i].AddrTmg;
          DctOdcCtl = PSCfg[i].Odc;
          break;
        }
      }
    }
  }

  ASSERT (i < GET_SIZE_OF (PSCfg));

  //
  // Programmable Dimm ODT
  //
  for (i = 0; i < GET_SIZE_OF (PSCfgDIMMsODT); i++) {
    if (Dimms != PSCfgDIMMsODT[i].Dimms) {
      continue;
    }
    DimmTpMatch = 0;
    _DIMMRankType = DIMMRankType & PSCfgDIMMsODT[i].DIMMRankType;
    for (j = 0; j < MAX_DIMMS_PER_CHANNEL; j++) {
      if ((_DIMMRankType & (UINT16) 0x0F << (j << 2)) != 0) {
        DimmTpMatch++;
      }
    }
    if (DimmTpMatch == PSCfgDIMMsODT[i].Dimms) {
      PhyRODTCS = PSCfgDIMMsODT[i].PhyRODTCSLow;
      PhyWODTCS = PSCfgDIMMsODT[i].PhyWODTCSLow;
      break;
    }
  }

  //
  //WL ODT
  //
  PhyWLODT[0] = (UINT8) (PhyWODTCS & 0x0F);
  PhyWLODT[1] = (UINT8) ((PhyWODTCS >> 16) & 0x0F);
  PhyWLODT[2] = PhyWLODT[3] = 0;

  //
  // Overrides and/or exceptions
  //
  CurrentChannel->MemClkDisMap = (UINT8 *) OnUDdr3CLKDis;
  CurrentChannel->CKETriMap = (UINT8 *) OnUDdr3CKETri;
  CurrentChannel->ODTTriMap = (UINT8 *) OnUDdr3ODTTri;
  CurrentChannel->ChipSelTriMap = (UINT8 *) OnUDdr3CSTri;

  CurrentChannel->DctEccDqsLike = 0x0403;
  CurrentChannel->DctEccDqsScale = 0x70;
  CurrentChannel->DctAddrTmg = AddrTmgCTL;
  CurrentChannel->DctOdcCtl = DctOdcCtl;
  CurrentChannel->PhyRODTCSLow  = PhyRODTCS;
  CurrentChannel->PhyWODTCSLow  = PhyWODTCS;
  for (i = 0; i < sizeof (CurrentChannel->PhyWLODT); i++) {
    CurrentChannel->PhyWLODT[i] = PhyWLODT[i];
  }
  CurrentChannel->SlowMode = SlowMode;

  return AGESA_SUCCESS;
}
