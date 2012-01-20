/* $NoKeywords:$ */
/**
 * @file
 *
 * mrndctOr.c
 *
 * Northbridge DCT support for Orochi Recovery
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Proc/Recovery/Mem)
 * @e \$Revision: 49896 $ @e \$Date: 2011-03-30 02:18:18 -0600 (Wed, 30 Mar 2011) $
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


/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */

#include "AGESA.h"
#include "OptionMemory.h"
#include "PlatformMemoryConfiguration.h"
#include "Ids.h"
#include "mrport.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "mp.h"
#include "mru.h"
#include "mrt3.h"
#include "mrnor.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_RECOVERY_MEM_NB_OR_MRNDCTOR_FILECODE

/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function gets platform specific config/timing values from the interface layer and
 *   programs them into DCT.
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE - An Error value lower than AGESA_ERROR may have occurred
 *     @return          FALSE - An Error value greater than or equal to AGESA_ERROR may have occurred
 */

BOOLEAN
MemRecNPlatformSpecOr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  CONST UINT32 OdtPattern3D[][4] = {
    {0x00000000, 0x00000000, 0x00000404, 0x00000000},
    {0x00000101, 0x00000404, 0x00000105, 0x00000405},
    {0x00000303, 0x05050606, 0x00000307, 0x0D070607},
    {0x00000000, 0x00000000, 0x020A0000, 0x080A0000},
    {0x04040A0A, 0x04040000, 0x040C0A0E, 0x04060000},
    {0x05050B0B, 0x05050E0E, 0x050D0B0F, 0x05070E0F}
  };

  UINT32 PhyRODTCSHigh;
  UINT32 PhyRODTCSLow;
  UINT32 PhyWODTCSHigh;
  UINT32 PhyWODTCSLow;
  UINT8  MaxDimmPerCH;
  UINT32 ODC;
  UINT32 AddrCmd;
  UINT8  i;
  CH_DEF_STRUCT *ChannelPtr;

  ChannelPtr = NBPtr->ChannelPtr;

  MaxDimmPerCH = RecGetMaxDimmsPerChannel (NBPtr->RefPtr->PlatformMemoryConfiguration, 0, ChannelPtr->ChannelID);

  // Set ODC, AddrCmdTmg
  if (ChannelPtr->Dimms == 1) {
    ODC = 0x10112222;
    AddrCmd = 0x003B0000;
  } else {
    ODC = 0x10222222;
    AddrCmd = 0x00390039;
  }
  if (ChannelPtr->DimmQrPresent != 0) {
    ODC = 0x10222222;
  }
  MemRecNSetBitFieldNb (NBPtr, BFODCControl, ODC);
  MemRecNSetBitFieldNb (NBPtr, BFAddrTmgControl, AddrCmd);

  // Find Rtt_Nom, Rtt_Wr
  if (ChannelPtr->DimmQrPresent == 0) {
    if (ChannelPtr->Dimms == 1) {
      NBPtr->PsPtr->DramTerm = 2;
      NBPtr->PsPtr->DynamicDramTerm = 0;
    } else {
      NBPtr->PsPtr->DramTerm = 3;
      NBPtr->PsPtr->DynamicDramTerm = 2;
    }
  } else {
    NBPtr->PsPtr->DramTerm = 2;
    NBPtr->PsPtr->DynamicDramTerm = 2;
  }

  // Set ODT patterns
  PhyRODTCSLow  = 0x00000000;
  PhyRODTCSHigh = 0x00000000;
  PhyWODTCSHigh = 0x00000000;
  if (MaxDimmPerCH != 3) {
    if (ChannelPtr->DimmQrPresent == 0) {
      if (ChannelPtr->Dimms == 1) {
        PhyWODTCSLow  = 0x08020401;
      } else {
        PhyRODTCSLow = 0x01010202;
        PhyWODTCSLow = 0x09030603;
      }
    } else {
      if (ChannelPtr->Dimms == 1) {
        PhyWODTCSLow  = 0x080A0505;
        PhyWODTCSHigh = 0x020A0505;
      } else {
        PhyRODTCSHigh = 0x05050A0A;
        PhyRODTCSLow  = 0x05050A0A;
        PhyWODTCSHigh = 0x050D0A0E;
        PhyWODTCSLow  = 0x05070A0E;
      }
    }
  } else {
    i = ChannelPtr->Dimms + ((ChannelPtr->DimmQrPresent != 0) ? 3 : 0) - 1;
    PhyWODTCSLow  = OdtPattern3D[i][0];
    PhyWODTCSHigh = OdtPattern3D[i][1];
    PhyRODTCSLow  = OdtPattern3D[i][2];
    PhyRODTCSHigh = OdtPattern3D[i][3];
  }

  MemRecNSetBitFieldNb (NBPtr, BFPhyWODTCSLow, PhyWODTCSLow);
  MemRecNSetBitFieldNb (NBPtr, BFPhyRODTCSLow, PhyRODTCSLow);
  MemRecNSetBitFieldNb (NBPtr, BFPhyRODTCSHigh, PhyRODTCSHigh);
  MemRecNSetBitFieldNb (NBPtr, BFPhyWODTCSHigh, PhyWODTCSHigh);
  return TRUE;
}


/* -----------------------------------------------------------------------------*/
/**
 *
 *   Set Dram ODT for mission mode and write leveling mode.
 *
 *     @param[in,out]   *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in]       OdtMode    - Mission mode or write leveling mode
 *     @param[in]       ChipSelect - Chip select number
 *     @param[in]       TargetCS   - Chip select number that is being trained
 *
 */

VOID
MemRecNSetDramOdtOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       ODT_MODE OdtMode,
  IN       UINT8 ChipSelect,
  IN       UINT8 TargetCS
  )
{
  UINT8 DramTerm;
  UINT8 DramTermDyn;

  // Dram nominal termination & Dram dynamic termination
  DramTerm = NBPtr->PsPtr->DramTerm;
  DramTermDyn = NBPtr->PsPtr->DynamicDramTerm;

  if (OdtMode == WRITE_LEVELING_MODE) {
    if (ChipSelect == TargetCS) {
      DramTerm = DramTermDyn;
      MemRecNSetBitFieldNb (NBPtr, BFWrLvOdt, 0xF); // Use MRS commands to disable ODT#
    }
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  This function initializes the DRAM devices on all DCTs at the same time
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */
VOID
MemRecNStartupDCTOr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  // Program D18F2x[1,0]9C_x0000_000B = 80000000h.
  MemRecNSetBitFieldNb (NBPtr, BFDramPhyStatusReg, 0x80000000);

  // Program D18F2x[1,0]9C_x0D0F_E013[PllRegWaitTime] = 0118h.
  MemRecNSetBitFieldNb (NBPtr, BFPllRegWaitTime, 0x118);

  // Phy Voltage Level Programming
  MemRecNPhyVoltageLevelNb (NBPtr);

  // Run frequency change sequence
  MemRecNSetBitFieldNb (NBPtr, BFPllLockTime, 0x190);
  MemRecNSetBitFieldNb (NBPtr, BFMemClkFreq, 4);

  MemRecNSetBitFieldNb (NBPtr, BFMemClkFreqVal, 1);
  while (MemRecNGetBitFieldNb (NBPtr, BFFreqChgInProg) == 1) {};

  MemRecNSetBitFieldNb (NBPtr, BFPllLockTime, 0x000F);

  // Run DramInit sequence
  AGESA_TESTPOINT (TpProcMemDramInit, &(NBPtr->MemPtr->StdHeader));
  NBPtr->TechPtr->DramInit (NBPtr->TechPtr);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function  sets the maximum round-trip latency in the system from the processor to the DRAM
 *   devices and back.

 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]     MaxRcvEnDly - Maximum receiver enable delay value
 *
 */

VOID
MemRecNSetMaxLatencyOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT16 MaxRcvEnDly
  )
{
  UINT32 N;
  UINT32 T;
  UINT32 P;
  UINT32 MemClkPeriod;
  UINT32 NBClkFreq;

  // Find NB frequency
  NBClkFreq = (200 * (MemRecNGetBitFieldNb (NBPtr, BFNbFid) + 4)) >> MemRecNGetBitFieldNb (NBPtr, BFNbDid);

  // 1. P = N = T = 0.
  P = N = T = 0;

  // 3. If (D18F2x9C_x0000_0004_dct[1:0][AddrCmdSetup] = 0 & D18F2x9C_x0000_0004_dct[1:0][CsOdt-
  // Setup] = 0 & D18F2x9C_x0000_0004_dct[1:0][CkeSetup] = 0)
  // then P = P + 1
  // else P = P + 2
  P += 2;

  // 4. P = P + (8 - D18F2x210_dct[1:0]_nbp[3:0][RdPtrInit]) + 1
  P = P + (8 - 6) + 1;

  // 6. If (D18F2xA8_dct[1:0][SubMemclkRegDly] = 0 & D18F2x90_dct[1:0][UnbuffDimm] = 0)
  // then P = P + 2
  P += 2;

  // 7. P = P + (2 * (D18F2x200_dct[1:0][Tcl] - 1 clocks))
  P = P + (2 * (6 - 1));

  // 5. P = P + 5
  P += 5;

  // 8. P = P + CEIL(MAX(D18F2x9C_x0000_00[2A:10]_dct[1:0][DqsRcvEnGrossDelay, DqsRcvEnFineDelay] +
  //    D18F2x9C_x0000_0[3:0]0[7:5]_dct[1:0][RdDqsTime] PCLKs))
  P = P + (MaxRcvEnDly / 32) + 2;

  // 9. P = P + 5
  P += 5;

  // 10. T = T + 800 ps
  T += 800;

  // 11. N = (P/(MemClkFreq * 2) + T) * NclkFreq; Convert from PCLKs plus time to NCLKs.
  MemClkPeriod = 1000000 / DDR667_FREQUENCY;
  N = ((((P * MemClkPeriod + 1) / 2) + T) * NBClkFreq + 999999) / 1000000;

  // 12. N = N - 1. See step 9.
  N = N - 1;

  // 13. D18F2x210_dct[1:0]_nbp[3:0][MaxRdLatency] = CEIL(N) - 1
  N = N - 1;

  IDS_HDT_CONSOLE (MEM_FLOW, "NB Freq: %d MHz\n", NBClkFreq);
  IDS_HDT_CONSOLE (MEM_FLOW, "MaxRdLat: %03x\n", N);
  MemRecNSetBitFieldNb (NBPtr, BFMaxLatency, N);
}
