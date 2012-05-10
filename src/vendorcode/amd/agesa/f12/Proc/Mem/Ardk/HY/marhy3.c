/* $NoKeywords:$ */
/**
 * @file
 *
 * marhy3.c
 *
 * Memory Controller, registered dimms
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
#include "ma.h"
#include "OptionMemory.h"
#include "PlatformMemoryConfiguration.h"
#include "mu.h"
#include "Ids.h"
#include "cpuFamRegisters.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_MEM_ARDK_HY_MARHY3_FILECODE
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

STATIC CONST UINT8 ROMDATA HyRDdr3CLKDis[] = {0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00};

// Even chip select maps to M[B,A]_CKE[0]
// Odd chip select maps to M[B,A]_CKE[1]
STATIC CONST UINT8 ROMDATA HyRDdr3CKETri[] = {0x55, 0xAA};

// 2 dimms per channel
// Dimm 0: BP_MEMODTx[2,0]
// Dimm 1: BP_MEMODTx[3,1]
STATIC CONST UINT8 ROMDATA HyRDdr3ODTTri2D[] = {0x03, 0x0C, 0x32, 0xC8};
// 3 dimms per channel
// Dimm 0: BP_MEMODTx[0]
// Dimm 1: BP_MEMODTx[3,1]
// Dimm 2: BP_MEMODTx[2]
STATIC CONST UINT8 ROMDATA HyRDdr3ODTTri3D[] = {0x03, 0x0C, 0x30, 0xC8};
// 4 dimms per channel
// Dimm 0: BP_MEMODTx[0]
// Dimm 1: BP_MEMODTx[1]
// Dimm 2: BP_MEMODTx[2]
// Dimm 3: BP_MEMODTx[3]
STATIC CONST UINT8 ROMDATA HyRDdr3ODTTri4D[] = {0x03, 0x0C, 0x30, 0xC0};

// BIOS must not tri-state chip select pin corresponding to the second chip
// select of a single rank registered dimm
STATIC CONST UINT8 ROMDATA HyRDdr3CSTri[] = {0x01, 0x03, 0x04, 0x0C, 0x10, 0x30, 0x40, 0xC0};

/* -----------------------------------------------------------------------------*/
/**
 *
 *         This is function sets the platform specific settings for HY DDR3 L1 system
 *
 *     @param[in,out]   *MemData           Pointer to MEM_DATA_STRUCTURE
 *     @param[in]       SocketID        Socket number
 *     @param[in]       *CurrentChannel       Pointer to CH_DEF_STRUCT
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
 */

AGESA_STATUS
MemAGetPsCfgRHy3 (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  )
{
  //
  // Address Timings and Drive Strengths for 1 DIMM per channel or 2 Dimms per Channel
  //
  // Code searches table for matching speed, then matches the current dimm
  // population and # of dimms to current config and programs the Addr Timing and RC2/RC2
  //
  // Frequency, Dimm Config ,
  // Address Timing Value(F2x[1, 0]9C_x04), RC2/RC8 Value, # Dimms to match
  //
  STATIC CONST ADV_R_PSCFG_ENTRY PSCfg2DIMMs[] = {
     {DDR800_FREQUENCY, SR_DIMM0 + DR_DIMM0 + SR_DIMM1 + DR_DIMM1, \
       0x00000000, 0x0000, 1},
     {DDR800_FREQUENCY, QR_DIMM0 + QR_DIMM1, \
       0x00000000, 0x0040, 1},
     {DDR800_FREQUENCY, ANY_DIMM0 + ANY_DIMM1, \
       0x00000000, 0x4004, 2},
     {DDR1066_FREQUENCY, SR_DIMM0 + DR_DIMM0 + SR_DIMM1 + DR_DIMM1, \
       0x003C3C3C, 0x0000,1},
     {DDR1066_FREQUENCY, QR_DIMM0 + QR_DIMM1, \
       0x003C3C3C, 0x0040, 1},
     {DDR1066_FREQUENCY, SR_DIMM0 + DR_DIMM0 + SR_DIMM1 + DR_DIMM1, \
       0x003A3C3A, 0x4004, 2},
     {DDR1066_FREQUENCY, QR_DIMM0 + ANY_DIMM1, \
       0x003A3C3A, 0x4004, 2},
     {DDR1066_FREQUENCY, ANY_DIMM0 + QR_DIMM1, \
       0x003A3C3A, 0x4004, 2},
     {DDR1333_FREQUENCY, SR_DIMM0 + DR_DIMM0 + SR_DIMM1 + DR_DIMM1, \
       0x003A3A3A, 0x0000, 1},
     {DDR1333_FREQUENCY, QR_DIMM0 + QR_DIMM1, \
       0x003A3A3A, 0x0040, 1},
     {DDR1333_FREQUENCY, SR_DIMM0 + DR_DIMM0 + SR_DIMM1 + DR_DIMM1, \
       0x00383A38, 0x4040, 2},
     {DDR1333_FREQUENCY, QR_DIMM0 + ANY_DIMM1, \
       0x00383A38, 0x4004, 2},
     {DDR1333_FREQUENCY, ANY_DIMM0 + QR_DIMM1, \
       0x00383A38, 0x4004, 2},
     {DDR1600_FREQUENCY, SR_DIMM0 + DR_DIMM0 + SR_DIMM1 + DR_DIMM1, \
       0x00373937, 0x0000, 1},
     {DDR1600_FREQUENCY, QR_DIMM0 + QR_DIMM1, \
       0x00373937, 0x0040, 1},
     {DDR1600_FREQUENCY, ANY_DIMM0 + ANY_DIMM1, \
       0x00353935, 0x4004, 2}
  };
  //
  // Address Timings and Drive Strengths for 3 DIMMs per channel
  //
  // Code searches table for matching speed, then matches the current dimm
  // population and # of dimms to current config and programs the Addr Timing and RC2/RC2
  //
  // Frequency, Dimm Config ,
  // Address Timing Value(F2x[1, 0]9C_x04), RC2/RC8 Value, # Dimms to match
  //
  STATIC CONST ADV_R_PSCFG_ENTRY PSCfg3DIMMs[] = {
     {DDR800_FREQUENCY, SR_DIMM0 + DR_DIMM0 + SR_DIMM1 + DR_DIMM1 + SR_DIMM2 + DR_DIMM2, \
       0x00000000, 0x0000, 1},
     {DDR800_FREQUENCY, SR_DIMM0 + DR_DIMM0 + SR_DIMM1 + DR_DIMM1 + SR_DIMM2 + DR_DIMM2, \
       0x00000000, 0x4040, 2},
     {DDR800_FREQUENCY, SR_DIMM0 + DR_DIMM0 + ANY_DIMM1 + SR_DIMM2 + DR_DIMM2, \
       0x00380038, 0x4004, 3},
     {DDR800_FREQUENCY, QR_DIMM1, \
       0x00000000, 0x0040, 1},
     {DDR800_FREQUENCY, SR_DIMM0 + DR_DIMM0 + QR_DIMM1 + SR_DIMM2 + DR_DIMM2, \
       0x00000000, 0x4004, 2},
     {DDR1066_FREQUENCY, SR_DIMM0 + DR_DIMM0 + SR_DIMM1 + DR_DIMM1 + SR_DIMM2 + DR_DIMM2, \
       0x003C3C3C, 0x0000, 1},
     {DDR1066_FREQUENCY, SR_DIMM0 + DR_DIMM0 + SR_DIMM1 + DR_DIMM1 + SR_DIMM2 + DR_DIMM2, \
       0x003A3C3A, 0x4040, 2},
     {DDR1066_FREQUENCY, SR_DIMM0 + DR_DIMM0 + SR_DIMM1 + DR_DIMM1 + SR_DIMM2 + DR_DIMM2, \
       0x00373C37, 0x4040, 3},
     {DDR1066_FREQUENCY, SR_DIMM0 + DR_DIMM0 + QR_DIMM1 + SR_DIMM2 + DR_DIMM2, \
       0x00373C37, 0x4004, 3},
     {DDR1066_FREQUENCY, QR_DIMM1, \
       0x003C3C3C, 0x0040, 1},
     {DDR1066_FREQUENCY, SR_DIMM0 + DR_DIMM0 + QR_DIMM1 + SR_DIMM2 + DR_DIMM2, \
       0x003A3C3A, 0x4004, 2},
     {DDR1333_FREQUENCY, SR_DIMM0 + DR_DIMM0 + SR_DIMM1 + DR_DIMM1 + SR_DIMM2 + DR_DIMM2, \
       0x003A3A3A, 0x0000, 1},
     {DDR1333_FREQUENCY, SR_DIMM0 + DR_DIMM0 + SR_DIMM1 + DR_DIMM1 + SR_DIMM2 + DR_DIMM2, \
       0x00383A38, 0x4040, 2},
     {DDR1333_FREQUENCY, SR_DIMM0 + DR_DIMM0 + ANY_DIMM1 + SR_DIMM2 + DR_DIMM2, \
       0x00343A34, 0x4004, 3},
     {DDR1333_FREQUENCY, QR_DIMM1, \
       0x003A3A3A, 0x0040, 1},
     {DDR1333_FREQUENCY, SR_DIMM0 + DR_DIMM0 + QR_DIMM1 + SR_DIMM2 + DR_DIMM2, \
       0x00383A38, 0x4004, 2},
     {DDR1600_FREQUENCY, SR_DIMM0 + DR_DIMM0 + SR_DIMM1 + DR_DIMM1 + SR_DIMM2 + DR_DIMM2, \
       0x00393939, 0x0000, 1},
     {DDR1600_FREQUENCY, SR_DIMM0 + DR_DIMM0 + SR_DIMM1 + DR_DIMM1 + SR_DIMM2 + DR_DIMM2, \
       0x00363936, 0x4040, 2},
     {DDR1600_FREQUENCY, SR_DIMM0 + DR_DIMM0 + ANY_DIMM1 + SR_DIMM2 + DR_DIMM2, \
       0x00303930, 0x4004, 3},
     {DDR1600_FREQUENCY, QR_DIMM1, \
       0x00393939, 0x0040, 1},
     {DDR1600_FREQUENCY, SR_DIMM0 + DR_DIMM0 + QR_DIMM1 + SR_DIMM2 + DR_DIMM2, \
       0x00363936, 0x4004, 2}
  };
  //
  // DIMM ODT Pattern (1 or 2 DIMMs per channel)
  //
  // Dimm Config ,
  // Fn2_9C 180, Fn2_9C 181, Fn2_9C 182, Fn2_9C 183, # Dimms to match
  //
  STATIC CONST ADV_PSCFG_ODT_ENTRY PSCfg2DIMMsODT[] = {
    {SR_DIMM0, \
      0x00000000, 0x00000000, 0x00000001, 0x00000000, 1},
    {DR_DIMM0, \
      0x00000000, 0x00000000, 0x00000104, 0x00000000, 1},
    {QR_DIMM0, \
      0x00000000, 0x00000000, 0x00000505, 0x00000505, 1},
    {SR_DIMM1, \
      0x00000000, 0x00000000, 0x00020000, 0x00000000, 1},
    {DR_DIMM1, \
      0x00000000, 0x00000000, 0x02080000, 0x00000000, 1},
    {QR_DIMM1, \
      0x00000000, 0x00000000, 0x0A0A0000, 0x0A0A0000, 1},
    {SR_DIMM0 + DR_DIMM0 + SR_DIMM1 + DR_DIMM1, \
      0x01010202, 0x00000000, 0x09030603, 0x00000000, 2},
    {SR_DIMM0 + DR_DIMM0 + QR_DIMM1, \
      0x01010A0A, 0x01010000, 0x01030E0B, 0x01090000, 2},
    {QR_DIMM0 + SR_DIMM1 + DR_DIMM1, \
      0x05050202, 0x00000202, 0x0D070203, 0x00000206, 2},
    {QR_DIMM0 + QR_DIMM1, \
      0x05050A0A, 0x05050A0A, 0x05070A0B, 0x050D0A0E, 2}
  };
  // DIMM ODT Pattern (3 DIMMs per channel)
  //
  // Dimm Config ,
  // Fn2_9C 180, Fn2_9C 181, Fn2_9C 182, Fn2_9C 183, # Dimms to match
  //
  STATIC CONST ADV_PSCFG_ODT_ENTRY PSCfg3DIMMsODT[] = {
    {SR_DIMM2 + DR_DIMM2, \
      0x00000000, 0x00000000, 0x00000000, 0x00000404, 1},
    {SR_DIMM0 + DR_DIMM0, \
      0x00000000, 0x00000000, 0x00000101, 0x00000000, 1},
    {SR_DIMM0 + DR_DIMM0 + SR_DIMM2 + DR_DIMM2, \
      0x00000404, 0x00000101, 0x00000405, 0x00000105, 2},
    {SR_DIMM0 + DR_DIMM0 + SR_DIMM1 + DR_DIMM1 + SR_DIMM2 + DR_DIMM2, \
      0x05050606, 0x00000303, 0x0D070607, 0x00000307, 3},
    {QR_DIMM1, \
      0x00000000, 0x00000000, 0x080A0000, 0x020A0000, 1},
    {QR_DIMM1 + SR_DIMM2 + DR_DIMM2, \
      0x04040000, 0x04040A0A, 0x04060000, 0x040C0A0E, 2},
    {SR_DIMM0 + DR_DIMM0 + QR_DIMM1, \
      0x01010A0A, 0x01010000, 0x01030A0B, 0x01090000, 2},
    {SR_DIMM0 + DR_DIMM0 + QR_DIMM1 + SR_DIMM2 + DR_DIMM2, \
      0x05050E0E, 0x05050B0B, 0x05070E0F, 0x050D0B0F, 3}
  };
  //
  // DIMM ODT Pattern (4 DIMMs per channel)
  //
  // Dimm Config ,
  // Fn2_9C 180, Fn2_9C 181, Fn2_9C 182, Fn2_9C 183, # Dimms to match
  //
  STATIC CONST ADV_PSCFG_ODT_ENTRY PSCfg4DIMMsODT[] = {
    {ANY_DIMM3, \
      0x00000000, 0x00000000, 0x00000000, 0x08080000, 1},
    {ANY_DIMM2 + ANY_DIMM3, \
      0x00000000, 0x04040808, 0x00000000, 0x0C0C0C0C, 2},
    {ANY_DIMM1 + ANY_DIMM2 + ANY_DIMM3, \
      0x0C0C0000, 0x06060A0A, 0x0E0E0000, 0x0E0E0E0E, 3},
    {ANY_DIMM0 + ANY_DIMM1 + ANY_DIMM2 + ANY_DIMM3, \
      0x0D0D0E0E, 0x07070B0B, 0x0F0F0F0F, 0x0F0F0F0F, 4}
  };
  //
  // DIMM Write Leveling ODT Pattern for 1 or 2 Dimms Per Channel
  //
  // Dimm Config, WrLvOdt (Fn2_9C_0x08[11:8]) Value for each Dimm, # Dimms to match
  //
  STATIC CONST ADV_R_PSCFG_WL_ODT_ENTRY PSCfg2DIMMsWlODT[] = {
    {SR_DIMM0,            {0x01, 0x00, 0x00, 0x00}, 1},
    {DR_DIMM0,            {0x04, 0x00, 0x00, 0x00}, 1},
    {QR_DIMM0,            {0x05, 0x00, 0x00, 0x00}, 1},
    {SR_DIMM1,            {0x00, 0x02, 0x00, 0x00}, 1},
    {DR_DIMM1,            {0x00, 0x08, 0x00, 0x00}, 1},
    {QR_DIMM1,            {0x00, 0x0A, 0x00, 0x00}, 1},
    {SR_DIMM0 + DR_DIMM0 + SR_DIMM1 + DR_DIMM1, {0x03, 0x03, 0x00, 0x00}, 2},
    {SR_DIMM0 + QR_DIMM1, {0x0B, 0x03, 0x00, 0x09}, 2},
    {DR_DIMM0 + QR_DIMM1, {0x0B, 0x03, 0x00, 0x09}, 2},
    {QR_DIMM0 + SR_DIMM1, {0x03, 0x07, 0x06, 0x00}, 2},
    {QR_DIMM0 + DR_DIMM1, {0x03, 0x07, 0x06, 0x00}, 2},
    {QR_DIMM0 + QR_DIMM1, {0x0B, 0x07, 0x0E, 0x0D}, 2}
  };
  //
  // DIMM Write Leveling ODT Pattern 3 Dimms Per Channel
  //
  // Dimm Config, WrLvOdt (Fn2_9C_0x08[11:8]) Value for each Dimm, # Dimms to match
  //
  STATIC CONST ADV_R_PSCFG_WL_ODT_ENTRY PSCfg3DIMMsWlODT[] = {
    {SR_DIMM2 + DR_DIMM2, {0x00, 0x00, 0x04, 0x00}, 1},
    {SR_DIMM0 + DR_DIMM0, {0x01, 0x02, 0x00, 0x00}, 1},
    {SR_DIMM0 + DR_DIMM0 + SR_DIMM2 + DR_DIMM2, {0x05, 0x00, 0x05, 0x00}, 2},
    {SR_DIMM0 + DR_DIMM0 + SR_DIMM1 + DR_DIMM1 + SR_DIMM2 + DR_DIMM2, {0x07, 0x07, 0x07, 0x00}, 3},
    {QR_DIMM1, {0x00, 0x0A, 0x00, 0x0A}, 1},
    {QR_DIMM1 + SR_DIMM2 + DR_DIMM2, {0x00, 0x06, 0x0E, 0x0C}, 2},
    {SR_DIMM0 + DR_DIMM0 + QR_DIMM1, {0x0B, 0x03, 0x00, 0x09}, 2},
    {SR_DIMM0 + DR_DIMM0 + QR_DIMM1 + SR_DIMM2 + DR_DIMM2, {0x0F, 0x07, 0x0F, 0x0D}, 3}
  };
  //
  // DIMM Write Leveling ODT Pattern 4 Dimms Per Channel
  //
  // Dimm Config, WrLvOdt (Fn2_9C_0x08[11:8]) Value for each Dimm, # Dimms to match
  //
  STATIC CONST ADV_R_PSCFG_WL_ODT_ENTRY PSCfg4DIMMsWlODT[] = {
    {ANY_DIMM3, {0x00, 0x00, 0x00, 0x08}, 1},
    {ANY_DIMM2 + ANY_DIMM3, {0x00, 0x00, 0x0C, 0x0C}, 2},
    {ANY_DIMM1 + ANY_DIMM2 + ANY_DIMM3, {0x00, 0x0E, 0x0E, 0x0E}, 3},
    {ANY_DIMM0 + ANY_DIMM1 + ANY_DIMM2 + ANY_DIMM3, {0x0F, 0x0F, 0x0F, 0x0F}, 4}
  };

  UINT16 i;
  UINT16 j;
  UINT8  MaxDimmPerCH;
  UINT8 Dimms;
  UINT16 Speed;
  UINT16 DIMMRankType;
  UINT16 _DIMMRankType;
  UINT8 DimmTpMatch;
  UINT32 AddrTmgCTL;
  UINT32 DctOdcCtl;
  UINT16 RC2RC8;
  UINT32 PhyRODTCSLow;
  UINT32 PhyRODTCSHigh;
  UINT32 PhyWODTCSLow;
  UINT32 PhyWODTCSHigh;
  BOOLEAN SlowMode;
  UINT8 PSCfgSize;
  UINT8 PSCfgODTSize;
  UINT8 PSCfgWlODTSize;
  UINT8 PhyWLODT[4];

  CONST ADV_R_PSCFG_ENTRY *PSCfgPtr;
  CONST ADV_PSCFG_ODT_ENTRY *PSCfgODTPtr;
  CONST ADV_R_PSCFG_WL_ODT_ENTRY *PSCfgWlODTPtr;
  UINT8 *DimmsPerChPtr;

  ASSERT (MemData != NULL);
  ASSERT (CurrentChannel != NULL);

  AddrTmgCTL = 0;
  DctOdcCtl = 0;
  RC2RC8 = 0;
  PhyRODTCSLow = 0;
  PhyRODTCSHigh = 0;
  PhyWODTCSLow = 0;
  PhyWODTCSHigh = 0;
  SlowMode = FALSE;
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
  if (CurrentChannel->RegDimmPresent == 0) {
    return AGESA_UNSUPPORTED;
  }

  // Prepare inputs
  Dimms = CurrentChannel->Dimms;
  Speed = CurrentChannel->DCTPtr->Timings.Speed;
  DimmsPerChPtr = FindPSOverrideEntry (MemData->ParameterListPtr->PlatformMemoryConfiguration, PSO_MAX_DIMMS, SocketID, CurrentChannel->ChannelID);
  if (DimmsPerChPtr != NULL) {
    MaxDimmPerCH = *DimmsPerChPtr;
  } else {
    MaxDimmPerCH = 2;
  }

  DIMMRankType = MemAGetPsRankType (CurrentChannel);

  if (MaxDimmPerCH == 4) {
    PSCfgPtr = NULL;
    PSCfgSize = NULL;
    PSCfgODTPtr = PSCfg4DIMMsODT;
    PSCfgWlODTPtr = PSCfg4DIMMsWlODT;
    PSCfgODTSize = sizeof (PSCfg4DIMMsODT) / sizeof (ADV_PSCFG_ODT_ENTRY);
    PSCfgWlODTSize = sizeof (PSCfg4DIMMsWlODT) / sizeof (ADV_R_PSCFG_WL_ODT_ENTRY);
  } else if (MaxDimmPerCH == 3) {
    PSCfgPtr = PSCfg3DIMMs;
    PSCfgSize = sizeof (PSCfg3DIMMs) / sizeof (ADV_R_PSCFG_ENTRY);
    PSCfgODTPtr = PSCfg3DIMMsODT;
    PSCfgWlODTPtr = PSCfg3DIMMsWlODT;
    PSCfgODTSize = sizeof (PSCfg3DIMMsODT) / sizeof (ADV_PSCFG_ODT_ENTRY);
    PSCfgWlODTSize = sizeof (PSCfg3DIMMsWlODT) / sizeof (ADV_R_PSCFG_WL_ODT_ENTRY);
  } else {
    PSCfgPtr = PSCfg2DIMMs;
    PSCfgSize = sizeof (PSCfg2DIMMs) / sizeof (ADV_R_PSCFG_ENTRY);
    PSCfgODTPtr = PSCfg2DIMMsODT;
    PSCfgWlODTPtr = PSCfg2DIMMsWlODT;
    PSCfgODTSize = sizeof (PSCfg2DIMMsODT) / sizeof (ADV_PSCFG_ODT_ENTRY);
    PSCfgWlODTSize = sizeof (PSCfg2DIMMsWlODT) / sizeof (ADV_R_PSCFG_WL_ODT_ENTRY);
  }

  // AddrTmgCTL and DctOdcCtl
  if (MaxDimmPerCH != 4) {
    for (i = 0; i < PSCfgSize; i++, PSCfgPtr++) {
      if ((Speed != PSCfgPtr->Speed) || (Dimms != PSCfgPtr->Dimms)) {
        continue;
      }
      DimmTpMatch = 0;
      _DIMMRankType = DIMMRankType & PSCfgPtr->DIMMRankType;
      for (j = 0; j < MAX_DIMMS_PER_CHANNEL; j++) {
        if ((_DIMMRankType & (UINT16) 0x0F << (j << 2)) != 0) {
          DimmTpMatch++;
        }
      }
      if (DimmTpMatch == PSCfgPtr->Dimms) {
        AddrTmgCTL = PSCfgPtr->AddrTmg;
        DctOdcCtl = 0x00223222;
        RC2RC8 = PSCfgPtr->RC2RC8;
        break;
      }
    }
  }

  //
  // Overrides and/or exceptions
  //
  DimmTpMatch = 0;
  for (j = 0; j < MAX_DIMMS_PER_CHANNEL; j++) {
    if ((DIMMRankType & (UINT16) 0x03 << (j << 2)) != 0) {
      DimmTpMatch++;
    }
  }
  if (MaxDimmPerCH == 4) {
    if (DimmTpMatch > 0) {
      DctOdcCtl = 0x00223222;
      if ((Speed == DDR800_FREQUENCY) && (DimmTpMatch == 1)) {
        DctOdcCtl = 0x00113222;
      }
    }
    if (DimmTpMatch >= 3) {
      AddrTmgCTL |= 0x002F0000;
    }
    if (DimmTpMatch >= 2) {
      RC2RC8 = 0x4040;
    }
  } else if ((MaxDimmPerCH == 3) && (CurrentChannel->Dimms == 3)) {
    DctOdcCtl = 0x00113222;
  } else {
    if ((Dimms == 1) && (DimmTpMatch == 1)) {
      DctOdcCtl = 0x00113222;
    }
  }

  //RC2 and RC8
  for (j = 0; j < MAX_DIMMS_PER_CHANNEL; j++) {
    // CtrlWrd02(s) will contain the info. of SPD byte 63 after MemTDIMMPresence3 execution.
    if (CurrentChannel->CtrlWrd02[j] > 0) {
      if (CurrentChannel->CtrlWrd02[j] == 1) {
        // Store real RC2 and RC8 value (High byte) into CtrlWrd02(s) and CtrlWrd08(s).
        CurrentChannel->CtrlWrd02[j] = (UINT8) (RC2RC8 >> 12) & 0x000F;
        CurrentChannel->CtrlWrd08[j] = (UINT8) (RC2RC8 >> 8) & 0x000F;
      } else {
        // Store real RC2 and RC8 value (low byte) into CtrlWrd02(s) and CtrlWrd08(s).
        CurrentChannel->CtrlWrd02[j] = (UINT8) (RC2RC8 >> 4) & 0x000F;
        CurrentChannel->CtrlWrd08[j] = (UINT8) RC2RC8 & 0x000F;
      }
    }
  }

  //Programmable ODT
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

  //WLODT
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

  // Set ProcODT
  DctOdcCtl |= 0x20000000;

  CurrentChannel->MemClkDisMap = (UINT8 *) HyRDdr3CLKDis;
  CurrentChannel->CKETriMap = (UINT8 *) HyRDdr3CKETri;
  CurrentChannel->ChipSelTriMap = (UINT8 *) HyRDdr3CSTri;

  switch (MaxDimmPerCH) {
  case 3:
    CurrentChannel->ODTTriMap = (UINT8 *) HyRDdr3ODTTri3D;
    break;
  case 4:
    CurrentChannel->ODTTriMap = (UINT8 *) HyRDdr3ODTTri4D;
    break;
  default:
    CurrentChannel->ODTTriMap = (UINT8 *) HyRDdr3ODTTri2D;  // Most conservative
  }

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
