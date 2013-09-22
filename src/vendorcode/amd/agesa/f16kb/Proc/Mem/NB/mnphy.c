/* $NoKeywords:$ */
/**
 * @file
 *
 * mnphy.c
 *
 * Common Northbridge Phy support
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB)
 * @e \$Revision: 87494 $ @e \$Date: 2013-02-04 12:06:47 -0600 (Mon, 04 Feb 2013) $
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


/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */



#include "AGESA.h"
#include "amdlib.h"
#include "Ids.h"
#include "mport.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "mu.h"
#include "PlatformMemoryConfiguration.h"
#include "heapManager.h"
#include "merrhdl.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)
#define FILECODE PROC_MEM_NB_MNPHY_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define UNUSED_CLK 4

/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */
/// Type of an entry for processing phy init compensation for client NB
typedef struct {
  BIT_FIELD_NAME IndexBitField;          ///< Bit field on which the value is decided
  BIT_FIELD_NAME StartTargetBitField;    ///< First bit field to be modified
  BIT_FIELD_NAME EndTargetBitField;      ///< Last bit field to be modified
  UINT16 ExtraValue;                     ///< Extra value needed to be written to bit field
  CONST UINT16 (*TxPrePN)[3][5];         ///< Pointer to slew rate table
} PHY_COMP_INIT_CLIENTNB;

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
 *
 *   This function gets a delay value a PCI register during training
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]   TrnDly - type of delay to be set
 *     @param[in]   DrbnVar - encoding of Dimm-Rank-Byte-Nibble to be accessed
 *                  (use either DIMM_BYTE_ACCESS(dimm,byte) or CS_NBBL_ACCESS(cs,nibble) to use this encoding
 *
 *     @return      Value read
 */

UINT32
MemNGetTrainDlyNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       TRN_DLY_TYPE TrnDly,
  IN       DRBN DrbnVar
  )
{
  return NBPtr->MemNcmnGetSetTrainDly (NBPtr, 0, TrnDly, DrbnVar, 0);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function sets a delay value a PCI register during training
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]   TrnDly - type of delay to be set
 *     @param[in]   DrbnVar - encoding of Dimm-Rank-Byte-Nibble to be accessed
 *                  (use either DIMM_BYTE_ACCESS(dimm,byte) or CS_NBBL_ACCESS(cs,nibble) to use this encoding
 *     @param[in]   Field - Value to be programmed
 *
 */

VOID
MemNSetTrainDlyNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       TRN_DLY_TYPE TrnDly,
  IN       DRBN DrbnVar,
  IN       UINT16 Field
  )
{
  NBPtr->MemNcmnGetSetTrainDly (NBPtr, 1, TrnDly, DrbnVar, Field);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function executes prototypical Phy fence training function.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNPhyFenceTrainingUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 FenceThresholdTxDll;
  UINT8 FenceThresholdRxDll;
  UINT8 FenceThresholdTxPad;
  UINT16 Fence2Data;

  MemNSetBitFieldNb (NBPtr, BFDataFence2, 0);
  MemNSetBitFieldNb (NBPtr, BFFence2, 0);
  // 1. Program D18F2x[1,0]9C_x0000_0008[FenceTrSel]=10b.
  // 2. Perform phy fence training.
  // 3. Write the calculated fence value to D18F2x[1,0]9C_x0000_000C[FenceThresholdTxDll].
  MemNSetBitFieldNb (NBPtr, BFFenceTrSel, 2);
  MAKE_TSEFO (NBPtr->NBRegTable, DCT_PHY_ACCESS, 0x0C, 30, 26, BFPhyFence);
  IDS_HDT_CONSOLE (MEM_FLOW, "\t\tFenceThresholdTxDll\n");
  MemNTrainPhyFenceNb (NBPtr);
  FenceThresholdTxDll = (UINT8) MemNGetBitFieldNb (NBPtr, BFPhyFence);
  NBPtr->FamilySpecificHook[DetectMemPllError] (NBPtr, &FenceThresholdTxDll);

  // 4. Program D18F2x[1,0]9C_x0D0F_0[F,7:0]0F[AlwaysEnDllClks]=001b.
  MemNSetBitFieldNb (NBPtr, BFAlwaysEnDllClks, 0x1000);

  // 5. Program D18F2x[1,0]9C_x0000_0008[FenceTrSel]=01b.
  // 6. Perform phy fence training.
  // 7. Write the calculated fence value to D18F2x[1,0]9C_x0000_000C[FenceThresholdRxDll].
  MemNSetBitFieldNb (NBPtr, BFFenceTrSel, 1);
  MAKE_TSEFO (NBPtr->NBRegTable, DCT_PHY_ACCESS, 0x0C, 25, 21, BFPhyFence);
  IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\tFenceThresholdRxDll\n");
  MemNTrainPhyFenceNb (NBPtr);
  FenceThresholdRxDll = (UINT8) MemNGetBitFieldNb (NBPtr, BFPhyFence);
  NBPtr->FamilySpecificHook[DetectMemPllError] (NBPtr, &FenceThresholdRxDll);

  // 8. Program D18F2x[1,0]9C_x0D0F_0[F,7:0]0F[AlwaysEnDllClks]=000b.
  MemNSetBitFieldNb (NBPtr, BFAlwaysEnDllClks, 0x0000);

  // 9. Program D18F2x[1,0]9C_x0000_0008[FenceTrSel]=11b.
  // 10. Perform phy fence training.
  // 11. Write the calculated fence value to D18F2x[1,0]9C_x0000_000C[FenceThresholdTxPad].
  MemNSetBitFieldNb (NBPtr, BFFenceTrSel, 3);
  MAKE_TSEFO (NBPtr->NBRegTable, DCT_PHY_ACCESS, 0x0C, 20, 16, BFPhyFence);
  IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\tFenceThresholdTxPad\n");
  MemNTrainPhyFenceNb (NBPtr);
  FenceThresholdTxPad = (UINT8) MemNGetBitFieldNb (NBPtr, BFPhyFence);
  NBPtr->FamilySpecificHook[DetectMemPllError] (NBPtr, &FenceThresholdTxPad);

  // Program Fence2 threshold for Clk, Cmd, and Addr
  if (FenceThresholdTxPad < 16) {
    MemNSetBitFieldNb (NBPtr, BFClkFence2, FenceThresholdTxPad | 0x10);
    MemNSetBitFieldNb (NBPtr, BFCmdFence2, FenceThresholdTxPad | 0x10);
    MemNSetBitFieldNb (NBPtr, BFAddrFence2, FenceThresholdTxPad | 0x10);
  } else {
    MemNSetBitFieldNb (NBPtr, BFClkFence2, 0);
    MemNSetBitFieldNb (NBPtr, BFCmdFence2, 0);
    MemNSetBitFieldNb (NBPtr, BFAddrFence2, 0);
  }

  // Program Fence2 threshold for data
  Fence2Data = 0;
  if (FenceThresholdTxPad < 16) {
    Fence2Data |= FenceThresholdTxPad | 0x10;
  }
  if (FenceThresholdRxDll < 16) {
    Fence2Data |= (FenceThresholdRxDll | 0x10) << 10;
  }
  if (FenceThresholdTxDll < 16) {
    Fence2Data |= (FenceThresholdTxDll | 0x10) << 5;
  }
  MemNSetBitFieldNb (NBPtr, BFDataFence2, Fence2Data);
  NBPtr->FamilySpecificHook[ProgramFence2RxDll] (NBPtr, &Fence2Data);

  if (NBPtr->MCTPtr->Status[SbLrdimms]) {
    // 18. If motherboard routing requires CS[7:6] to adopt address timings, e.g. 3 LRDIMMs/ch with CS[7:6]
    // routed across all DIMM sockets, BIOS performs the following:
    if (FindPSOverrideEntry (NBPtr->RefPtr->PlatformMemoryConfiguration, PSO_NO_LRDIMM_CS67_ROUTING, NBPtr->MCTPtr->SocketId, NBPtr->ChannelPtr->ChannelID, 0, NULL, NULL) != NULL) {
      //   A. Program D18F2xA8_dct[1:0][CSTimingMux67] = 1.
      MemNSetBitFieldNb (NBPtr, BFCSTimingMux67, 1);
      //   B. Program D18F2x9C_x0D0F_8021_dct[1:0]:
      //       - DiffTimingEn = 1.
      //       - IF (D18F2x9C_x0000_0004_dct[1:0][AddrCmdFineDelay] >=
      //         D18F2x9C_x0D0F_E008_dct[1:0][FenceValue]) THEN Fence = 1 ELSE Fence = 0.
      //       - Delay = D18F2x9C_x0000_0004_dct[1:0][AddrCmdFineDelay].
      //
      MemNSetBitFieldNb (NBPtr, BFDiffTimingEn, 1);
      MemNSetBitFieldNb (NBPtr, BFFence, (MemNGetBitFieldNb (NBPtr, BFAddrCmdFineDelay) >= MemNGetBitFieldNb (NBPtr, BFFenceValue)) ? 1 : 0);
      MemNSetBitFieldNb (NBPtr, BFDelay, (MemNGetBitFieldNb (NBPtr, BFAddrCmdFineDelay)));
    }
  }

  // 19. Reprogram F2x9C_04.
  MemNSetBitFieldNb (NBPtr, BFAddrTmgControl, MemNGetBitFieldNb (NBPtr, BFAddrTmgControl));

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function executes Phy fence training
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNTrainPhyFenceNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Byte;
  INT16 Avg;
  UINT8 PREvalue;

  if (MemNGetBitFieldNb (NBPtr, BFDisDramInterface)) {
    return;
  }

  // 1. BIOS first programs a seed value to the phase recovery
  //    engine registers.
  //
  IDS_HDT_CONSOLE (MEM_FLOW, "\t\tSeeds: ");
  for (Byte = 0; Byte < MAX_BYTELANES_PER_CHANNEL; Byte++) {
    // This includes ECC as byte 8
    MemNSetTrainDlyNb (NBPtr, AccessPhRecDly, DIMM_BYTE_ACCESS (0, Byte), 19);
    IDS_HDT_CONSOLE (MEM_FLOW, "%02x ", 19);
  }

  IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\tPhyFenceTrEn = 1");
  // 2. Set F2x[1, 0]9C_x08[PhyFenceTrEn]=1.
  MemNSetBitFieldNb (NBPtr, BFPhyFenceTrEn, 1);

  if (!NBPtr->IsSupported[UnifiedNbFence]) {
    // 3. Wait 200 MEMCLKs.
    MemNWaitXMemClksNb (NBPtr, 200);
  } else {
    // 3. Wait 2000 MEMCLKs.
    MemNWaitXMemClksNb (NBPtr, 2000);
  }

  // 4. Clear F2x[1, 0]9C_x08[PhyFenceTrEn]=0.
  MemNSetBitFieldNb (NBPtr, BFPhyFenceTrEn, 0);

  // 5. BIOS reads the phase recovery engine registers
  //    F2x[1, 0]9C_x[51:50] and F2x[1, 0]9C_x52.
  // 6. Calculate the average value of the fine delay and subtract 8.
  //
  Avg = 0;
  IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\t  PRE: ");
  for (Byte = 0; Byte < MAX_BYTELANES_PER_CHANNEL; Byte++) {
    //
    // This includes ECC as byte 8.   ECC Byte lane (8) is ignored by MemNGetTrainDlyNb function where
    // ECC is not supported.
    //
    PREvalue = (UINT8) (0x1F & MemNGetTrainDlyNb (NBPtr, AccessPhRecDly, DIMM_BYTE_ACCESS (0, Byte)));
    Avg = Avg + ((INT16) PREvalue);
    IDS_HDT_CONSOLE (MEM_FLOW, "%02x ", PREvalue);
  }
  Avg = ((Avg + 8) / 9);    // round up

  NBPtr->MemNPFenceAdjustNb (NBPtr, &Avg);

  IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\tFence: %02x\n", Avg);

  // 7. Write the value to F2x[1, 0]9C_x0C[PhyFence].
  MemNSetBitFieldNb (NBPtr, BFPhyFence, Avg);

  // 8. BIOS rewrites F2x[1, 0]9C_x04, DRAM Address/Command Timing Control
  //    Register delays for both channels. This forces the phy to recompute
  //    the fence.
  //
  MemNSetBitFieldNb (NBPtr, BFAddrTmgControl, MemNGetBitFieldNb (NBPtr, BFAddrTmgControl));
}

/*-----------------------------------------------------------------------------*/
/**
 *
 *     Returns the parameters for a requested delay value to be used in training
 *     The correct Min, Max and Mask are determined based on the type of Delay,
 *     and the frequency
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in] TrnDly - Type of delay
 *     @param[in,out] *Parms - Pointer to the TRN_DLY-PARMS struct
 *
 */

VOID
MemNGetTrainDlyParmsUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       TRN_DLY_TYPE TrnDly,
  IN OUT   TRN_DLY_PARMS *Parms
  )
{
  Parms->Min = 0;

  if ((TrnDly == AccessWrDatDly) || (TrnDly == AccessRdDqsDly)) {
    Parms->Max = 0x1F;
    Parms->Mask = 0x01F;
  }
}
/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
/* -----------------------------------------------------------------------------*/
/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function gets or set DQS timing during training.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]   TrnDly - type of delay to be set
 *     @param[in]   DrbnVar - encoding of Dimm-Rank-Byte-Nibble to be accessed
 *                  (use either DIMM_BYTE_ACCESS(dimm,byte) or CS_NBBL_ACCESS(cs,nibble) to use this encoding
 *     @param[in]   Field - Value to be programmed
 *     @param[in]   IsSet - Indicates if the function will set or get
 *
 *     @return      value read, if the function is used as a "get"
 */

UINT32
MemNcmnGetSetTrainDlyUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 IsSet,
  IN       TRN_DLY_TYPE TrnDly,
  IN       DRBN DrbnVar,
  IN       UINT16 Field
  )
{
  UINT16 Index;
  UINT16 Offset;
  UINT32 Value;
  UINT32 Address;
  UINT8 Dimm;
  UINT8 Rank;
  UINT8 Byte;
  UINT8 Nibble;
  UINT8 DimmNibble;

  Dimm = DRBN_DIMM (DrbnVar);
  Rank = DRBN_RANK (DrbnVar);
  Byte = DRBN_BYTE (DrbnVar);
  Nibble = DRBN_NBBL (DrbnVar);
  DimmNibble = DRBN_DIMM_NBBL (DrbnVar);

  ASSERT (Dimm < (NBPtr->CsPerChannel / NBPtr->CsPerDelay));
  ASSERT (Byte <= ECC_DLY);
  if ((Byte == ECC_DLY) && (!NBPtr->MCTPtr->Status[SbEccDimms] || !NBPtr->IsSupported[EccByteTraining])) {
    // When ECC is not enabled
    if (IsSet) {
      // On write, ignore
      return 0;
    } else {
      // On read, redirect to byte 0 to correct fence averaging
      Byte = 0;
    }
  }

  switch (TrnDly) {
  case AccessRcvEnDly:
    Index = 0x10;
    break;
  case AccessWrDqsDly:
    Index = 0x30;
    break;
  case AccessWrDatDly:
    Index = 0x01;
    break;
  case AccessRdDqsDly:
    Index = 0x05;
    break;
  case AccessRdDqs2dDly:
    Index = 0x00;
    break;
  case AccessPhRecDly:
    Index = 0x50;
    break;
  default:
    Index = 0;
    IDS_ERROR_TRAP;
  }

  switch (TrnDly) {
  case AccessRcvEnDly:
  case AccessWrDqsDly:
    Index += (Dimm * 3);
    if (Byte & 0x04) {
      // if byte 4,5,6,7
      Index += 0x10;
    }
    if (Byte & 0x02) {
      // if byte 2,3,6,7
      Index++;
    }
    if (Byte > 7) {
      Index += 2;
    }
    Offset = 16 * (Byte % 2);
    Index |= (Rank << 8);
    Index |= (Nibble << 9);
    Address = Index;
    break;

  case AccessRdDqsDly:
  case AccessWrDatDly:

    if (NBPtr->IsSupported[DimmBasedOnSpeed]) {
      if (NBPtr->DCTPtr->Timings.Speed < DDR800_FREQUENCY) {
        // if DDR speed is below 800, use DIMM 0 delays for all DIMMs.
        Dimm = 0;
      }
    }

    Index += (Dimm * 0x100);
    if (Nibble) {
      if (Rank) {
        Index += 0xA0;
      } else {
        Index += 0x70;
      }
    } else if (Rank) {
      Index += 0x60;
    }
    // break is not being used here because AccessRdDqsDly and AccessWrDatDly also need
    // to run AccessPhRecDly sequence.
  case AccessPhRecDly:
    Index += (Byte / 4);
    Offset = 8 * (Byte % 4);
    Address = Index;
    break;
  case AccessRdDqs2dDly:
    Address = 0x0D0F0000;
    Index += (DimmNibble >> 1) * 0x100;
    Index += 0x20;
    Index = Index + Dimm;
    Offset = 4 * ((DimmNibble & 0x01) * 2);
    Address += Index;
    break;
  default:
    Offset = 0;
    IDS_ERROR_TRAP;
    Address = Index;
  }
  MemNSetBitFieldNb (NBPtr, BFDctAddlOffsetReg, Address);
  MemNPollBitFieldNb (NBPtr, BFDctAccessDone, 1, PCI_ACCESS_TIMEOUT, FALSE);
  Value = MemNGetBitFieldNb (NBPtr, BFDctAddlDataReg);
  if (TrnDly == AccessRdDqsDly) {
    NBPtr->FamilySpecificHook[AdjustRdDqsDlyOffset] (NBPtr, &Offset);
  }

  if (IsSet) {
    if (TrnDly == AccessPhRecDly) {
      Value = NBPtr->DctCachePtr->PhRecReg[Index & 0x03];
    }
    if (TrnDly != AccessRdDqs2dDly) {
      Value = ((UINT32)Field << Offset) | (Value & (~((UINT32) ((TrnDly == AccessRcvEnDly) ? 0x3FF : 0xFF) << Offset)));
    } else {
      Value = ((UINT32)Field << Offset) | (Value & (~((UINT32) 0x1F << Offset)));
    }
    ASSERT (!NBPtr->IsSupported[ScrubberEn]);   // Phy CSR write is not allowed after scrubber is enabled
    MemNSetBitFieldNb (NBPtr, BFDctAddlDataReg, Value);
    Address |= DCT_ACCESS_WRITE;
    MemNSetBitFieldNb (NBPtr, BFDctAddlOffsetReg, Address);
    MemNPollBitFieldNb (NBPtr, BFDctAccessDone, 1, PCI_ACCESS_TIMEOUT, FALSE);
    if (TrnDly == AccessPhRecDly) {
      NBPtr->DctCachePtr->PhRecReg[Index & 0x03] = Value;
    }
  } else {
    if (TrnDly != AccessRdDqs2dDly) {
      Value = (Value >> Offset) & (UINT32) ((TrnDly == AccessRcvEnDly) ? 0x3FF : 0xFF);
    } else {
      Value = (Value >> Offset) & (UINT32) (0x1F);
    }
  }
  return Value;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function initializes the training pattern.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return      AGESA_STATUS - Result
 *                  AGESA_SUCCESS - Training pattern is ready to use
 *                  AGESA_ERROR   - Unable to initialize the pattern.
 */

AGESA_STATUS
MemNTrainingPatternInitNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  MEM_TECH_BLOCK *TechPtr;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;
  TRAIN_PATTERN TrainPattern;
  AGESA_STATUS Status;

  TechPtr = NBPtr->TechPtr;
  TrainPattern = 0;
  //
  // Check the training type
  //
  if (TechPtr->TrainingType == TRN_DQS_POSITION) {
    //
    // DQS Position Training
    //
    if (NBPtr->PosTrnPattern == POS_PATTERN_256B) {
      //
      // 256 Bit pattern
      //
      if (NBPtr->MCTPtr->Status[Sb128bitmode]) {
        TrainPattern = TestPatternJD256B;
        TechPtr->PatternLength = 64;
      } else {
        TrainPattern = TestPatternJD256A;
        TechPtr->PatternLength = 32;
      }
    } else {
      //
      // 72 bit pattern will be used if PosTrnPattern is not specified
      //
      if (NBPtr->MCTPtr->Status[Sb128bitmode]) {
        TrainPattern = TestPatternJD1B;
        TechPtr->PatternLength = 18;
      } else {
        TrainPattern = TestPatternJD1A;
        TechPtr->PatternLength = 9;
      }
    }
  } else if (TechPtr->TrainingType == TRN_MAX_READ_LATENCY) {
    //
    // Max Read Latency Training
    //
    TrainPattern = TestPatternML;
    TechPtr->PatternLength = (NBPtr->MCTPtr->Status[Sb128bitmode]) ? 6 : 3;
  } else {
    //
    // Error - TechPtr->Training Type must be set to one of the types handled in this function
    //
    ASSERT (FALSE);
  }
  //
  // Allocate training buffer
  //
  AllocHeapParams.RequestedBufferSize = (TechPtr->PatternLength * 64 * 2) + 16;
  AllocHeapParams.BufferHandle = AMD_MEM_TRAIN_BUFFER_HANDLE;
  AllocHeapParams.Persist = HEAP_LOCAL_CACHE;
  Status = HeapAllocateBuffer (&AllocHeapParams, &NBPtr->MemPtr->StdHeader);
  ASSERT (Status == AGESA_SUCCESS);
  if (Status != AGESA_SUCCESS) {
    return Status;
  }
  TechPtr->PatternBufPtr = AllocHeapParams.BufferPtr;
  AlignPointerTo16Byte (&TechPtr->PatternBufPtr);
  TechPtr->TestBufPtr = TechPtr->PatternBufPtr + (TechPtr->PatternLength * 64);

  // Prepare training pattern
  MemUFillTrainPattern (TrainPattern, TechPtr->PatternBufPtr, TechPtr->PatternLength * 64);

  return Status;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function determined the settings for the Reliable Read/Write engine
 *       for each specific type of training
 *
 *     @param[in,out]   *NBPtr      - Pointer to the MEM_NB_BLOCK
 *     @param[in]       *OptParam   - Pointer to an Enum of TRAINING_TYPE
 *
 *     @return      TRUE
 */

BOOLEAN
MemNSetupHwTrainingEngineUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       VOID *OptParam
  )
{
  TRAINING_TYPE TrnType;
  RRW_SETTINGS *Rrw;

  TrnType = *(TRAINING_TYPE*) OptParam;
  Rrw = &NBPtr->RrwSettings;
  //
  // Common Settings
  //
  Rrw->TgtBankAddressA = CPG_BANK_ADDRESS_A;
  Rrw->TgtRowAddressA = CPG_ROW_ADDRESS_A;
  Rrw->TgtColAddressA = CPG_COL_ADDRESS_A;
  Rrw->TgtBankAddressB = CPG_BANK_ADDRESS_B;
  Rrw->TgtRowAddressB = CPG_ROW_ADDRESS_B;
  Rrw->TgtColAddressB = CPG_COL_ADDRESS_B;
  Rrw->CompareMaskHigh = CPG_COMPARE_MASK_HI;
  Rrw->CompareMaskLow = CPG_COMPARE_MASK_LOW;
  Rrw->CompareMaskEcc = CPG_COMPARE_MASK_ECC;

  switch (TrnType) {
  case TRN_RCVR_ENABLE:
    //
    // Receiver Enable Training
    //
    NBPtr->TechPtr->PatternLength = 192;
    break;
  case TRN_MAX_READ_LATENCY:
    //
    // Max Read Latency Training
    //
    Rrw->CmdTgt = CMD_TGT_A;
    NBPtr->TechPtr->PatternLength = 32;
    Rrw->DataPrbsSeed = PRBS_SEED_32;
    break;
  case TRN_DQS_POSITION:
    //
    // Read/Write DQS Position training
    //
    Rrw->CmdTgt = CMD_TGT_AB;
    NBPtr->TechPtr->PatternLength = 256;
    Rrw->DataPrbsSeed = PRBS_SEED_256;
    break;
  default:
    ASSERT (FALSE);
  }
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function finalizes the training pattern.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       Index    - Index of Write Data Delay Value
 *     @param[in,out]   *Value   - Write Data Delay Value
 *     @return      BOOLEAN - TRUE - Use the value returned.
 *                            FALSE - No more values in table.
 */

BOOLEAN
MemNGetApproximateWriteDatDelayNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Index,
  IN OUT   UINT8 *Value
  )
{
  CONST UINT8 WriteDatDelayValue[] = {0x10, 0x4, 0x8, 0xC, 0x14, 0x18, 0x1C, 0x1F};
  if (Index < GET_SIZE_OF (WriteDatDelayValue)) {
    *Value = WriteDatDelayValue[Index];
    return TRUE;
  }
  return FALSE;
}


/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function finalizes the training pattern.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return      AGESA_STATUS - Result
 *                  AGESA_SUCCESS - Training pattern has been finalized.
 *                  AGESA_ERROR   - Unable to initialize the pattern.
 */

AGESA_STATUS
MemNTrainingPatternFinalizeNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  AGESA_STATUS Status;
  //
  // Deallocate training buffer
  //
  Status = HeapDeallocateBuffer (AMD_MEM_TRAIN_BUFFER_HANDLE, &NBPtr->MemPtr->StdHeader);
  ASSERT (Status == AGESA_SUCCESS);
  return Status;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function returns the number of Chipselects controlled by each set
 *     of Delay registers under current conditions.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return
 */

UINT8
MemNCSPerDelayNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  if (MemNGetBitFieldNb (NBPtr, BFPerRankTimingEn) == 1) {
    return 1;
  } else {
    return 2;
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function returns the minimum data eye width in 32nds of a UI for
 *     the type of data eye(Rd/Wr) that is being trained.  This value will
 *     be the minimum number of consecutive delays that yield valid data.
 *     Uses TechPtr->Direction to determine read or write.
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return
 */

UINT8
MemNMinDataEyeWidthNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 MinRdDataeye;
  UINT8 MinWrDataeye;
  UINT8 *MinRdWrDataeyePtr;

  MinRdWrDataeyePtr = FindPSOverrideEntry (NBPtr->RefPtr->PlatformMemoryConfiguration, PSO_MIN_RD_WR_DATAEYE_WIDTH, NBPtr->MCTPtr->SocketId, NBPtr->ChannelPtr->ChannelID, 0,
                                           &(NBPtr->MCTPtr->LogicalCpuid), &(NBPtr->MemPtr->StdHeader));

  if (NBPtr->TechPtr->Direction == DQS_READ_DIR) {
    if (MinRdWrDataeyePtr != NULL) {
      MinRdDataeye = MinRdWrDataeyePtr[0];
      return MinRdDataeye;
    } else {
      return MIN_RD_DATAEYE_WIDTH_NB;
    }
  } else {
    if (MinRdWrDataeyePtr != NULL) {
      MinWrDataeye = MinRdWrDataeyePtr[1];
      return MinWrDataeye;
    } else {
      return MIN_WR_DATAEYE_WIDTH_NB;
    }
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function programs the phy registers according to the desired phy VDDIO voltage level
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNPhyVoltageLevelNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  BIT_FIELD_NAME BitField;
  BIT_FIELD_NAME BFEnd;
  UINT16 BFValue;
  UINT16 RegValue;

  BFValue = (UINT16) CONVERT_VDDIO_TO_ENCODED (NBPtr->RefPtr->DDR3Voltage) << 3;
  BFEnd = NBPtr->IsSupported[ProgramCsrComparator] ? BFCsrComparator : BFCmpVioLvl;

  for (BitField = BFDataRxVioLvl; BitField <= BFEnd; BitField++) {
    RegValue = BFValue;
    if (BitField == BFCsrComparator) {
      RegValue >>= (3 - 2);
      // Setting this bit in DCT0 adjusts the comparator for DCT0 and DCT1. Setting this bit in DCT1 has no effect.
      NBPtr->SwitchDCT (NBPtr, 0);
      MemNSetBitFieldNb (NBPtr, BitField, RegValue);
      break;
    } else if (BitField == BFCmpVioLvl) {
      RegValue <<= (14 - 3);
      // Must set this bit on DCT0 even when DCT0 has no memory
      NBPtr->SwitchDCT (NBPtr, 0);
      MemNSetBitFieldNb (NBPtr, BitField, RegValue);
    }
    MemNBrdcstSetNb (NBPtr, BitField, RegValue);
  }
  // Program D18F2x9C_x0D0F_4006_dct[0][VrefSel] = 0.
  MemNSetBitFieldNb (NBPtr, BFObsrvVrefSel, 0);
  // Program D18F2x9C_x0D0F_4007_dct[0] per platform requirements.
  NBPtr->FamilySpecificHook[PhyInitVref] (NBPtr, NULL);
}

/*-----------------------------------------------------------------------------
 *
 *
 *     This function calculates the value of WrDqDqsEarly and programs it into
 *       the DCT and adds it to the WrDqsGrossDelay of each byte lane on each
 *       DIMM of the channel.
 *
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]  OptParam   - Optional parameter
 *
 *     @return    TRUE
 * ----------------------------------------------------------------------------
 */
BOOLEAN
MemNCalcWrDqDqsEarlyUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  MEM_TECH_BLOCK *TechPtr;
  DCT_STRUCT *DCTPtr;
  CH_DEF_STRUCT *ChannelPtr;
  UINT8 Dimm;
  UINT8 ByteLane;
  UINT8 *WrDqsDlysPtr;
  UINT8 WrDqDqsEarly;
  UINT8 ChipSel;

  ASSERT ((NBPtr->IsSupported[WLSeedAdjust]) && (NBPtr->IsSupported[WLNegativeDelay]));

  TechPtr = NBPtr->TechPtr;
  ChannelPtr = NBPtr->ChannelPtr;
  DCTPtr = NBPtr->DCTPtr;

  ASSERT (NBPtr != NULL);
  ASSERT (ChannelPtr != NULL);
  ASSERT (DCTPtr != NULL);
  //
  // For each DIMM:
  // - The Critical Gross Delay (CGD) is the minimum GrossDly of all byte lanes and all DIMMs.
  // - If (CGD < 0) Then
  // -   D18F2xA8_dct[1:0][WrDqDqsEarly] = ABS(CGD)
  // -   WrDqsGrossDly = GrossDly + WrDqDqsEarly
  // - Else
  // -   D18F2xA8_dct[1:0][WrDqDqsEarly] = 0.
  // -   WrDqsGrossDly = GrossDly
  //
  WrDqDqsEarly = 0;
  if (TechPtr->WLCriticalDelay < 0) {
    IDS_HDT_CONSOLE (MEM_FLOW, "\t\tCalculating WrDqDqsEarly, adjusting WrDqs.\n");
    IDS_HDT_CONSOLE (MEM_FLOW, "\t\tMin. Critical Delay: %x\n", TechPtr->WLCriticalDelay);
    // We've saved the entire negative delay value, so take the ABS and convert to GrossDly.
    WrDqDqsEarly =  (UINT8) (0x00FF &((((ABS (TechPtr->WLCriticalDelay)) + 0x1F) / 0x20)));
    IDS_HDT_CONSOLE (MEM_FLOW, "\t\tWrDqDqsEarly : %02x\n\n", WrDqDqsEarly);
    //
    // Loop through All WrDqsDlys on all DIMMs
    //
    for (ChipSel = 0; ChipSel < NBPtr->CsPerChannel; ChipSel = ChipSel + NBPtr->CsPerDelay) {
      if ((NBPtr->MCTPtr->Status[SbLrdimms]) ? ((NBPtr->ChannelPtr->LrDimmPresent & ((UINT8) 1 << (ChipSel >> 1))) != 0) :
          ((DCTPtr->Timings.CsEnabled & ((UINT16) ((NBPtr->CsPerDelay == 2)? 3 : 1) << ChipSel)) != 0)) {
        //
        // If LRDIMMs, only include the physical dimms, not logical Dimms
        //
        IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tCS %x:", ChipSel);
        Dimm = ChipSel / NBPtr->CsPerDelay;

        WrDqsDlysPtr = &(ChannelPtr->WrDqsDlys[(Dimm * TechPtr->DlyTableWidth ())]);
        for (ByteLane = 0; ByteLane < (NBPtr->MCTPtr->Status[SbEccDimms] ? 9 : 8); ByteLane++) {
          WrDqsDlysPtr[ByteLane] += (WrDqDqsEarly << 5);
          NBPtr->SetTrainDly (NBPtr, AccessWrDqsDly, DIMM_BYTE_ACCESS (Dimm, ByteLane), WrDqsDlysPtr[ByteLane]);
          IDS_HDT_CONSOLE (MEM_FLOW, " %02x", WrDqsDlysPtr[ByteLane]);
        }
        IDS_HDT_CONSOLE (MEM_FLOW, "\n");
      }
    }
  }
  MemNSetBitFieldNb (NBPtr, BFWrDqDqsEarly, WrDqDqsEarly);
  return TRUE;
}

/*-----------------------------------------------------------------------------
 *
 *
 *     This function forces phy to M0 state
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *
 *     @return  none
 * ----------------------------------------------------------------------------
 */
VOID
MemNForcePhyToM0Unb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  IDS_SKIP_HOOK (IDS_FORCE_PHY_TO_M0, NBPtr, &(NBPtr->MemPtr->StdHeader)) {
    // 1. Program D18F2x9C_x0D0F_E013_dct[1:0] = 0118h.
    MemNBrdcstSetUnConditionalNb (NBPtr, BFPllRegWaitTime, 0x118);
    // 2. For each byte lane and each memory P-state: Program D18F2x9C_x0D0F_0[F,7:0][53,13]_dct[1:0][RxSsbMntClkEn] = 0.
    MemNBrdcstSetUnConditionalNb (NBPtr, BFRxSsbMntClkEn, 0);
    // 3. D18F2xA8_dct[1:0][MemPhyPllPdMode] = 00b.
    MemNBrdcstSetUnConditionalNb (NBPtr, BFMemPhyPllPdMode, 0);
    // 4. Force the phy to M0 with the following sequence:
    // A. Program D18F2x9C_x0D0F_E006_dct[1:0][PllLockTime] = 190h. Restore the default PLL lock time.
    MemNBrdcstSetUnConditionalNb (NBPtr, BFPllLockTime, NBPtr->FreqChangeParam->PllLockTimeDefault);
    // B. For each DCT: Program D18F2x9C_x0000_000B_dct[1:0] = 80800000h.
    MemNBrdcstSetUnConditionalNb (NBPtr, BFDramPhyStatusReg, 0x80800000);
    NBPtr->SwitchDCT (NBPtr, 0);
    // C. Program D18F2x9C_x0D0F_E018_dct[0][PhyPSMasterChannel] = 0.
    MemNSetBitFieldNb (NBPtr, BFPhyPSMasterChannel, 0);
    // D. Program D18F2x9C_x0000_000B_dct[0] = 40000000h. CH0 only;
    MemNSetBitFieldNb (NBPtr, BFDramPhyStatusReg, 0x40000000);
    // E. For each DCT: Program D18F2x9C_x0000_000B_dct[1:0] = 80000000h.
    MemNBrdcstSetUnConditionalNb (NBPtr, BFDramPhyStatusReg, 0x80000000);
  }
}

/*-----------------------------------------------------------------------------
 *
 *
 *     This function sets SkewMemClk before enabling MemClk
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]  *OptParam   - Optional parameter
 *
 *     @return  TRUE - always
 * ----------------------------------------------------------------------------
 */
BOOLEAN
MemNSetSkewMemClkUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  UINT8 Dct;

  // SkewMemClk is set to 1 if all DCTs are enabled, else 0.
  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    MemNSwitchDCTNb (NBPtr, Dct);
    if (NBPtr->DCTPtr->Timings.DctMemSize == 0) {
    break;
    }
  }
  MemNSwitchDCTNb (NBPtr, 0);
  if (Dct == NBPtr->DctCount) {
    MemNSetBitFieldNb (NBPtr, BFSkewMemClk, 0x10);
  } else {
    MemNSetBitFieldNb (NBPtr, BFSkewMemClk, 0);
  }

  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function masks the RdDqsDly Bit 0 before writing to register for UNB.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *Offset -  Bit offset of the field to be programmed
 *
 *     @return    TRUE
 */
BOOLEAN
MemNAdjustRdDqsDlyOffsetUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *Offset
  )
{
  *(UINT16*) Offset = *(UINT16*) Offset + 1;
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *     This function initializes RxEn Delays for RxEn seedless training
 *
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]  OptParam   - Optional parameter
 *
 *     @return    TRUE
 * ----------------------------------------------------------------------------
 */
BOOLEAN
MemNInitializeRxEnSeedlessTrainingUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  UINT8 ByteLane;
  // Save original PRE based RxEnDly for RxEn Seedless training
  for (ByteLane = 0; ByteLane < (NBPtr->MCTPtr->Status[SbEccDimms] ? 9 : 8); ByteLane++) {
    NBPtr->TechPtr->RxOrig[ByteLane] = NBPtr->ChannelPtr->RcvEnDlys[(NBPtr->TechPtr->ChipSel / NBPtr->CsPerDelay) * NBPtr->TechPtr->DlyTableWidth () + ByteLane];
  }
  return TRUE;
}
/*-----------------------------------------------------------------------------
 *
 *
 *     This function checks each bytelane for no window error.
 *
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   OptParam   - Optional parameter
 *
 *     @return    TRUE
 * ----------------------------------------------------------------------------
 */
BOOLEAN
MemNTrackRxEnSeedlessRdWrNoWindBLErrorUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  MemTTrackRxEnSeedlessRdWrNoWindBLError (NBPtr->TechPtr, OptParam);
  return TRUE;
}
/*-----------------------------------------------------------------------------
 *
 *
 *      This function checks each bytelane for small window error.
 *
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   OptParam   - Optional parameter
 *
 *     @return    TRUE
 * ----------------------------------------------------------------------------
 */
BOOLEAN
MemNTrackRxEnSeedlessRdWrSmallWindBLErrorUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  MemTTrackRxEnSeedlessRdWrSmallWindBLError (NBPtr->TechPtr, OptParam);
  return TRUE;
}
/*-----------------------------------------------------------------------------
 *
 *
 *      This function initializes a ByteLaneError error.
 *
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   OptParam   - Optional parameter
 *
 *     @return    TRUE
 * ----------------------------------------------------------------------------
 */
BOOLEAN
MemNInitialzeRxEnSeedlessByteLaneErrorUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  UINT8 ByteLane;
  for (ByteLane = 0; ByteLane < (NBPtr->MCTPtr->Status[SbEccDimms] ? 9 : 8); ByteLane++) {
    NBPtr->TechPtr->ByteLaneError[ByteLane] = FALSE; // All Bytelanes have no errors
  }
  return TRUE;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *     This function sets phy power saving related settings in different MPstate context.
 *
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *
 *     @return    none
 * ----------------------------------------------------------------------------
 */
VOID
MemNPhyPowerSavingMPstateUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  STATIC UINT8 Sequence[] = {8, 4, 3, 5, 2, 6, 1, 7, 0};
  UINT16 DllPower[9];
  UINT8 NumLanes;
  UINT8 DllWakeTime;
  UINT8 MaxRxStggrDly;
  UINT8 MinRcvEnGrossDly;
  UINT8 MinWrDatGrossDly;
  UINT8 dRxStggrDly;
  UINT8 dTxStggrDly;
  UINT8 TempStggrDly;
  UINT8 MaxTxStggrDly;
  UINT8 Tcwl;
  UINT8 i;
  UINT16 MemClkSpeed;
  MemClkSpeed = ( (NBPtr->MemPstate == MEMORY_PSTATE0) ? NBPtr->DCTPtr->Timings.Speed : MemNGetMemClkFreqUnb (NBPtr, (UINT8) MemNGetBitFieldNb (NBPtr, BFM1MemClkFreq)) );

  IDS_HDT_CONSOLE (MEM_FLOW, "Start Phy power saving setting for memory Pstate %d\n", NBPtr->MemPstate);
  // 4. Program D18F2x9C_x0D0F_0[F,8:0]13_dct[1:0][DllDisEarlyU] = 1b.
  // 5. Program D18F2x9C_x0D0F_0[F,8:0]13_dct[1:0][DllDisEarlyL] = 1b.
  // 6. D18F2x9C_x0D0F_0[F,7:0][53,13]_dct[1:0][RxDqsUDllPowerDown] = 1.
  MemNSetBitFieldNb (NBPtr, BFPhy0x0D0F0F13, MemNGetBitFieldNb (NBPtr, BFPhy0x0D0F0F13) | 0x83);
  // 7. D18F2x9C_x0D0F_812F_dct[1:0][PARTri] = ~D18F2x90_dct[1:0][ParEn].
  // 8. D18F2x9C_x0D0F_812F_dct[1:0][Add17Tri, Add16Tri] = {1b, 1b}
  if (NBPtr->MemPstate == MEMORY_PSTATE0) {
    MemNSetBitFieldNb (NBPtr, BFAddrCmdTri, MemNGetBitFieldNb (NBPtr, BFAddrCmdTri) | 0xA1);
  }
  // 9. IF (DimmsPopulated == 1)&& ((D18F2x9C_x0000_0000_dct[1:0]_mp[1:0][CkeDrvStren]==010b) ||
  // (D18F2x9C_x0000_0000_dct[1:0]_mp[1:0][CkeDrvStren]==011b)) THEN THEN
  // program D18F2x9C_x0D0F_C0[40,00]_dct[1:0][LowPowerDrvStrengthEn] = 1
  // ELSE program D18F2x9C_x0D0F_C0[40,00]_dct[1:0][LowPowerDrvStrengthEn] = 0 ENDIF.
  if ((NBPtr->ChannelPtr->Dimms == 1) && ((MemNGetBitFieldNb (NBPtr, BFCkeDrvStren) == 2) || (MemNGetBitFieldNb (NBPtr, BFCkeDrvStren) == 3))) {
    MemNSetBitFieldNb (NBPtr, BFLowPowerDrvStrengthEn, 0x100);
  }
  // 10. Program D18F2x9C_x0D0F_0[F,7:0][50,10]_dct[1:0][EnRxPadStandby] = IF
  // (D18F2x94_dct[1:0][MemClkFreq] <= 800 MHz) THEN 1 ELSE 0 ENDIF.
  MemNSetBitFieldNb (NBPtr, BFEnRxPadStandby, (MemClkSpeed <= DDR1600_FREQUENCY) ? 0x1000 : 0);
  // 11. Program D18F2x9C_x0000_000D_dct[1:0]_mp[1:0] as follows:
  // If (DDR rate < = 1600) TxMaxDurDllNoLock = RxMaxDurDllNoLock = 8h
  // else TxMaxDurDllNoLock = RxMaxDurDllNoLock = 7h.
  if (MemClkSpeed <= DDR1600_FREQUENCY) {
    MemNSetBitFieldNb (NBPtr, BFTxMaxDurDllNoLock, 8);
    MemNSetBitFieldNb (NBPtr, BFRxMaxDurDllNoLock, 8);
  } else {
    MemNSetBitFieldNb (NBPtr, BFTxMaxDurDllNoLock, 7);
    MemNSetBitFieldNb (NBPtr, BFRxMaxDurDllNoLock, 7);
  }
  // TxCPUpdPeriod = RxCPUpdPeriod = 011b.
  MemNSetBitFieldNb (NBPtr, BFTxCPUpdPeriod, 3);
  MemNSetBitFieldNb (NBPtr, BFRxCPUpdPeriod, 3);
  // TxDLLWakeupTime = RxDLLWakeupTime = 11b.
  MemNSetBitFieldNb (NBPtr, BFTxDLLWakeupTime, 3);
  MemNSetBitFieldNb (NBPtr, BFRxDLLWakeupTime, 3);

  if (NBPtr->IsSupported[DllStaggerEn]) {
    // 12. Program D18F2x9C_x0D0F_0[F,7:0][5C,1C]_dct[1:0] as follows.
    // Let Numlanes = 8. = 9 with ECC.
    NumLanes = (NBPtr->MCTPtr->Status[SbEccDimms] && NBPtr->IsSupported[EccByteTraining]) ? 9 : 8;
    // RxDllStggrEn = TxDllStggrEn = 1.
    for (i = 0; i < 9; i ++) {
      DllPower[i] = 0x8080;
    }
    // If (DDR rate > = 1866) DllWakeTime = 1, Else DllWakeTime = 0.
    DllWakeTime = (MemClkSpeed >= DDR1866_FREQUENCY) ? 1 : 0;
    // Let MaxRxStggrDly = (Tcl*2) + MIN(DqsRcvEnGrossDelay for all byte lanes (see D18F2x9C_x0000_00[2A:10]_dct[1:0]_mp[1:0])) - 4.
    MinRcvEnGrossDly = NBPtr->TechPtr->GetMinMaxGrossDly (NBPtr->TechPtr, AccessRcvEnDly, FALSE);
    ASSERT ((NBPtr->DCTPtr->Timings.CasL * 2 + MinRcvEnGrossDly) >= 4);
    MaxRxStggrDly = NBPtr->DCTPtr->Timings.CasL * 2 + MinRcvEnGrossDly - 4;
    // Let (real) dRxStggrDly = (MaxRxStggrDly - DllWakeTime) / (Numlanes - 1).
    ASSERT (MaxRxStggrDly >= DllWakeTime);
    dRxStggrDly = (MaxRxStggrDly - DllWakeTime) / (NumLanes - 1);
    IDS_HDT_CONSOLE (MEM_FLOW, "\tMinimum RcvEnGrossDly: 0x%02x MaxRxStggrDly: 0x%02x dRxStggrDly: 0x%02x\n", MinRcvEnGrossDly, MaxRxStggrDly, dRxStggrDly);
    // For each byte lane in the ordered sequence {8, 4, 3, 5, 2, 6, 1, 7, 0}, program RxDllStggrDly[5:0] = an
    // increasing value, starting with 0 for the first byte lane in the sequence and increasing at a rate of dRxStggrDly
    // for each subsequent byte lane. Convert the real to integer by rounding down or using C (int) typecast after linearization.
    i = 9 - NumLanes;
    TempStggrDly = 0;
    for (; i < 9; i ++) {
      DllPower[Sequence[i]] |= ((TempStggrDly & 0x3F) << 8);
      TempStggrDly = TempStggrDly + dRxStggrDly;
    }

    // Let MaxTxStggrDly = (Tcwl*2) + MIN(MIN (WrDatGrossDly for all byte lanes (see
    // D18F2x9C_x0000_0[3:0]0[2:1]_dct[1:0]_mp[1:0])), MIN(DqsRcvEnGrossDelay for all byte lanes (see
    // D18F2x9C_x0000_00[2A:10]_dct[1:0]_mp[1:0])) - 4.
    Tcwl = (UINT8) MemNGetBitFieldNb (NBPtr, BFTcwl);
    MinWrDatGrossDly = NBPtr->TechPtr->GetMinMaxGrossDly (NBPtr->TechPtr, AccessWrDatDly, FALSE);
    MaxTxStggrDly = Tcwl * 2 + MIN (MinRcvEnGrossDly, MinWrDatGrossDly) - 4;
    // Let dTxStggrDly = (MaxTxStggrDly - DllWakeTime) / (Numlanes - 1).
    ASSERT (MaxTxStggrDly >= DllWakeTime);
    dTxStggrDly = (MaxTxStggrDly - DllWakeTime) / (NumLanes - 1);
    // For each byte lane in the ordered sequence {8, 4, 3, 5, 2, 6, 1, 7, 0}, program TxDllStggrDly[5:0] = an
    // increasing integer value, starting with 0 for the first byte lane in the sequence and increasing at a rate of
    // dTxStggrDly for each subsequent byte lane.
    IDS_HDT_CONSOLE (MEM_FLOW, "\tMinimum WrDatGrossDly: 0x%02x MaxTxStggrDly: 0x%02x dTxStggrDly: 0x%02x\n", MinWrDatGrossDly, MaxTxStggrDly, dTxStggrDly);
    i = 9 - NumLanes;
    TempStggrDly = 0;
    for (; i < 9; i ++) {
      DllPower[Sequence[i]] |= (TempStggrDly & 0x3F);
      TempStggrDly = TempStggrDly + dTxStggrDly;
    }

    IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\t\tByte Lane    :  ECC   07   06   05   04   03   02   01   00\n");
    IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\t\tDll Power    : %04x %04x %04x %04x %04x %04x %04x %04x %04x\n",
                     DllPower[8], DllPower[7], DllPower[6], DllPower[5], DllPower[4], DllPower[3], DllPower[2], DllPower[1], DllPower[0]);

    for (i = 0; i < NumLanes; i ++) {
      MemNSetBitFieldNb (NBPtr, BFDataByteDllPowerMgnByte0 + i, (MemNGetBitFieldNb (NBPtr, BFDataByteDllPowerMgnByte0 + i) & 0x4040) | DllPower[i]);
    }
  }
  // 13. Program D18F2x248_dct[1:0]_mp[1:0] and then D18F2x9C_x0D0F_0[F,7:0][53,13]_dct[1:0] as follows:
  // For M1 context program RxChMntClkEn=RxSsbMntClkEn=0.
  // For M0 context program RxChMntClkEn=RxSsbMntClkEn=1.
  if (NBPtr->MemPstate == MEMORY_PSTATE1 || NBPtr->IsSupported[RxChnMntClksEn] == FALSE) {
    MemNSetBitFieldNb (NBPtr, BFRxChMntClkEn, 0);
    MemNSetBitFieldNb (NBPtr, BFRxSsbMntClkEn, 0);
  } else {
    MemNSetBitFieldNb (NBPtr, BFRxChMntClkEn, 1);
    MemNSetBitFieldNb (NBPtr, BFRxSsbMntClkEn, 0x100);
  }

  IDS_OPTION_HOOK (IDS_PHY_DLL_STANDBY_CTRL, NBPtr, &NBPtr->MemPtr->StdHeader);
}

/* -----------------------------------------------------------------------------*/
/**
 *      This function adjusts the Phase Mask based on ECC.
 *
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   OptParam   - Optional parameter
 *
 *     @return    TRUE
 * ----------------------------------------------------------------------------
 */
BOOLEAN
MemNAdjust2DPhaseMaskBasedOnEccUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  NBPtr->PhaseLaneMask = NBPtr->PhaseLaneMask & (UINT32) (NBPtr->MCTPtr->Status[SbEccDimms] ? 0x0FFFF : 0x3FFFF);
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function adjusts WrDqsBias before seed scaling
 *
 *     @param[in,out]   *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *WrDqsBias - Pointer to WrDqsBias
 *
 *     @return    FALSE - Supported
 *     @return    TRUE - Not supported
 */

BOOLEAN
MemNAdjustWrDqsBeforeSeedScalingUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *WrDqsBias
  )
{
  // Subtract (0x20 * WrDqDqsEarly) since it is a non-scalable component
  * (INT16 *) WrDqsBias = (INT16) (0x20 * MemNGetBitFieldNb (NBPtr, BFWrDqDqsEarly));
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function adjust RdDqsDly used for MaxRdLatency calculation
 *
 *     @param[in,out]   *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *OptParam  - Optional parameter
 *
 *     @return    FALSE - Supported
 *     @return    TRUE - Not supported
 */

BOOLEAN
MemNAdjustRdDqsDlyForMaxRdLatUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  // Set RdDqsDlyForMaxRdLat to 0 so that actual RdDqsDly is used for MaxRdLatency calculation
  NBPtr->RdDqsDlyForMaxRdLat = 0;
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *     This function collects data for Eye Rim Search
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return  TRUE - No Errors occurred
 *     @return  FALSE - Errors occurred
 */
BOOLEAN
MemN2DRdDQSEyeRimSearchUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return MemT2DRdDQSEyeRimSearch (NBPtr->TechPtr);
}

/*-----------------------------------------------------------------------------
 *
 *
 *     Hook for some families which need an intermediate Mem Clk Frequency Valid
 *       before changing to the initial startup training frequency.
 *
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]  *OptParam   - Optional parameter
 *
 *     @return  TRUE - always
 * ----------------------------------------------------------------------------
 */
BOOLEAN
MemNIntermediateMemclkFreqValUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  // 1. Program MemClkFreq = 200MHz
  MemNSetBitFieldNb (NBPtr, BFMemClkFreq, MemNGetMemClkFreqIdUnb (NBPtr, DDR400_FREQUENCY));
  // 2. MemClkFreqVal = 1;
  MemNSetBitFieldNb (NBPtr, BFMemClkFreqVal, 1);
  // 3. Wait for FreqChgInPrg == 0
  MemNPollBitFieldNb (NBPtr, BFFreqChgInProg, 0, PCI_ACCESS_TIMEOUT, FALSE);
  // 4. Program MemClkFreqVal = 0
  MemNSetBitFieldNb (NBPtr, BFMemClkFreqVal, 0);
  // 5. Retore MemClkFreq to startup value.
  MemNSetBitFieldNb (NBPtr, BFMemClkFreq, MemNGetMemClkFreqIdUnb (NBPtr, NBPtr->DCTPtr->Timings.Speed));
  return TRUE;
}
