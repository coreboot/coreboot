/* $NoKeywords:$ */
/**
 * @file
 *
 * mrndct.c
 *
 * Northbridge common DCT support for Recovery
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Proc/Recovery/Mem/NB)
 * @e \$Revision: 50454 $ @e \$Date: 2011-04-10 21:20:37 -0600 (Sun, 10 Apr 2011) $
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
#include "cpuFamRegisters.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "mru.h"
#include "ma.h"
#include "Filecode.h"
#define FILECODE PROC_RECOVERY_MEM_NB_MRNDCT_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define RECDEF_CSMASK_REG         0x00083FE0
#define RECDEF_DRAM_BASE_REG      0x00000003


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
  CONST UINT16 (*TxPrePN)[4];         ///< Pointer to slew rate table
} REC_PHY_COMP_INIT_CLIENTNB;

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
VOID
STATIC
MemRecTCtlOnDimmMirrorNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       BOOLEAN SetFlag
  );

VOID
STATIC
MemRecNSwapBitsNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
STATIC
MemRecNProgNbPstateDependentRegClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
STATIC
MemRecNTrainPhyFenceNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
STATIC
MemRecNCommonReadWritePatternUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8  CmdType,
  IN       UINT16 ClCount
  );

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function programs the memory controller with configuration parameters
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE - An Error value lower than AGESA_ERROR may have occurred
 *     @return          FALSE - An Error value greater than or equal to AGESA_ERROR may have occurred
 */

BOOLEAN
MemRecNAutoConfigNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Dimm;
  UINT8 Dct;
  UINT8 ChipSel;
  UINT32 CSBase;
  DCT_STRUCT *DCTPtr;
  CH_DEF_STRUCT *ChannelPtr;
  UINT16 i;

  Dct = NBPtr->Dct;
  DCTPtr = NBPtr->DCTPtr;
  ChannelPtr = NBPtr->ChannelPtr;

  //Prepare variables for future usage.
  for (Dimm = 0; Dimm < MAX_DIMMS_PER_CHANNEL; Dimm++) {
    if ((ChannelPtr->ChDimmValid & (UINT8) 1 << Dimm) != 0) {
      DCTPtr->Timings.CsPresent |= (UINT16) 1 << (Dimm * 2);
      if (((ChannelPtr->DimmDrPresent & (UINT8) 1 << Dimm) == 0) && ((ChannelPtr->DimmQrPresent & (UINT8) 1 << Dimm) == 0)) {
        continue;
      } else {
        DCTPtr->Timings.CsPresent |= (UINT16) 1 << (Dimm * 2 + 1);
      }
    }
  }

  Dimm = NBPtr->DimmToBeUsed;

  //Temporarily set all CS Base/Limit registers (corresponding to Dimms exist on a channel) with 256MB size for WL training.
  CSBase = 0;
  for (ChipSel = 0; ChipSel < MAX_CS_PER_CHANNEL; ChipSel++) {
    if (DCTPtr->Timings.CsPresent & (UINT8) 1 << ChipSel) {

      CSBase &= (UINT32) ~0x08; //Clear OnDimmMirror bit.
      if (((ChipSel & 1) != 0) && ((ChannelPtr->DimmMirrorPresent & (UINT8) 1 << (ChipSel >> 1)) != 0)) {
        CSBase |= (UINT32) 0x08; //Set OnDimmMirror bit.
      }
      MemRecNSetBitFieldNb (NBPtr, (BFCSBaseAddr0Reg + ChipSel), (CSBase | 0x01));
      CSBase += 0x100000;
      if ((ChipSel & 1) == 0) {
        MemRecNSetBitFieldNb (NBPtr, (BFCSMask0Reg + (ChipSel >> 1)), RECDEF_CSMASK_REG);
      }
    }
  }
  MemRecNSetBitFieldNb (NBPtr, BFDramBaseReg0, RECDEF_DRAM_BASE_REG);
  MemRecNSetBitFieldNb (NBPtr, BFDramLimitReg0, 0x70000);

  // Disable the other DCT
  NBPtr->MemRecNSwitchDctNb (NBPtr, Dct ^ 0x01);
  MemRecNSetBitFieldNb (NBPtr, BFDisDramInterface, 1);
  NBPtr->MemRecNSwitchDctNb (NBPtr, Dct);
  if (Dct != 0) {
    // If DCT 1, set DctSelBase registers
    MemRecNSetBitFieldNb (NBPtr, BFDctSelBaseAddrReg, 0x00000003);
    MemRecNSetBitFieldNb (NBPtr, BFDctSelBaseOffsetReg, 0x00000000);
  }

  // Use default values for common registers
  i = 0;
  while (NBPtr->RecModeDefRegArray[i] != NULL) {
    MemRecNSetBitFieldNb (NBPtr, NBPtr->RecModeDefRegArray[i], NBPtr->RecModeDefRegArray[i + 1]);
    i += 2;
  }

  // Other specific settings
  MemRecNSetBitFieldNb (NBPtr, BFX4Dimm, ChannelPtr->Dimmx4Present );

  if ((ChannelPtr->RegDimmPresent  == 0) && (ChannelPtr->SODimmPresent == 0)) {
    MemRecNSetBitFieldNb (NBPtr, BFUnBuffDimm, 1);
  }

  if ((NBPtr->ChannelPtr->RegDimmPresent != 0) && (NBPtr->ChannelPtr->TechType == DDR3_TECHNOLOGY)) {
    MemRecNSetBitFieldNb (NBPtr, BFSubMemclkRegDly, 1);
  }
  MemRecNSetBitFieldNb (NBPtr, BFOdtSwizzle, 1);

  return TRUE;
}


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
MemRecNPlatformSpecNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 p;

  p = 0;
  for (p = 0; p < MAX_PLATFORM_TYPES; p++) {
    if (NBPtr->MemPtr->GetPlatformCfg[p] (NBPtr->MemPtr, NBPtr->MCTPtr->SocketId, NBPtr->ChannelPtr) == AGESA_SUCCESS) {
      MemRecNSetBitFieldNb (NBPtr, BFODCControl, NBPtr->ChannelPtr->DctOdcCtl);
      MemRecNSetBitFieldNb (NBPtr, BFAddrTmgControl, NBPtr->ChannelPtr->DctAddrTmg);
      return TRUE;
    }
  }
  return FALSE;
}


/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function  reads MemClkFreqVal bit to see if the DIMMs are present in this node.
 *  If the DIMMs are present then set the DRAM Enable bit for this node.
 *
 *  Setting dram init starts up the DCT state machine, initializes the
 *  dram devices with MRS commands, and kicks off any
 *  HW memory clear process that the chip is capable of.  The sooner
 *  that dram init is set for all nodes, the faster the memory system
 *  initialization can complete.  Thus, the init loop is unrolled into
 *  two loops so as to start the processes for non BSP nodes sooner.
 *  This procedure will not wait for the process to finish.  Synchronization is
 *  handled elsewhere.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemRecNStartupDCTNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  // 1. Ensure F2x[1, 0]9C_x08[DisAutoComp] = 1.
  // 2. BIOS waits 5 us for the disabling of the compensation engine to complete.
  // ------- Done in InitPhyComp_Nb -------
  //
  MemRecNSetBitFieldNb (NBPtr, BFDisAutoComp, 1);
  MemRecUWait10ns (500, NBPtr->MemPtr);

  //MemRecNSetBitFieldNb (NBPtr, BFInitDram, 1);    // HW Dram init
  AGESA_TESTPOINT (TpProcMemDramInit, &(NBPtr->MemPtr->StdHeader));
  NBPtr->TechPtr->DramInit (NBPtr->TechPtr);

  // 7. Program F2x[1, 0]9C_x08[DisAutoComp] = 0.
  // 8. BIOS must wait 750 us for the phy compensation engine
  //    to reinitialize.
  //
  MemRecNSetBitFieldNb (NBPtr, BFDisAutoComp, 0);
  MemRecUWait10ns (75000, NBPtr->MemPtr);

  while (MemRecNGetBitFieldNb (NBPtr, BFDramEnabled) == 0);
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
MemRecNStartupDCTClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  IDS_HDT_CONSOLE (MEM_STATUS, "\tDct %d\n", NBPtr->Dct);

  // Program D18F2x[1,0]9C_x0000_000B = 80000000h. #109999.
  MemRecNSetBitFieldNb (NBPtr, BFDramPhyStatusReg, 0x80000000);

  // Program D18F2x[1,0]9C_x0D0F_E013[PllRegWaitTime] = 0118h. #193770.
  MemRecNSetBitFieldNb (NBPtr, BFPllRegWaitTime, 0x118);

  // Phy Voltage Level Programming
  MemRecNPhyVoltageLevelNb (NBPtr);

  // Run frequency change sequence
  MemRecNSetBitFieldNb (NBPtr, BFPllLockTime, NBPtr->FreqChangeParam->PllLockTimeDefault);
  MemRecNSetBitFieldNb (NBPtr, BFMemClkFreq, 6);
  MemRecNProgNbPstateDependentRegClientNb (NBPtr);
  MemRecNSetBitFieldNb (NBPtr, BFMemClkFreqVal, 1);
  MemRecNSetBitFieldNb (NBPtr, BFPllLockTime, 0x000F);

  IDS_HDT_CONSOLE (MEM_FLOW, "\tMemClkAlign=0\n");
  IDS_HDT_CONSOLE (MEM_FLOW, "\tEnDramInit = 1 for DCT%d\n", NBPtr->Dct);
  MemRecNSetBitFieldNb (NBPtr, BFDbeGskMemClkAlignMode, 0);
  MemRecNSetBitFieldNb (NBPtr, BFEnDramInit, 1);

  // Run DramInit sequence
  AGESA_TESTPOINT (TpProcMemDramInit, &(NBPtr->MemPtr->StdHeader));
  NBPtr->TechPtr->DramInit (NBPtr->TechPtr);
  IDS_HDT_CONSOLE (MEM_FLOW, "\nMemClkFreq: %d MHz\n", DDR800_FREQUENCY);
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
MemRecNSetMaxLatencyNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT16 MaxRcvEnDly
  )
{
  UINT16 SubTotal;

  AGESA_TESTPOINT (TpProcMemRcvrCalcLatency, &(NBPtr->MemPtr->StdHeader));

  // Multiply the CAS Latency by two to get a number of 1/2 MEMCLKs UINTs.
  SubTotal = 6 * 2;

  // If registered DIMMs are being used then add 1 MEMCLK to the sub-total.
  if (MemRecNGetBitFieldNb (NBPtr, BFUnBuffDimm) == 0) {
    SubTotal += 2;
  }

  // if (AddrCmdSetup || CsOdtSetup || CkeSetup) then K := K + 2;
  SubTotal += 2;

  // If the F2x[1, 0]78[RdPtrInit] field is 4, 5, 6 or 7 MEMCLKs,
  // then add 4, 3, 2, or 1 MEMCLKs, respectively to the sub-total.
  //
  SubTotal += 8 - 5;

  // Add the maximum (worst case) delay value of DqsRcvEnGrossDelay
  // that exists across all DIMMs and byte lanes.
  //
  SubTotal += MaxRcvEnDly >> 5;

  // Add 5.5 to the sub-total. 5.5 represents part of the processor
  // specific constant delay value in the DRAM clock domain.
  //
  SubTotal += 5;             // add 5.5 1/2MemClk

  // Convert the sub-total (in 1/2 MEMCLKs) to northbridge clocks (NCLKs)
  // as follows (assuming DDR400 and assuming that no P-state or link speed
  // changes have occurred).
  //
  //     Simplified formula:
  //     SubTotal *= (Fn2xD4[NBFid]+4)/4
  //
  SubTotal = SubTotal * ((UINT16) MemRecNGetBitFieldNb (NBPtr, BFNbFid) + 4);
  SubTotal /= 4;

  // Add 5 NCLKs to the sub-total. 5 represents part of the processor
  // specific constant value in the northbridge clock domain.
  //
  SubTotal += 5;

  // Program the F2x[1, 0]78[MaxRdLatency] register with the total delay value
  MemRecNSetBitFieldNb (NBPtr, BFMaxLatency, SubTotal);
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
MemRecNSetDramOdtNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       ODT_MODE OdtMode,
  IN       UINT8 ChipSelect,
  IN       UINT8 TargetCS
  )
{
  UINT8 DramTerm;
  UINT8 DramTermDyn;

  DramTerm = NBPtr->ChannelPtr->Reserved[0];
  DramTermDyn = NBPtr->ChannelPtr->Reserved[1];

  if (OdtMode == WRITE_LEVELING_MODE) {
    if (ChipSelect == TargetCS) {
      DramTerm = DramTermDyn;
      MemRecNSetBitFieldNb (NBPtr, BFWrLvOdt, NBPtr->ChannelPtr->PhyWLODT[TargetCS >> 1]);
    }
  }
  MemRecNSetBitFieldNb (NBPtr, BFDramTerm, DramTerm);
  MemRecNSetBitFieldNb (NBPtr, BFDramTermDyn, DramTermDyn);
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function sends an MRS command
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemRecNSendMrsCmdNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  BOOLEAN ClearODM;
  ClearODM = FALSE;
  if (NBPtr->IsSupported[CheckClearOnDimmMirror]) {
    ClearODM = FALSE;
    if ((NBPtr->MCTPtr->LogicalCpuid.Revision & AMD_F10_C0) != 0) {
      if (NBPtr->IsSupported[CheckClearOnDimmMirror]) {
        if (MemRecNGetBitFieldNb (NBPtr, BFEnDramInit) == 0) {
          // For C0, if EnDramInit bit is cleared, ODM needs to be cleared before sending MRS
          MemRecTCtlOnDimmMirrorNb (NBPtr, FALSE);
          ClearODM = TRUE;
        }
      }
    }
  }

  MemRecNSwapBitsNb (NBPtr);

  IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tCS%d MR%d %04x\n",
              MemRecNGetBitFieldNb (NBPtr, BFMrsChipSel),
              MemRecNGetBitFieldNb (NBPtr, BFMrsBank),
              MemRecNGetBitFieldNb (NBPtr, BFMrsAddress));

  // 1.Set SendMrsCmd=1
  MemRecNSetBitFieldNb (NBPtr, BFSendMrsCmd, 1);

  // 2.Wait for SendMrsCmd=0
  while (MemRecNGetBitFieldNb (NBPtr, BFSendMrsCmd)) {}

  if (NBPtr->IsSupported[CheckClearOnDimmMirror]) {
    if (ClearODM) {
      // Restore ODM if necessary
      MemRecTCtlOnDimmMirrorNb (NBPtr, TRUE);
    }
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function sends the ZQCL command
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemRecNSendZQCmdNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  // 1.Program MrsAddress[10]=1
  MemRecNSetBitFieldNb (NBPtr, BFMrsAddress, (UINT32) 1 << 10);

  // 2.Set SendZQCmd=1
  MemRecNSetBitFieldNb (NBPtr, BFSendZQCmd, 1);

  // 3.Wait for SendZQCmd=0
  while (MemRecNGetBitFieldNb (NBPtr, BFSendZQCmd)) {}

  // 4.Wait 512 MEMCLKs
  MemRecUWait10ns (128, NBPtr->MemPtr);   // 512*2.5ns=1280, wait 1280ns
}


/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function disables/enables F2x[1, 0][5C:40][OnDimmMirror]
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]   SetFlag   - Enable or disable flag - TRUE - Enable, FALSE - DISABLE
 *
 */

VOID
STATIC
MemRecTCtlOnDimmMirrorNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       BOOLEAN SetFlag
  )
{
  UINT8 Chipsel;
  UINT32 CSBaseAddrReg;

  for (Chipsel = 0; Chipsel < MAX_CS_PER_CHANNEL; Chipsel += 2) {
    CSBaseAddrReg = MemRecNGetBitFieldNb (NBPtr, BFCSBaseAddr1Reg + Chipsel);
    if ((CSBaseAddrReg & 1) == 1) {
      if (SetFlag && ((NBPtr->ChannelPtr->DimmMirrorPresent & ((UINT8) 1 << (Chipsel >> 1))) != 0)) {
        CSBaseAddrReg |= ((UINT32) 1 << BFOnDimmMirror);
      } else {
        CSBaseAddrReg &= ~((UINT32) 1 << BFOnDimmMirror);
      }
      MemRecNSetBitFieldNb (NBPtr, BFCSBaseAddr1Reg + Chipsel, CSBaseAddrReg);
    }
  }
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function swaps bits for OnDimmMirror support
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
STATIC
MemRecNSwapBitsNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 ChipSel;
  UINT32 MRSReg;

  ChipSel = (UINT8) MemRecNGetBitFieldNb (NBPtr, BFMrsChipSel);
  if ((ChipSel & 1) != 0) {
    MRSReg = MemRecNGetBitFieldNb (NBPtr, BFDramInitRegReg);
    if ((NBPtr->ChannelPtr->DimmMirrorPresent & (UINT8) 1 << (ChipSel >> 1)) != 0) {
      MRSReg = (MRSReg & 0xFFFCFE07) | ((MRSReg&0x100A8) << 1) | ((MRSReg&0x20150) >> 1);
      MemRecNSetBitFieldNb (NBPtr, BFDramInitRegReg, MRSReg);
    }
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function gets the total of sync components for Max Read Latency calculation
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return      Total in 1/2 MEMCLKs
 */

UINT32
MemRecNTotalSyncComponentsClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT32 T;
  UINT32 P;
  UINT32 AddrTmgCtl;
  UINT32 MemClkPeriod;

  AGESA_TESTPOINT (TpProcMemRcvrCalcLatency , &(NBPtr->MemPtr->StdHeader));

  // P = P + ((16 + RdPtrInitMin - D18F2x[1,0]78[RdPtrInit]) MOD 16) where RdPtrInitMin = RdPtrInit
  P = 0;

  AddrTmgCtl = MemRecNGetBitFieldNb (NBPtr, BFAddrTmgControl);
  if (((AddrTmgCtl >> 16) & 0x20) != (AddrTmgCtl & 0x20)) {
    P += 1;
  }

  // IF (DbeGskMemClkAlignMode==01b || (DbeGskMemClkAlignMode==00b && !(AddrCmdSetup==CsOdtSetup==CkeSetup)))
  // THEN P = P + 1

  // IF (SlowAccessMode==1) THEN P = P + 2

  // T = T + (0.5 * MemClkPeriod) - 786 ps
  MemClkPeriod = 1000000 / DDR800_FREQUENCY;
  T = MemClkPeriod / 2 - 768;

  // If (AddrCmdSetup==0 && CsOdtSetup==0 && CkeSetup==0)
  // then P = P + 1
  // else P = P + 2
  if ((AddrTmgCtl & 0x0202020) == 0) {
    P += 1;
  } else {
    P += 2;
  }

  // P = P + (2 * (D18F2x[1,0]88[Tcl] clocks - 1))
  P += 2 * 5;  // Tcl = 6 clocks

  // (DisCutThroughMode = 0), so P = P + 3
  P += 3;

  return ((P * MemClkPeriod + 1) / 2) + T;
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
MemRecNPhyVoltageLevelNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  BIT_FIELD_NAME BitField;
  UINT16 Value;
  UINT16 Mask;

  Mask = 0xFFE7;
  Value = (UINT16) CONVERT_VDDIO_TO_ENCODED (NBPtr->RefPtr->DDR3Voltage) << 3;

  for (BitField = BFDataRxVioLvl; BitField <= BFCmpVioLvl; BitField++) {
    if (BitField == BFCmpVioLvl) {
      Mask = 0x3FFF;
      Value <<= (14 - 3);
    }
    MemRecNSetBitFieldNb (NBPtr, BitField, ((MemRecNGetBitFieldNb (NBPtr, BitField) & Mask)) | Value);
  }
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
STATIC
MemRecNTrainPhyFenceNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Byte;
  UINT16 Avg;
  UINT8 PREvalue;

  if (MemRecNGetBitFieldNb (NBPtr, BFDisDramInterface)) {
    return;
  }

  // 1. BIOS first programs a seed value to the phase recovery
  //    engine registers.
  //
  IDS_HDT_CONSOLE (MEM_FLOW, "\t\tSeeds: ");
  for (Byte = 0; Byte < 9; Byte++) {
    // This includes ECC as byte 8
    MemRecNSetTrainDlyNb (NBPtr, AccessPhRecDly, DIMM_BYTE_ACCESS (0, Byte), 19);
    IDS_HDT_CONSOLE (MEM_FLOW, "%02x ", 19);
  }

  IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\tPhyFenceTrEn = 1");
  // 2. Set F2x[1, 0]9C_x08[PhyFenceTrEn]=1.
  MemRecNSetBitFieldNb (NBPtr, BFPhyFenceTrEn, 1);

  MemRecUWait10ns (5000, NBPtr->MemPtr);

  // 4. Clear F2x[1, 0]9C_x08[PhyFenceTrEn]=0.
  MemRecNSetBitFieldNb (NBPtr, BFPhyFenceTrEn, 0);

  // 5. BIOS reads the phase recovery engine registers
  //    F2x[1, 0]9C_x[51:50] and F2x[1, 0]9C_x52.
  // 6. Calculate the average value of the fine delay and subtract 8.
  //
  Avg = 0;
  for (Byte = 0; Byte < 9; Byte++) {
    // This includes ECC as byte 8
    PREvalue = (UINT8) (0x1F & MemRecNGetTrainDlyNb (NBPtr, AccessPhRecDly, DIMM_BYTE_ACCESS (0, Byte)));
    Avg = Avg + ((UINT16) PREvalue);
  }
  Avg = ((Avg + 8) / 9);    // round up
  Avg -= 6;

  // 7. Write the value to F2x[1, 0]9C_x0C[PhyFence].
  MemRecNSetBitFieldNb (NBPtr, BFPhyFence, Avg);

  // 8. BIOS rewrites F2x[1, 0]9C_x04, DRAM Address/Command Timing Control
  //    Register delays for both channels. This forces the phy to recompute
  //    the fence.
  //
  MemRecNSetBitFieldNb (NBPtr, BFAddrTmgControl, MemRecNGetBitFieldNb (NBPtr, BFAddrTmgControl));
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  This function calculates and programs NB P-state dependent registers
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
STATIC
MemRecNProgNbPstateDependentRegClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 i;
  UINT8 NclkFid;
  UINT16 MemClkDid;
  UINT8 PllMult;
  UINT8 NclkDiv;
  UINT8 RdPtrInit;
  UINT32 NclkPeriod;
  UINT32 MemClkPeriod;
  INT32 PartialSum2x;
  INT32 PartialSumSlotI2x;
  INT32 RdPtrInitRmdr2x;

  NclkFid = (UINT8) (MemRecNGetBitFieldNb (NBPtr, BFMainPllOpFreqId) + 0x10);
  MemClkDid = 2; //BKDG recommended value for DDR800
  PllMult = 16;  //BKDG recommended value for DDR800
  NclkDiv = (UINT8) MemRecNGetBitFieldNb (NBPtr, BFNbPs0NclkDiv);

  NclkPeriod = (2500 * NclkDiv) / NclkFid;
  MemClkPeriod = 1000000 / DDR800_FREQUENCY;
  NBPtr->NBClkFreq = ((UINT32) NclkFid * 400) / NclkDiv;

  IDS_HDT_CONSOLE (MEM_FLOW, "\n\tNB P%d  Freq: %dMHz\n", 0, NBPtr->NBClkFreq);
  IDS_HDT_CONSOLE (MEM_FLOW, "\tMemClk Freq: %dMHz\n", DDR800_FREQUENCY);

  // D18F2x[1,0]78[RdPtrInit] = IF (D18F2x[1,0]94[MemClkFreq] >= 667 MHz) THEN 7 ELSE 8 ENDIF (Llano)
  //                                                                      THEN 2 ELSE 3 ENDIF (Ontario)
  RdPtrInit = NBPtr->FreqChangeParam->RdPtrInitLower667;
  MemRecNSetBitFieldNb (NBPtr, BFRdPtrInit, RdPtrInit);
  IDS_HDT_CONSOLE (MEM_FLOW, "\t\tRdPtr: %d\n", RdPtrInit);

  // Program D18F2x[1,0]F4_x30[DbeGskFifoNumerator] and D18F2x[1,0]F4_x31[DbeGskFifoDenominator].
  MemRecNSetBitFieldNb (NBPtr, BFDbeGskFifoNumerator, NclkFid * MemClkDid * 16);
  MemRecNSetBitFieldNb (NBPtr, BFDbeGskFifoDenominator, PllMult * NclkDiv);

  IDS_HDT_CONSOLE (MEM_FLOW, "\t\tDbeGskFifoNumerator: %d\n", NclkFid * MemClkDid * 16);
  IDS_HDT_CONSOLE (MEM_FLOW, "\t\tDbeGskFifoDenominator: %d\n", PllMult * NclkDiv);

  // Program D18F2x[1,0]F4_x32[DataTxFifoSchedDlyNegSlot1, DataTxFifoSchedDlySlot1,
  // DataTxFifoSchedDlyNegSlot0, DataTxFifoSchedDlySlot0].
  //   PartialSum = ((7 * NclkPeriod) + (1.5 * MemClkPeriod) + 520ps)*MemClkFrequency - tCWL -
  //   CmdSetup - PtrSeparation - 1. (Llano)
  //   PartialSum = ((5 * NclkPeriod) + MemClkPeriod) + 520ps)*MemClkFrequency - tCWL -
  //   CmdSetup - PtrSeparation - 1. (Ontario)
  PartialSum2x = NBPtr->FreqChangeParam->NclkPeriodMul2x * NclkPeriod;
  PartialSum2x += NBPtr->FreqChangeParam->MemClkPeriodMul2x * MemClkPeriod;
  PartialSum2x += 520 * 2;

  // PtrSeparation = ((16 + RdPtrInitMin - D18F2x[1,0]78[RdPtrInit]) MOD 16)/2 + RdPtrInitRmdr
  // RdPtrInitRmdr = (((2.25 * MemClkPeriod) - 1520ps) MOD MemClkPeriod)/MemClkPeriod
  RdPtrInitRmdr2x = ((NBPtr->FreqChangeParam->SyncTimeMul4x * MemClkPeriod) / 2) - 2 * (NBPtr->FreqChangeParam->TDataPropLower800 + 520);
  RdPtrInitRmdr2x %= MemClkPeriod;
  PartialSum2x -= RdPtrInitRmdr2x;
  PartialSum2x = (PartialSum2x + MemClkPeriod - 1) / MemClkPeriod;  // round-up here
  PartialSum2x -= 2 * 5;  //Tcwl + 5

  if ((MemRecNGetBitFieldNb (NBPtr, BFAddrTmgControl) & 0x0202020) == 0) {
    PartialSum2x -= 1;
  } else {
    PartialSum2x -= 2;
  }
  PartialSum2x -= 2;

  // If PartialSumSlotN is positive:
  //   DataTxFifoSchedDlySlotN=CEIL(PartialSumSlotN).
  //   DataTxFifoSchedDlyNegSlotN=0.
  // Else if PartialSumSlotN is negative:
  //   DataTxFifoSchedDlySlotN=ABS(CEIL(PartialSumSlotN*MemClkPeriod/NclkPeriod)).
  //   DataTxFifoSchedDlyNegSlotN=1.
  for (i = 0; i < 2; i++) {
    PartialSumSlotI2x = PartialSum2x;
    if (i == 0) {
    PartialSumSlotI2x += 2;
    }
    if (PartialSumSlotI2x > 0) {
      MemRecNSetBitFieldNb (NBPtr, BFDataTxFifoSchedDlyNegSlot0 + i, 0);
      MemRecNSetBitFieldNb (NBPtr, BFDataTxFifoSchedDlySlot0 + i, (PartialSumSlotI2x + 1) / 2);
      IDS_HDT_CONSOLE (MEM_FLOW, "\t\tDataTxFifoSchedDlySlot%d: %d\n", i, (PartialSumSlotI2x + 1) / 2);
    } else {
      MemRecNSetBitFieldNb (NBPtr, BFDataTxFifoSchedDlyNegSlot0 + i, 1);
      PartialSumSlotI2x = ((-PartialSumSlotI2x) * MemClkPeriod) / (2 * NclkPeriod);
      MemRecNSetBitFieldNb (NBPtr, BFDataTxFifoSchedDlySlot0 + i, PartialSumSlotI2x);
      IDS_HDT_CONSOLE (MEM_FLOW, "\t\tDataTxFifoSchedDlySlot%d: -%d\n", i, PartialSumSlotI2x);
    }
  }
  // Program ProcOdtAdv
  MemRecNSetBitFieldNb (NBPtr, BFProcOdtAdv, 0);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function reads cache lines continuously using TCB CPG engine
 *
 *     @param[in,out] NBPtr  - Pointer to the MEM_NB_BLOCK
 *     @param[in,out] Buffer - Array of bytes to be filled with data read from DRAM
 *     @param[in]     Address - System Address [47:16]
 *     @param[in] ClCount - Number of cache lines
 *
 */

VOID
MemRecNContReadPatternClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT32 Address,
  IN       UINT16 ClCount
  )
{
  // 1. Program D18F2x1C0[RdDramTrainMode]=1.
  MemRecNSetBitFieldNb (NBPtr, BFRdDramTrainMode, 1);

  // 2. Program D18F2x1C0[TrainLength] to the appropriate number of cache lines.
  MemRecNSetBitFieldNb (NBPtr, BFTrainLength, ClCount);

  // 3. Program the DRAM training address as follows:
  MemRecNSetBitFieldNb (NBPtr, BFWrTrainAdrPtrLo, (Address >> 6));

  // 4. Program D18F2x1D0[WrTrainBufAddr]=000h
  MemRecNSetBitFieldNb (NBPtr, BFWrTrainBufAddr, 0);

  // 5. Program D18F2x1C0[RdTrainGo]=1.
  MemRecNSetBitFieldNb (NBPtr, BFRdTrainGo, 1);

  // 6. Wait for D18F2x1C0[RdTrainGo]=0.
  while (MemRecNGetBitFieldNb (NBPtr, BFRdTrainGo) != 0) {}

  // 7. Read D18F2x1E8[TrainCmpSts] and D18F2x1E8[TrainCmpSts2].

  // 8. Program D18F2x1C0[RdDramTrainMode]=0.
  MemRecNSetBitFieldNb (NBPtr, BFRdDramTrainMode, 0);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This is function sets the platform specific settings for the systems with UDIMMs configuration
 *
 *     @param[in,out]   *MemData           Pointer to MEM_DATA_STRUCTURE
 *     @param[in]       SocketID        Socket number
 *     @param[in]       *CurrentChannel       Pointer to CH_DEF_STRUCT
 *
 *     @return          AGESA_SUCCESS
 *     @return          CurrentChannel->DctAddrTmg        Address Command Timing Settings for specified channel
 *     @return          CurrentChannel->DctOdcCtl         Drive Strength settings for specified channel
 *     @return          CurrentChannel->Reserved[0]       Dram Term for specified channel
 *     @return          CurrentChannel->Reserved[1]       Dynamic Dram Term for specified channel
 *     @return          CurrentChannel->PhyWLODT[0]       WL ODT for DIMM0
 *     @return          CurrentChannel->PhyWLODT[1]       WL ODT for DIMM1
 *     @return          CurrentChannel->PhyWLODT[2]       WL ODT for DIMM2
 *     @return          CurrentChannel->PhyWLODT[3]       WL ODT for DIMM3
 *
 */
AGESA_STATUS
MemRecNGetPsCfgUDIMM3Nb (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  )
{
  UINT32 AddrTmgCTL;
  UINT32 DctOdcCtl;
  UINT8 Dimms;
  UINT8  MaxDimmPerCH;
  UINT8 DramTerm;
  UINT8 DramTermDyn;

  if ((CurrentChannel->RegDimmPresent != 0) || (CurrentChannel->SODimmPresent != 0)) {
    return AGESA_UNSUPPORTED;
  }

  Dimms = CurrentChannel->Dimms;
  MaxDimmPerCH = RecGetMaxDimmsPerChannel (MemData->ParameterListPtr->PlatformMemoryConfiguration, 0, CurrentChannel->ChannelID);

  if (MaxDimmPerCH == 1) {
    return AGESA_UNSUPPORTED;
  } else {
    DctOdcCtl = 0x20223323;
    AddrTmgCTL = 0x00390039;
    if (Dimms == 1) {
      DctOdcCtl = 0x20113222;
      AddrTmgCTL = 0x00390039;
      if (CurrentChannel->Loads == 16) {
        AddrTmgCTL = 0x003B0000;
      }
    }
  }
  CurrentChannel->DctAddrTmg = AddrTmgCTL;
  CurrentChannel->DctOdcCtl = DctOdcCtl;

  // ODT
  if (Dimms == 1) {
    DramTerm = 1; // 60 ohms
    DramTermDyn = 0; // Disable
    if ((MaxDimmPerCH == 3) && (CurrentChannel->DimmDrPresent != 0)) {
      DramTermDyn = 1; // 60 ohms
    }
  } else {
    DramTerm = 3; // 40 ohms
    DramTermDyn = 2; // 120 ohms
  }
  CurrentChannel->Reserved[0] = DramTerm;
  CurrentChannel->Reserved[1] = DramTermDyn;

  // WL ODT
  if (Dimms == 1) {
    CurrentChannel->PhyWLODT[0] = 0;
    CurrentChannel->PhyWLODT[1] = (CurrentChannel->DimmDrPresent != 0) ? 8 : 2;
  } else {
    CurrentChannel->PhyWLODT[0] = 3;
    CurrentChannel->PhyWLODT[1] = 3;
  }
  CurrentChannel->PhyWLODT[2] = 0;
  CurrentChannel->PhyWLODT[3] = 0;

  return AGESA_SUCCESS;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This is function sets the platform specific settings for the systems with SODIMMs configuration
 *
 *     @param[in,out]   *MemData           Pointer to MEM_DATA_STRUCTURE
 *     @param[in]       SocketID        Socket number
 *     @param[in]       *CurrentChannel       Pointer to CH_DEF_STRUCT
 *
 *     @return          AGESA_SUCCESS
 *     @return          CurrentChannel->DctAddrTmg        Address Command Timing Settings for specified channel
 *     @return          CurrentChannel->DctOdcCtl         Drive Strength settings for specified channel
 *     @return          CurrentChannel->Reserved[0]       Dram Term for specified channel
 *     @return          CurrentChannel->Reserved[1]       Dynamic Dram Term for specified channel
 *     @return          CurrentChannel->PhyWLODT[0]       WL ODT for DIMM0
 *     @return          CurrentChannel->PhyWLODT[1]       WL ODT for DIMM1
 *     @return          CurrentChannel->PhyWLODT[2]       WL ODT for DIMM2
 *     @return          CurrentChannel->PhyWLODT[3]       WL ODT for DIMM3
 *
 */
AGESA_STATUS
MemRecNGetPsCfgSODIMM3Nb (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  )
{
  UINT32 AddrTmgCTL;
  UINT32 DctOdcCtl;
  UINT8  MaxDimmPerCH;
  UINT8 Dimms;
  UINT8 DramTerm;
  UINT8 DramTermDyn;

  if (CurrentChannel->SODimmPresent != CurrentChannel->ChDimmValid) {
    return AGESA_UNSUPPORTED;
  }

  Dimms = CurrentChannel->Dimms;
  MaxDimmPerCH = RecGetMaxDimmsPerChannel (MemData->ParameterListPtr->PlatformMemoryConfiguration, 0, CurrentChannel->ChannelID);

  if (MaxDimmPerCH == 1) {
    DctOdcCtl = 0x00113222;
    AddrTmgCTL = 0;
  } else {
    DctOdcCtl = 0x00223323;
    AddrTmgCTL = 0x00000039;
    if (Dimms == 1) {
      DctOdcCtl = 0x00113222;
      AddrTmgCTL = 0;
    }
  }
  CurrentChannel->DctAddrTmg = AddrTmgCTL;
  CurrentChannel->DctOdcCtl = DctOdcCtl;

  // ODT
  if (Dimms == 1) {
    DramTerm = 2; // 120 ohms
    DramTermDyn = 0; // Disable
    if (MaxDimmPerCH == 2) {
      DramTerm = 1; // 60 ohms
    }
  } else {
    DramTerm = 3; // 40 ohms
    DramTermDyn = 2; // 120 ohms
  }
  CurrentChannel->Reserved[0] = DramTerm;
  CurrentChannel->Reserved[1] = DramTermDyn;

  // WL ODT
  if (Dimms == 1) {
    if (MaxDimmPerCH == 1) {
      CurrentChannel->PhyWLODT[0] = (CurrentChannel->DimmDrPresent != 0) ? 4 : 1;
      CurrentChannel->PhyWLODT[1] = 0;
    } else {
      CurrentChannel->PhyWLODT[0] = 0;
      CurrentChannel->PhyWLODT[1] = (CurrentChannel->DimmDrPresent != 0) ? 8 : 2;
    }
  } else {
    CurrentChannel->PhyWLODT[0] = 3;
    CurrentChannel->PhyWLODT[1] = 3;
  }
  CurrentChannel->PhyWLODT[2] = 0;
  CurrentChannel->PhyWLODT[3] = 0;

  return AGESA_SUCCESS;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This is function sets the platform specific settings for the systems with RDIMMs configuration
 *
 *     @param[in,out]   *MemData           Pointer to MEM_DATA_STRUCTURE
 *     @param[in]       SocketID        Socket number
 *     @param[in]       *CurrentChannel       Pointer to CH_DEF_STRUCT
 *
 *     @return          AGESA_SUCCESS
 *     @return          CurrentChannel->DctAddrTmg        Address Command Timing Settings for specified channel
 *     @return          CurrentChannel->DctOdcCtl         Drive Strength settings for specified channel
 *     @return          CurrentChannel->Reserved[0]       Dram Term for specified channel
 *     @return          CurrentChannel->Reserved[1]       Dynamic Dram Term for specified channel
 *     @return          CurrentChannel->PhyWLODT[0]       WL ODT for DIMM0
 *     @return          CurrentChannel->PhyWLODT[1]       WL ODT for DIMM1
 *     @return          CurrentChannel->PhyWLODT[2]       WL ODT for DIMM2
 *     @return          CurrentChannel->PhyWLODT[3]       WL ODT for DIMM3
 *
 */

AGESA_STATUS
MemRecNGetPsCfgRDIMM3Nb (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  )
{
  STATIC CONST ADV_R_PSCFG_WL_ODT_ENTRY RecPSCfg2DIMMsWlODT[] = {
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
  STATIC CONST ADV_R_PSCFG_WL_ODT_ENTRY RecPSCfg3DIMMsWlODT[] = {
    {SR_DIMM2 + DR_DIMM2, {0x00, 0x00, 0x04, 0x00}, 1},
    {SR_DIMM0 + DR_DIMM0, {0x01, 0x02, 0x00, 0x00}, 1},
    {SR_DIMM0 + DR_DIMM0 + SR_DIMM2 + DR_DIMM2, {0x05, 0x00, 0x05, 0x00}, 2},
    {SR_DIMM0 + DR_DIMM0 + SR_DIMM1 + DR_DIMM1 + SR_DIMM2 + DR_DIMM2, {0x07, 0x07, 0x07, 0x00}, 3},
    {QR_DIMM1, {0x00, 0x0A, 0x00, 0x0A}, 1},
    {QR_DIMM1 + SR_DIMM2 + DR_DIMM2, {0x00, 0x06, 0x0E, 0x0C}, 2},
    {SR_DIMM0 + DR_DIMM0 + QR_DIMM1, {0x0B, 0x03, 0x00, 0x09}, 2},
    {SR_DIMM0 + DR_DIMM0 + QR_DIMM1 + SR_DIMM2 + DR_DIMM2, {0x0F, 0x07, 0x0F, 0x0D}, 3}
  };
  STATIC CONST ADV_R_PSCFG_WL_ODT_ENTRY RecPSCfg4DIMMsWlODT[] = {
    {ANY_DIMM3, {0x00, 0x00, 0x00, 0x08}, 1},
    {ANY_DIMM2 + ANY_DIMM3, {0x00, 0x00, 0x0C, 0x0C}, 2},
    {ANY_DIMM1 + ANY_DIMM2 + ANY_DIMM3, {0x00, 0x0E, 0x0E, 0x0E}, 3},
    {ANY_DIMM0 + ANY_DIMM1 + ANY_DIMM2 + ANY_DIMM3, {0x0F, 0x0F, 0x0F, 0x0F}, 4}
  };

  UINT8 i;
  UINT8 j;
  UINT8 Dimms;
  UINT8 DimmQrPresent;
  UINT32 AddrTmgCTL;
  UINT32 DctOdcCtl;
  UINT8 PhyWLODT[4];
  UINT8 DramTerm;
  UINT8 DramTermDyn;
  UINT16 DIMMRankType;
  UINT16 _DIMMRankType_;
  UINT8 DimmTpMatch;
  UINT8  MaxDimmPerCH;
  UINT8 PSCfgWlODTSize;
  CONST ADV_R_PSCFG_WL_ODT_ENTRY *PSCfgWlODTPtr;

  if (CurrentChannel->RegDimmPresent != CurrentChannel->ChDimmValid) {
    return AGESA_UNSUPPORTED;
  }

  DIMMRankType = MemRecNGetPsRankType (CurrentChannel);
  MaxDimmPerCH = RecGetMaxDimmsPerChannel (MemData->ParameterListPtr->PlatformMemoryConfiguration, 0, CurrentChannel->ChannelID);
  Dimms = CurrentChannel->Dimms;
  PSCfgWlODTPtr = RecPSCfg2DIMMsWlODT;
  PSCfgWlODTSize = GET_SIZE_OF (RecPSCfg2DIMMsWlODT);
  PhyWLODT[0] = PhyWLODT[1] = PhyWLODT[2] = PhyWLODT[3] = 0xFF;
  DimmQrPresent = CurrentChannel->DimmQrPresent;

  if (MaxDimmPerCH == 4) {
    AddrTmgCTL = (Dimms > 2) ? 0x002F0000 : 0;
    DctOdcCtl = (Dimms == 1) ? 0x20113222 : 0x20223222;
    PSCfgWlODTPtr = RecPSCfg4DIMMsWlODT;
    PSCfgWlODTSize = GET_SIZE_OF (RecPSCfg4DIMMsWlODT);
  } else  if (MaxDimmPerCH == 3) {
    AddrTmgCTL = 0;
    DctOdcCtl = 0x20223222;
    if (Dimms == 3) {
      AddrTmgCTL = 0x00380038;
      DctOdcCtl = 0x20113222;
    }
    if (Dimms == 1) {
      DctOdcCtl = 0x20113222;
    }
    PSCfgWlODTPtr = RecPSCfg3DIMMsWlODT;
    PSCfgWlODTSize = GET_SIZE_OF (RecPSCfg3DIMMsWlODT);
  } else if (MaxDimmPerCH == 2) {
    AddrTmgCTL = 0;
    DctOdcCtl = 0x20223222;
    if ((Dimms == 1) && (DimmQrPresent == 0)) {
      DctOdcCtl = 0x20113222;
    }
  } else {
    AddrTmgCTL = 0;
    DctOdcCtl = (DimmQrPresent == 0) ? 0x20113222 : 0x20223222;
  }
  CurrentChannel->DctAddrTmg = AddrTmgCTL;
  CurrentChannel->DctOdcCtl = DctOdcCtl;

  // ODT
  if (Dimms == 1) {
    DramTerm = 1; // 60 ohms
    DramTermDyn = 0; // Disable
    if (DimmQrPresent != 0) {
      DramTermDyn = 2; // 120 ohms
    }
  } else {
    DramTerm = 3; // 40 ohms
    DramTermDyn = 2; // 120 ohms
    if (DimmQrPresent != 0) {
      DramTerm = 1; // 60 ohms
    }
  }
  CurrentChannel->Reserved[0] = DramTerm;
  CurrentChannel->Reserved[1] = DramTermDyn;

  // WL ODT
  for (i = 0; i  < PSCfgWlODTSize; i++, PSCfgWlODTPtr++) {
    if (Dimms != PSCfgWlODTPtr->Dimms) {
      continue;
    }
    DimmTpMatch = 0;
    _DIMMRankType_ = DIMMRankType & PSCfgWlODTPtr->DIMMRankType;
    for (j = 0; j < MAX_DIMMS_PER_CHANNEL; j++) {
      if ((_DIMMRankType_ & (UINT16) 0x0F << (j << 2)) != 0) {
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
  CurrentChannel->PhyWLODT[0] = PhyWLODT[0];
  CurrentChannel->PhyWLODT[1] = PhyWLODT[1];
  CurrentChannel->PhyWLODT[2] = PhyWLODT[2];
  CurrentChannel->PhyWLODT[3] = PhyWLODT[3];

  return AGESA_SUCCESS;
}


/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function returns the max dimms for a given memory channel on a given
 *  processor.  It first searches the platform override table for the max dimms
 *  value.  If it is not provided, the AGESA default value is returned. The target
 *  socket must be a valid present socket.
 *
 *     @param[in] PlatformMemoryConfiguration - Platform config table
 *     @param[in] SocketID  - ID of the processor that owns the channel
 *     @param[in] ChannelID - Channel to get max dimms for
 *
 *
 *     @return UINT8 - Max Number of Dimms for that channel
 */
UINT8
RecGetMaxDimmsPerChannel (
  IN       PSO_TABLE *PlatformMemoryConfiguration,
  IN       UINT8 SocketID,
  IN       UINT8 ChannelID
  )
{
  UINT8  *DimmsPerChPtr;
  UINT8  MaxDimmPerCH;

  DimmsPerChPtr = MemRecFindPSOverrideEntry (PlatformMemoryConfiguration, PSO_MAX_DIMMS, SocketID, ChannelID, 0);
  if (DimmsPerChPtr != NULL) {
    MaxDimmPerCH = *DimmsPerChPtr;
  } else {
    MaxDimmPerCH = 2;
  }

  return MaxDimmPerCH;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *    This is the default return function of the ARDK block. The function always
 *    returns   AGESA_UNSUPPORTED
 *
 *     @param[in,out]   *MemData           Pointer to MEM_DATA_STRUCTURE
 *     @param[in]       SocketID        Socket number
 *     @param[in]       *CurrentChannel       Pointer to CH_DEF_STRUCT
 *
 *     @return          AGESA_UNSUPPORTED  AGESA status indicating that default is unsupported
 *
 */

AGESA_STATUS
MemRecNGetPsCfgDef (
  IN OUT   MEM_DATA_STRUCT *MemData,
  IN       UINT8 SocketID,
  IN OUT   CH_DEF_STRUCT *CurrentChannel
  )
{
  return AGESA_UNSUPPORTED;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *    This function returns the rank type map of a channel.
 *
 *     @param[in]       *CurrentChannel       Pointer to CH_DEF_STRUCT
 *
 *     @return          UINT16 - The map of rank type.
 *
 */
UINT16
MemRecNGetPsRankType (
  IN       CH_DEF_STRUCT *CurrentChannel
  )
{
  UINT8 i;
  UINT16 DIMMRankType;

  DIMMRankType = 0;
  for (i = 0; i < MAX_DIMMS_PER_CHANNEL; i++) {
    if ((CurrentChannel->DimmQrPresent & (UINT8) 1 << i) != 0) {
      if (i < 2) {
        DIMMRankType |= (UINT16) 4 << (i << 2);
      }
    } else if ((CurrentChannel->DimmDrPresent & (UINT8) 1 << i) != 0) {
      DIMMRankType |= (UINT16) 2 << (i << 2);
    } else if ((CurrentChannel->DimmSRPresent & (UINT8) 1 << i) != 0) {
      DIMMRankType |= (UINT16) 1 << (i << 2);
    }
  }
  return DIMMRankType;
}

UINT32
MemRecNcmnGetSetTrainDlyClientNb (
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
    // LN and ON do not support ECC delay, so:
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
  MemRecNSetBitFieldNb (NBPtr, BFDctAddlOffsetReg, Address);
  Value = MemRecNGetBitFieldNb (NBPtr, BFDctAddlDataReg);

  if (IsSet) {
    if (TrnDly == AccessPhRecDly) {
      Value = NBPtr->DctCachePtr->PhRecReg[Index & 0x03];
    }

    Value = ((UINT32)Field << Offset) | (Value & (~((UINT32) ((TrnDly == AccessRcvEnDly) ? 0x1FF : 0xFF) << Offset)));
    MemRecNSetBitFieldNb (NBPtr, BFDctAddlDataReg, Value);
    Address |= DCT_ACCESS_WRITE;
    MemRecNSetBitFieldNb (NBPtr, BFDctAddlOffsetReg, Address);

    if (TrnDly == AccessPhRecDly) {
      NBPtr->DctCachePtr->PhRecReg[Index & 0x03] = Value;
    }
    // Gross WrDatDly and WrDqsDly cannot be larger than 4
    ASSERT (((TrnDly == AccessWrDatDly) || (TrnDly == AccessWrDqsDly)) ? (NBPtr->IsSupported[WLNegativeDelay] || (Field < 0xA0)) : TRUE);
  } else {
    Value = (Value >> Offset) & (UINT32) ((TrnDly == AccessRcvEnDly) ? 0x1FF : 0xFF);
  }

  return Value;
}


/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function reads cache lines continuously using PRBS engine
 *
 *     @param[in,out] NBPtr  - Pointer to the MEM_NB_BLOCK
 *     @param[in,out] Buffer - Array of bytes to be filled with data read from DRAM
 *     @param[in]     Address - System Address [47:16]
 *     @param[in] ClCount - Number of cache lines
 *
 */

VOID
MemRecNContReadPatternUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT32 Address,
  IN       UINT16 ClCount
  )
{
  MemRecNCommonReadWritePatternUnb (NBPtr, CMD_TYPE_READ, ClCount);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function generates a continuous stream of writes to DRAM using the
 *       Unified Northbridge Reliable Read/Write Engine.
 *
 *     @param[in,out] NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out] Address - Unused by this function
 *     @param[in]     Pattern - Unused by this function
 *     @param[in]     ClCount - Number of cache lines to write
 *
 */

VOID
MemRecNContWritePatternUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 Address,
  IN       UINT8 Pattern[],
  IN       UINT16 ClCount
  )
{
  MemRecNCommonReadWritePatternUnb (NBPtr, CMD_TYPE_WRITE, ClCount);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function generates either read or write DRAM cycles for training
 *     using PRBS engine
 *
 *     @param[in,out] NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]     CmdType - Read/Write
 *     @param[in]     ClCount - Number of cache lines to write
 *
 */

VOID
STATIC
MemRecNCommonReadWritePatternUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8  CmdType,
  IN       UINT16 ClCount
  )
{
  MEM_TECH_BLOCK *TechPtr;

  TechPtr = NBPtr->TechPtr;

  // Enable PRBS, also set ResetAllErr
  MemRecNSetBitFieldNb (NBPtr, BFCmdTestEnable, 3);

  // Send activate command
  MemRecNSetBitFieldNb (NBPtr, BFDramCmd2Reg, (UINT32) 1 << (TechPtr->ChipSel + 22) | ((UINT32) 1 << 31));
  MemRecUWait10ns (750, NBPtr->MemPtr);

  // Setup test address
  MemRecNSetBitFieldNb (NBPtr, BFTgtChipSelectA, TechPtr->ChipSel);
  MemRecNSetBitFieldNb (NBPtr, BFDataPrbsSeed, PRBS_SEED_256);
  MemRecNSetBitFieldNb (NBPtr, BFCmdCount, ClCount);

  // Select read or write command
  MemRecNSetBitFieldNb (NBPtr, BFCmdType, CmdType);

  // Send command and wait for completion
  MemRecNSetBitFieldNb (NBPtr, BFSendCmd, 1);
  while (MemRecNGetBitFieldNb (NBPtr, BFTestStatus) == 0) {}
  while (MemRecNGetBitFieldNb (NBPtr, BFCmdSendInProg) != 0) {}
  MemRecNSetBitFieldNb (NBPtr, BFSendCmd, 0);

  // Send precharge all command
  MemRecNSetBitFieldNb (NBPtr, BFDramCmd2Reg, (UINT32) 1 << (TechPtr->ChipSel + 22) | ((UINT32) 1 << 30));
  MemRecUWait10ns (750, NBPtr->MemPtr);

  // Disable PRBS
  MemRecNSetBitFieldNb (NBPtr, BFCmdTestEnable, 0);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function checks the Error status bits for comparison results using PRBS
 *
 *     @param[in,out]   *NBPtr    - Pointer to the MEM_NB_BLOCK
 *     @param[in]       Buffer[]  -  Not used in this implementation
 *     @param[in]       Pattern[] - Not used in this implementation
 *     @param[in]       ByteCount - Not used in this implementation
 *
 *     @return  PASS - Bitmap of results of comparison
 */

UINT16
MemRecNCompareTestPatternUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT8 Pattern[],
  IN       UINT16 ByteCount
  )
{
  UINT16 i;
  UINT16 Pass;
  UINT32 NibbleErrSts;

  NibbleErrSts = MemRecNGetBitFieldNb (NBPtr, BFNibbleErrSts);

  Pass = 0;
  for (i = 0; i < 8; i++) {
    Pass |= ((NibbleErrSts & 0x03) > 0 ) ? (1 << i) : 0;
    NibbleErrSts >>= 2;
  }
  Pass = ~Pass;
  return Pass;
}

