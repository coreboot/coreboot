/* $NoKeywords:$ */
/**
 * @file
 *
 * mardr2.c
 *
 * Platform specific settings for DR DDR2 L1 system
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Ardk)
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

/* This file contains routine that add platform specific support L1 */


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

#define FILECODE PROC_MEM_ARDK_DR_MARDR2_FILECODE
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

STATIC CONST UINT8 ROMDATA DrRDdr2CLKDis[] = {0x00, 0x00, 0xC0, 0x30, 0x0C, 0x03, 0x00, 0x00};

// Chip select 0, 1, 4, 5 maps to M[B,A]_CKE[0]
// Chip select 2, 3, 6, 7 maps to M[B,A]_CKE[1]
STATIC CONST UINT8 ROMDATA DrRDdr2CKETri[] = {0x33, 0xCC};

// 2 dimms per channel
// Dimm 0: BP_MEMODTx[2,0]
// Dimm 1: BP_MEMODTx[3,1]
STATIC CONST UINT8 ROMDATA DrRDdr2ODTTri2D[] = {0x03, 0x0C, 0x32, 0xC8};
// 3 dimms per channel
// Dimm 0: BP_MEMODTx[0]
// Dimm 1: BP_MEMODTx[3,1]
// Dimm 2: BP_MEMODTx[2]
STATIC CONST UINT8 ROMDATA DrRDdr2ODTTri3D[] = {0x03, 0x0C, 0x30, 0xC8};
// 4 dimms per channel
// Dimm 0: BP_MEMODTx[0]
// Dimm 1: BP_MEMODTx[1]
// Dimm 2: BP_MEMODTx[2]
// Dimm 3: BP_MEMODTx[3]
STATIC CONST UINT8 ROMDATA DrRDdr2ODTTri4D[] = {0x03, 0x0C, 0x30, 0xC0};

// BIOS must not tri-state chip select pin corresponding to the second chip
// select of a single rank registered dimm
STATIC CONST UINT8 ROMDATA DrRDdr2CSTri[] = {0x01, 0x03, 0x04, 0x0C, 0x10, 0x30, 0x40, 0xC0};

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This is function sets the platform specific settings for DR DDR2 L1 system
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
MemAGetPsCfgRDr2 (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  )
{
  STATIC CONST ADV_PSCFG_ENTRY PSCfg4D[] = {
    {ANY_, ANY_, 0x00000000, 0x00111222, 1},
    {ANY_, ANY_, 0x00370000, 0x00111222, 2}
  };

  STATIC CONST ADV_PSCFG_ENTRY PSCfg8D[] = {
    {ANY_, ANY_, 0x00000000, 0x00111222, 1},
    {ANY_, ANY_, 0x00370000, 0x00111222, 2},
    {ANY_, ANY_, 0x002F0000, 0x00111222, ANY_}
  };

  CONST ADV_PSCFG_ENTRY *PSCfgPtr;
  UINT16 i;
  UINT8  MaxDimmPerCH;
  UINT16 TabSize;
  UINT8 Loads;
  UINT8 Dimms;
  UINT16 Speed;
  UINT32 AddrTmgCTL;
  UINT32 DctOdcCtl;
  UINT16 QRPresent;
  UINT16 DRx4Present;
  BOOLEAN SlowMode;
  UINT8 *DimmsPerChPtr;

  ASSERT (MemData != 0);
  ASSERT (CurrentChannel != 0);

  if ((CurrentChannel->MCTPtr->LogicalCpuid.Family & AMD_FAMILY_10_RB) == 0) {
    return AGESA_UNSUPPORTED;
  }
  if (CurrentChannel->TechType != DDR2_TECHNOLOGY) {
    return AGESA_UNSUPPORTED;
  }
  if (CurrentChannel->RegDimmPresent != CurrentChannel->ChDimmValid) {
    return AGESA_UNSUPPORTED;
  }

  // Prepare inputs
  Loads = CurrentChannel->Loads;
  Dimms = CurrentChannel->Dimms;
  Speed = CurrentChannel->DCTPtr->Timings.Speed;
  DimmsPerChPtr = FindPSOverrideEntry (MemData->ParameterListPtr->PlatformMemoryConfiguration, PSO_MAX_DIMMS, SocketID, CurrentChannel->ChannelID, 0, NULL, NULL);
  if (DimmsPerChPtr != NULL) {
    MaxDimmPerCH = *DimmsPerChPtr;
  } else {
    MaxDimmPerCH = 2;
  }
  QRPresent = CurrentChannel->DimmQrPresent;
  DRx4Present = CurrentChannel->DimmDrPresent & CurrentChannel->Dimmx4Present;
  if (QRPresent) {
    Dimms = (Dimms + 1) / 2;
  }

  // Table look up
  if (MaxDimmPerCH <= 2) {
    PSCfgPtr = PSCfg4D;
    TabSize = GET_SIZE_OF (PSCfg4D);
  } else {
    PSCfgPtr = PSCfg8D;
    TabSize = GET_SIZE_OF (PSCfg8D);
  }
  DctOdcCtl = 0;
  AddrTmgCTL = 0;
  for (i = 0; i < TabSize; i++) {
    if ((PSCfgPtr[i].Dimms == ANY_) || (PSCfgPtr[i].Dimms == Dimms)) {
      if ((PSCfgPtr[i].Speed == ANY_) || (PSCfgPtr[i].Speed == Speed)) {
        if ((PSCfgPtr[i].Loads == ANY_) || (PSCfgPtr[i].Loads >= Loads)) {
          AddrTmgCTL = PSCfgPtr[i].AddrTmg;
          DctOdcCtl = PSCfgPtr[i].Odc;
          break;
        }
      }
    }
  }
  ASSERT (i == TabSize);
  SlowMode = FALSE;  // 1T

  //
  // Overrides and/or exceptions
  //

  if (QRPresent == 0x55) {
    // QR for 4DIMM case only
    AddrTmgCTL = 0x002F0000;
    if (Speed >= DDR667_FREQUENCY) {
      DctOdcCtl = 0x00331222;
    }
  }

  if (Speed >= DDR667_FREQUENCY) {
    if ((QRPresent != 0) || (DRx4Present != 0)) {
      AddrTmgCTL |= 0x00002F00;
    }
    if (Dimms >= 3) {
      AddrTmgCTL |= 0x0000002F;
    }
    if (Dimms == 3 || Dimms == 4) {
      DctOdcCtl = 0x00331222;
    }
  }

  // Adjust Processor ODT
  if (Dimms == 1) {
    DctOdcCtl |= 0x20000000;        // 75ohms
  } else {
    DctOdcCtl |= 0x10000000;        // 150ohms
  }

  CurrentChannel->MemClkDisMap = (UINT8 *) DrRDdr2CLKDis;
  CurrentChannel->CKETriMap = (UINT8 *) DrRDdr2CKETri;
  CurrentChannel->ChipSelTriMap = (UINT8 *) DrRDdr2CSTri;

  switch (MaxDimmPerCH) {
  case 3:
    CurrentChannel->ODTTriMap = (UINT8 *) DrRDdr2ODTTri3D;
    break;
  case 4:
    CurrentChannel->ODTTriMap = (UINT8 *) DrRDdr2ODTTri4D;
    break;
  default:
    CurrentChannel->ODTTriMap = (UINT8 *) DrRDdr2ODTTri2D;
  }

  CurrentChannel->DctEccDqsLike = 0x0504;
  CurrentChannel->DctEccDqsScale = 0;
  CurrentChannel->DctAddrTmg = AddrTmgCTL;
  CurrentChannel->DctOdcCtl = DctOdcCtl;
  CurrentChannel->SlowMode = SlowMode;

  return AGESA_SUCCESS;
}
