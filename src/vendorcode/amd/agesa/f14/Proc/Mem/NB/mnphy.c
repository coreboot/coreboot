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
 * @e \$Revision: 38306 $ @e \$Date: 2010-09-22 01:51:51 +0800 (Wed, 22 Sep 2010) $
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
MemNPhyFenceTrainingNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  NBPtr->MemPPhyFenceTrainingNb (NBPtr);
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

  if (NBPtr->MCTPtr->Status[SbLrdimms]) {
    // 18. If motherboard routing requires CS[7:6] to adopt address timings, e.g. 3 LRDIMMs/ch with CS[7:6]
    // routed across all DIMM sockets, BIOS performs the following:
    if (FindPSOverrideEntry (NBPtr->RefPtr->PlatformMemoryConfiguration, PSO_NO_LRDIMM_CS67_ROUTING, NBPtr->MCTPtr->SocketId, NBPtr->ChannelPtr->ChannelID) != NULL) {
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
  UINT16 Avg;
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
    Avg = Avg + ((UINT16) PREvalue);
    IDS_HDT_CONSOLE (MEM_FLOW, "%02x ", PREvalue);
  }
  Avg = ((Avg + 8) / 9);    // round up

  Avg -= 8;
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

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function initializes the DDR phy compensation logic
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNInitPhyCompNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  CONST UINT8 TableCompRiseSlew20x[] = {7, 3, 2, 2};
  CONST UINT8 TableCompRiseSlew15x[] = {7, 7, 3, 2};
  CONST UINT8 TableCompFallSlew20x[] = {7, 5, 3, 2};
  CONST UINT8 TableCompFallSlew15x[] = {7, 7, 5, 3};
  UINT8 i;
  UINT8 j;
  UINT8 CurrDct;
  UINT8 CurrChannel;
  BOOLEAN MarginImprv;
  MarginImprv = FALSE;
  CurrDct = NBPtr->Dct;
  CurrChannel = NBPtr->Channel;
  if (NBPtr->IsSupported[CheckSlewWithMarginImprv]) {
    if (NBPtr->MCTPtr->GangedMode == FALSE) {
      for (i = 0; i < NBPtr->DctCount; i++) {
        MemNSwitchDCTNb (NBPtr, i);
        for (j = 0; j < NBPtr->ChannelCount; j++) {
          NBPtr->SwitchChannel (NBPtr, j);
          if ((NBPtr->ChannelPtr->Dimms == 4) && ((NBPtr->DCTPtr->Timings.Speed == DDR533_FREQUENCY) || (NBPtr->DCTPtr->Timings.Speed == DDR667_FREQUENCY))) {
            MarginImprv = TRUE;
          }
        }
      }
      MemNSwitchDCTNb (NBPtr, CurrDct);
      NBPtr->SwitchChannel (NBPtr, CurrChannel);
    }
  }

  // 1. BIOS disables the phy compensation register by programming F2x9C_x08[DisAutoComp]=1
  // 2. BIOS waits 5 us for the disabling of the compensation engine to complete.
  // DisAutoComp will be cleared after Dram init has completed
  //
  MemNSwitchDCTNb (NBPtr, 0);
  MemNSetBitFieldNb (NBPtr, BFDisAutoComp, 1);
  MemUWait10ns (500, NBPtr->MemPtr);
  MemNSwitchDCTNb (NBPtr, CurrDct);

  // 3. For each normalized driver strength code read from
  // F2x[1, 0]9C_x00[AddrCmdDrvStren], program the
  // corresponding 3 bit predriver code in F2x9C_x0A[D3Cmp1NCal, D3Cmp1PCal].
  //
  // 4. For each normalized driver strength code read from
  // F2x[1, 0]9C_x00[DataDrvStren], program the corresponding
  // 3 bit predriver code in F2x9C_x0A[D3Cmp0NCal, D3Cmp0PCal, D3Cmp2NCal,
  // D3Cmp2PCal].
  //
  j = (UINT8) MemNGetBitFieldNb (NBPtr, BFAddrCmdDrvStren);
  i = (UINT8) MemNGetBitFieldNb (NBPtr, BFDataDrvStren);

  MemNSwitchDCTNb (NBPtr, 0);
  MemNSetBitFieldNb (NBPtr, BFD3Cmp1NCal, TableCompRiseSlew20x[j]);
  MemNSetBitFieldNb (NBPtr, BFD3Cmp1PCal, TableCompFallSlew20x[j]);

  if (NBPtr->IsSupported[CheckSlewWithMarginImprv]) {
    MemNSetBitFieldNb (NBPtr, BFD3Cmp0NCal, (MarginImprv) ? 0 : TableCompRiseSlew15x[i]);
    MemNSetBitFieldNb (NBPtr, BFD3Cmp0PCal, (MarginImprv) ? 0 : TableCompFallSlew15x[i]);
    MemNSetBitFieldNb (NBPtr, BFD3Cmp2NCal, (MarginImprv) ? 0 : TableCompRiseSlew15x[i]);
    MemNSetBitFieldNb (NBPtr, BFD3Cmp2PCal, (MarginImprv) ? 0 : TableCompFallSlew15x[i]);
  }
  if (NBPtr->IsSupported[CheckSlewWithoutMarginImprv]) {
    ASSERT (i <= 3);
    MemNSetBitFieldNb (NBPtr, BFD3Cmp0NCal, TableCompRiseSlew15x[i]);
    MemNSetBitFieldNb (NBPtr, BFD3Cmp0PCal, TableCompFallSlew15x[i]);
    MemNSetBitFieldNb (NBPtr, BFD3Cmp2NCal, TableCompRiseSlew15x[i]);
    MemNSetBitFieldNb (NBPtr, BFD3Cmp2PCal, TableCompFallSlew15x[i]);
  }
  MemNSwitchDCTNb (NBPtr, CurrDct);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This is a general purpose function that executes before DRAM training
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNBeforeDQSTrainingNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Dct;
  UINT8 ChipSel;
  UINT32 TestAddrRJ16;
  UINT32 RealAddr;

  MemTBeginTraining (NBPtr->TechPtr);

  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    MemNSwitchDCTNb (NBPtr, Dct);
    if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
      for (ChipSel = 0; ChipSel < MAX_CS_PER_CHANNEL; ChipSel += 2) {
        if (MemNGetMCTSysAddrNb (NBPtr, ChipSel, &TestAddrRJ16)) {

          RealAddr = MemUSetUpperFSbase (TestAddrRJ16, NBPtr->MemPtr);

          MemUDummyCLRead (RealAddr);

          MemNSetBitFieldNb (NBPtr, BFErr350, 0x8000);
          MemUWait10ns (60, NBPtr->MemPtr);   // Wait 300ns
          MemNSetBitFieldNb (NBPtr, BFErr350, 0x0000);
          MemUWait10ns (400, NBPtr->MemPtr);  // Wait 2us
          MemUProcIOClFlush (TestAddrRJ16, 1, NBPtr->MemPtr);
          break;
        }
      }
    }
    if (NBPtr->IsSupported[CheckEccDLLPwrDnConfig]) {
      if (!NBPtr->MCTPtr->Status[SbEccDimms]) {
        MemNSetBitFieldNb (NBPtr, BFEccDLLPwrDnConf, 0x0010);
      }
      if (NBPtr->DCTPtr->Timings.Dimmx4Present == 0) {
        MemNSetBitFieldNb (NBPtr, BFEccDLLConf, 0x0080);
      }
    }
  }

  MemTEndTraining (NBPtr->TechPtr);
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
MemNGetTrainDlyParmsNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       TRN_DLY_TYPE TrnDly,
  IN OUT   TRN_DLY_PARMS *Parms
  )
{
  Parms->Min = 0;

  if (TrnDly == AccessWrDatDly) {
    Parms->Max = 0x1F;
    Parms->Mask = 0x01F;
  } else if (TrnDly == AccessRdDqsDly) {
    if ( (NBPtr->IsSupported[CheckMaxRdDqsDlyPtr]) && (NBPtr->DCTPtr->Timings.Speed > DDR667_FREQUENCY) ) {
      Parms->Max = 0x3E;
      Parms->Mask = 0x03E;
    } else {
      Parms->Max = 0x1F;
      Parms->Mask = 0x01F;
    }
  }
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
MemNGetTrainDlyParmsClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       TRN_DLY_TYPE TrnDly,
  IN OUT   TRN_DLY_PARMS *Parms
  )
{
  Parms->Min = 0;

  if (TrnDly == AccessWrDatDly) {
    Parms->Max = 0x1F;
    Parms->Mask = 0x01F;
  } else if (TrnDly == AccessRdDqsDly) {
    Parms->Max = 0x3E;
    Parms->Mask = 0x03E;
  }
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

  if (TrnDly == AccessWrDatDly) {
    Parms->Max = 0x1F;
    Parms->Mask = 0x01F;
  } else if (TrnDly == AccessRdDqsDly) {
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
MemNcmnGetSetTrainDlyNb (
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

  Dimm = DRBN_DIMM (DrbnVar);
  Rank = DRBN_RANK (DrbnVar);
  Byte = DRBN_BYTE (DrbnVar);
  Nibble = DRBN_NBBL (DrbnVar);

  ASSERT (Dimm < 4);
  ASSERT (Byte <= ECC_DLY);

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
    break;
  default:
    Offset = 0;
    IDS_ERROR_TRAP;
  }

  Address = Index;
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

    Value = ((UINT32)Field << Offset) | (Value & (~((UINT32) ((TrnDly == AccessRcvEnDly) ? 0x1FF : 0xFF) << Offset)));
    MemNSetBitFieldNb (NBPtr, BFDctAddlDataReg, Value);
    Address |= DCT_ACCESS_WRITE;
    MemNSetBitFieldNb (NBPtr, BFDctAddlOffsetReg, Address);
    MemNPollBitFieldNb (NBPtr, BFDctAccessDone, 1, PCI_ACCESS_TIMEOUT, FALSE);

    if (TrnDly == AccessPhRecDly) {
      NBPtr->DctCachePtr->PhRecReg[Index & 0x03] = Value;
    }
  } else {
    Value = (Value >> Offset) & (UINT32) ((TrnDly == AccessRcvEnDly) ? 0x1FF : 0xFF);
  }

  return Value;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function gets or set DQS timing during training.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]   IsSet - Indicates if the function will set or get
 *     @param[in]   TrnDly - type of delay to be set
 *     @param[in]   DrbnVar - encoding of Dimm-Rank-Byte-Nibble to be accessed
 *                  (use either DIMM_BYTE_ACCESS(dimm,byte) or CS_NBBL_ACCESS(cs,nibble) to use this encoding
 *     @param[in]   Field - Value to be programmed
 *
 *     @return      value read, if the function is used as a "get"
 */
UINT32
MemNcmnGetSetTrainDlyClientNb (
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
  UINT8 Byte;

  Dimm = DRBN_DIMM (DrbnVar);
  Byte = DRBN_BYTE (DrbnVar);

  ASSERT (Dimm < 2);
  ASSERT (Byte <= ECC_DLY);

  if ((Byte > 7)) {
    // Llano does not support ECC delay, so:
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
    Offset = 16 * (Byte % 2);
    break;

  case AccessRdDqsDly:
  case AccessWrDatDly:
    Index += (Dimm * 0x100);
    // break is not being used here because AccessRdDqsDly and AccessWrDatDly also need
    // to run AccessPhRecDly sequence.
  case AccessPhRecDly:
    Index += (Byte / 4);
    Offset = 8 * (Byte % 4);
    break;
  default:
    Offset = 0;
    IDS_ERROR_TRAP;
  }

  Address = Index;
  MemNSetBitFieldNb (NBPtr, BFDctAddlOffsetReg, Address);
  Value = MemNGetBitFieldNb (NBPtr, BFDctAddlDataReg);

  if (IsSet) {
    if (TrnDly == AccessPhRecDly) {
      Value = NBPtr->DctCachePtr->PhRecReg[Index & 0x03];
    }

    Value = ((UINT32)Field << Offset) | (Value & (~((UINT32) ((TrnDly == AccessRcvEnDly) ? 0x1FF : 0xFF) << Offset)));
    MemNSetBitFieldNb (NBPtr, BFDctAddlDataReg, Value);
    Address |= DCT_ACCESS_WRITE;
    MemNSetBitFieldNb (NBPtr, BFDctAddlOffsetReg, Address);

    if (TrnDly == AccessPhRecDly) {
      NBPtr->DctCachePtr->PhRecReg[Index & 0x03] = Value;
    }
    // Gross WrDatDly and WrDqsDly cannot be larger than 4
    ASSERT (((TrnDly == AccessWrDatDly) || (TrnDly == AccessWrDqsDly)) ? (Field < 0xA0) : TRUE);
  } else {
    Value = (Value >> Offset) & (UINT32) ((TrnDly == AccessRcvEnDly) ? 0x1FF : 0xFF);
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
 *     This function returns the number of chipselects per channel.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return
 */

UINT8
MemNCSPerChannelNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return MAX_CS_PER_CHANNEL;
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
  return MAX_CS_PER_DELAY;
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
  if (NBPtr->TechPtr->Direction == DQS_READ_DIR) {
    return MIN_RD_DATAEYE_WIDTH_NB;
  } else {
    return MIN_WR_DATAEYE_WIDTH_NB;
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
  UINT16 Value;

  Value = (UINT16) NBPtr->RefPtr->DDR3Voltage << 3;

  for (BitField = BFDataRxVioLvl; BitField <= BFCmpVioLvl; BitField++) {
    if (BitField == BFCmpVioLvl) {
      Value <<= (14 - 3);
    }
    MemNBrdcstSetNb (NBPtr, BitField, Value);
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function adjusts Avg PRE value of Phy fence training according to specific CPU family.
 *
 *     @param[in,out]   *NBPtr  - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *Value16 - Pointer to the value that we want to adjust
 *
 */
VOID
MemNPFenceAdjustUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   UINT16 *Value16
  )
{
  *Value16 += 2; //for LN,ON,and OR, the Avg PRE value is subtracted by 6 only.
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function initializes the DDR phy compensation logic
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNInitPhyCompClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  // Slew rate table array [x][y][z]
  // array[0]: slew rate for VDDIO 1.5V
  // array[1]: slew rate for VDDIO 1.35V
  // array[x][y]: slew rate for a certain frequency
  // array[x][y][0]: frequency mask for current entry
  CONST STATIC UINT16 TxPrePNDataDqs[2][3][5] = {
    {{ (UINT16) DDR800, 0x924, 0x924, 0x924, 0x924},
     { (UINT16) (DDR1066 + DDR1333), 0xFF6, 0xFF6, 0xFF6, 0xFF6},
     { (UINT16) (DDR1600 + DDR1866), 0xFF6, 0xFF6, 0xFF6, 0xFF6}},
    {{ (UINT16) DDR800, 0xFF6, 0xB6D, 0xB6D, 0x924},
     { (UINT16) (DDR1066 + DDR1333), 0xFF6, 0xFF6, 0xFF6, 0xFF6},
     { (UINT16) (DDR1600 + DDR1866), 0xFF6, 0xFF6, 0xFF6, 0xFF6}}
  };
  CONST STATIC UINT16 TxPrePNCmdAddr[2][3][5] = {
    {{ (UINT16) DDR800, 0x492, 0x492, 0x492, 0x492},
     { (UINT16) (DDR1066 + DDR1333), 0x6DB, 0x6DB, 0x6DB, 0x6DB},
     { (UINT16) (DDR1600 + DDR1866), 0xB6D, 0xB6D, 0xB6D, 0xB6D}},
    {{ (UINT16) DDR800, 0x492, 0x492, 0x492, 0x492},
     { (UINT16) (DDR1066 + DDR1333), 0x924, 0x6DB, 0x6DB, 0x6DB},
     { (UINT16) (DDR1600 + DDR1866), 0xB6D, 0xB6D, 0x924, 0x924}}
  };
  CONST STATIC UINT16 TxPrePNClock[2][3][5] = {
    {{ (UINT16) DDR800, 0x924, 0x924, 0x924, 0x924},
     { (UINT16) (DDR1066 + DDR1333), 0xFF6, 0xFF6, 0xFF6, 0xB6D},
     { (UINT16) (DDR1600 + DDR1866), 0xFF6, 0xFF6, 0xFF6, 0xFF6}},
    {{ (UINT16) DDR800, 0xDAD, 0xDAD, 0x924, 0x924},
     { (UINT16) (DDR1066 + DDR1333), 0xFF6, 0xFF6, 0xFF6, 0xDAD},
     { (UINT16) (DDR1600 + DDR1866), 0xFF6, 0xFF6, 0xFF6, 0xDAD}}
  };

  CONST PHY_COMP_INIT_CLIENTNB PhyCompInitBitField[] = {
    // 3. Program TxPreP/TxPreN for Data and DQS according toTable 14 if VDDIO is 1.5V or Table 15 if 1.35V.
    //    A. Program D18F2x[1,0]9C_x0D0F_0[F,7:0]0[A,6]={0000b, TxPreP, TxPreN}.
    //    B. Program D18F2x[1,0]9C_x0D0F_0[F,7:0]02={1000b, TxPreP, TxPreN}.
    {BFDqsDrvStren, BFDataByteTxPreDriverCal2Pad1, BFDataByteTxPreDriverCal2Pad1, 0, TxPrePNDataDqs},
    {BFDataDrvStren, BFDataByteTxPreDriverCal2Pad2, BFDataByteTxPreDriverCal2Pad2, 0, TxPrePNDataDqs},
    {BFDataDrvStren, BFDataByteTxPreDriverCal, BFDataByteTxPreDriverCal, 8, TxPrePNDataDqs},
    // 4. Program TxPreP/TxPreN for Cmd/Addr according toTable 16 if VDDIO is 1.5V or Table 17 if 1.35V.
    //    A. Program D18F2x[1,0]9C_x0D0F_[C,8][1:0][12,0E,0A,06]={0000b, TxPreP, TxPreN}.
    //    B. Program D18F2x[1,0]9C_x0D0F_[C,8][1:0]02={1000b, TxPreP, TxPreN}.
    {BFCsOdtDrvStren, BFCmdAddr0TxPreDriverCal2Pad1, BFCmdAddr0TxPreDriverCal2Pad2, 0, TxPrePNCmdAddr},
    {BFAddrCmdDrvStren, BFCmdAddr1TxPreDriverCal2Pad1, BFAddrTxPreDriverCal2Pad4, 0, TxPrePNCmdAddr},
    {BFCsOdtDrvStren, BFCmdAddr0TxPreDriverCalPad0, BFCmdAddr0TxPreDriverCalPad0, 8, TxPrePNCmdAddr},
    {BFCkeDrvStren, BFAddrTxPreDriverCalPad0, BFAddrTxPreDriverCalPad0, 8, TxPrePNCmdAddr},
    {BFAddrCmdDrvStren, BFCmdAddr1TxPreDriverCalPad0, BFCmdAddr1TxPreDriverCalPad0, 8, TxPrePNCmdAddr},
    // 5. Program TxPreP/TxPreN for Clock according toTable 18 if VDDIO is 1.5V or Table 19 if 1.35V.
    //    A. Program D18F2x[1,0]9C_x0D0F_2[1:0]02={1000b, TxPreP, TxPreN}.
    {BFClkDrvStren, BFClock0TxPreDriverCalPad0, BFClock1TxPreDriverCalPad0, 8, TxPrePNClock}
  };

  BIT_FIELD_NAME CurrentBitField;
  UINT16 SpeedMask;
  CONST UINT16 (*TxPrePNArray)[5];
  UINT8 Voltage;
  UINT8 i;
  UINT8 j;
  UINT8 k;
  UINT8 Dct;

  Dct = NBPtr->Dct;
  NBPtr->SwitchDCT (NBPtr, 0);
  // 1. Program D18F2x[1,0]9C_x0D0F_E003[DisAutoComp, DisablePreDriverCal] = {1b, 1b}.
  MemNSetBitFieldNb (NBPtr, BFDisablePredriverCal, 0x6000);
  NBPtr->SwitchDCT (NBPtr, Dct);

  SpeedMask = (UINT16) 1 << (NBPtr->DCTPtr->Timings.Speed / 66);
  Voltage = (UINT8) NBPtr->RefPtr->DDR3Voltage;

  for (j = 0; j < GET_SIZE_OF (PhyCompInitBitField); j ++) {
    i = (UINT8) MemNGetBitFieldNb (NBPtr, PhyCompInitBitField[j].IndexBitField);
    TxPrePNArray = PhyCompInitBitField[j].TxPrePN[Voltage];
    for (k = 0; k < 3; k ++) {
      if ((TxPrePNArray[k][0] & SpeedMask) != 0) {
        for (CurrentBitField = PhyCompInitBitField[j].StartTargetBitField; CurrentBitField <= PhyCompInitBitField[j].EndTargetBitField; CurrentBitField ++) {
          MemNSetBitFieldNb (NBPtr, CurrentBitField, ((PhyCompInitBitField[j].ExtraValue << 12) | TxPrePNArray[k][i + 1]));
        }
        break;
      }
    }
    ASSERT (k < 3);
  }

  NBPtr->FamilySpecificHook[ForceAutoComp] (NBPtr, NBPtr);
}

/*-----------------------------------------------------------------------------
 *
 *
 *     This function re-enable phy compensation.
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]  OptParam   - Optional parameter
 *
 *     @return    TRUE
 * ----------------------------------------------------------------------------
 */
BOOLEAN
MemNReEnablePhyCompNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  UINT8 Dct;

  Dct = NBPtr->Dct;

  NBPtr->SwitchDCT (NBPtr, 0);
  // Clear DisableCal and set DisablePredriverCal
  MemNSetBitFieldNb (NBPtr, BFDisablePredriverCal, 0x2000);
  NBPtr->SwitchDCT (NBPtr, Dct);

  return TRUE;
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
    IDS_HDT_CONSOLE(MEM_FLOW, "\t\tCalculating WrDqDqsEarly, adjusting WrDqs.\n");
    // We've saved the entire negative delay value, so take the ABS and convert to GrossDly.
    WrDqDqsEarly =  (UINT8) (0x00FF &((((ABS (TechPtr->WLCriticalDelay)) + 0x1F) / 0x20)));
    //
    // Loop through All WrDqsDlys on all DIMMs
    //
    for (Dimm = 0; Dimm < MAX_DIMMS_PER_CHANNEL; Dimm++) {
      if ((DCTPtr->Timings.CsEnabled & ((UINT16)3 << (Dimm << 1))) != 0) {
    	IDS_HDT_CONSOLE(MEM_FLOW, "\t\t\tDimm %x:",Dimm);
        WrDqsDlysPtr = &(ChannelPtr->WrDqsDlys[(Dimm * TechPtr->DlyTableWidth ())]);
        for (ByteLane = 0; ByteLane < TechPtr->DlyTableWidth (); ByteLane++) {
          WrDqsDlysPtr[ByteLane] += (WrDqDqsEarly << 5);
          NBPtr->SetTrainDly (NBPtr, AccessWrDqsDly, DIMM_BYTE_ACCESS (Dimm, ByteLane), WrDqsDlysPtr[ByteLane]);
		  IDS_HDT_CONSOLE(MEM_FLOW, " %02x",WrDqsDlysPtr[ByteLane]);
        }
		IDS_HDT_CONSOLE(MEM_FLOW, "\n");
      }
    }
  }
  MemNSetBitFieldNb (NBPtr, BFWrDqDqsEarly, WrDqDqsEarly);
  IDS_HDT_CONSOLE(MEM_FLOW, "\t\tWrDqDqsEarly : %02x\n",WrDqDqsEarly);
  return TRUE;
}
