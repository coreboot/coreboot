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
 * @e \$Revision: 6781 $ @e \$Date: 2008-07-17 12:11:08 -0500 (Thu, 17 Jul 2008) $
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
#include "cpuFamRegisters.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "mru.h"
#include "Filecode.h"
#define FILECODE PROC_RECOVERY_MEM_NB_MRNDCT_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define RECDEF_DRAM_CONTROL_REG   0x320C2A06
#define RECDEF_DRAM_MRSREG        0x000400A4
#define RECDEF_DRAM_TIMING_LO     0x000A0092
#define RECDEF_DRAM_TIMING_HI     0xB6D218FF
#define RECDEF_CSMASK_REG         0x00083FE0
#define RECDEF_DRAM_CONFIG_LO_REG 0x00000000
#define RECDEF_DRAM_CONFIG_HI_REG 0x1F48010B
#define RECDEF_DRAM_BASE_REG      0x00000003


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
  UINT32 RegValue;
  UINT8 ChipSel;
  UINT32 CSBase;
  DCT_STRUCT *DCTPtr;
  CH_DEF_STRUCT *ChannelPtr;

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

  MemRecNSetBitFieldNb (NBPtr, BFDramBankAddrReg, 0x00001111);

  // Set timing registers
  MemRecNSetBitFieldNb (NBPtr, BFDramTimingLoReg, RECDEF_DRAM_TIMING_LO);
  MemRecNSetBitFieldNb (NBPtr, BFDramTimingHiReg, RECDEF_DRAM_TIMING_HI);
  MemRecNSetBitFieldNb (NBPtr, BFDramMRSReg, RECDEF_DRAM_MRSREG);
  MemRecNSetBitFieldNb (NBPtr, BFDramControlReg, RECDEF_DRAM_CONTROL_REG);

  // Set DRAM Config Low Register
  RegValue = RECDEF_DRAM_CONFIG_LO_REG;
  // Set x4Dimm based on DIMM type
  if ((NBPtr->ChannelPtr->Dimmx4Present & ((UINT8) 1 << Dimm)) != 0) {
    RegValue |= ((UINT32) 1 << (Dimm + 12));
  }
  // If not registered, set unbuffered DIMM
  if (!(NBPtr->ChannelPtr->RegDimmPresent & ((UINT8) 1 << Dimm))) {
    RegValue |= ((UINT32) 1 << 16);
  }
  MemRecNSetBitFieldNb (NBPtr, BFDramConfigLoReg, RegValue);

  // Set DRAM Config High Register
  RegValue = RECDEF_DRAM_CONFIG_HI_REG;
  MemRecNSetBitFieldNb (NBPtr, BFDramConfigHiReg, RegValue);

  //======================================================================
  // Build Dram Config Misc Register Value
  //======================================================================
  //
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
  UINT8 MaxDimmPerCH;
  UINT32 AddrTmgValue;
  UINT32 DrvStrValue;
  UINT8 *DimmsPerChPtr;
  CH_DEF_STRUCT *ChannelPtr;

  ChannelPtr = NBPtr->ChannelPtr;
  DimmsPerChPtr = MemRecFindPSOverrideEntry (NBPtr->RefPtr->PlatformMemoryConfiguration, PSO_MAX_DIMMS, 0, ChannelPtr->ChannelID);
  if (DimmsPerChPtr != NULL) {
    MaxDimmPerCH = *DimmsPerChPtr;
  } else {
    MaxDimmPerCH = 2;
  }

  if (ChannelPtr->RegDimmPresent) {
    if (MaxDimmPerCH == 2) {
      DrvStrValue = 0x20223222;
      AddrTmgValue = 0;
      if ((ChannelPtr->Dimms == 1) && (ChannelPtr->DimmQrPresent == 0)) {
        DrvStrValue = 0x20113222;
      }
    } else if (MaxDimmPerCH == 3) {
      DrvStrValue = 0x20223222;
      AddrTmgValue = 0;
      if ((ChannelPtr->Dimms == 1) && (ChannelPtr->DimmQrPresent == 0)) {
        DrvStrValue = 0x20113222;
      } else if (ChannelPtr->Dimms == 3) {
        AddrTmgValue = 0x00380038;
      }
    } else {
      DrvStrValue = 0x20223222;
      AddrTmgValue = 0;
      if (ChannelPtr->Dimms >= 3) {
        AddrTmgValue = 0x002F0000;
      } else if (ChannelPtr->Dimms == 1) {
        DrvStrValue = 0x20113222;
      }
    }
  } else {
    if (ChannelPtr->Dimms == 2) {
      DrvStrValue = 0x20223323;
      AddrTmgValue = 0x00390039;
    } else {
      DrvStrValue = 0x20113222;
      AddrTmgValue = 0;
      if ((ChannelPtr->Dimms == 1) && (ChannelPtr->Loads >= 16)) {
        AddrTmgValue = 0x003B0000;
      }
    }
  }
  MemRecNSetBitFieldNb (NBPtr, BFODCControl, DrvStrValue);
  MemRecNSetBitFieldNb (NBPtr, BFAddrTmgControl, AddrTmgValue);
  return TRUE;
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
  UINT8 Dimms;
  UINT8 DramTerm;
  UINT8 DramTermDyn;
  BOOLEAN IsDualRank;

  Dimms = NBPtr->ChannelPtr->Dimms;
  IsDualRank = (NBPtr->DCTPtr->Timings.CsPresent & (ChipSelect + 1)) ? TRUE : FALSE;

  if (OdtMode == WRITE_LEVELING_MODE) {
    DramTermDyn = 0;  // Disabled

    if (ChipSelect & 1) {
      DramTerm = 0;   // Disabled
    } else if (Dimms == 1) {
      DramTerm = 1;   // 60 Ohms
    } else {
      DramTerm = 3;   // 40 Ohms
    }

    if (ChipSelect == TargetCS) {
      if (Dimms == 2) {
        DramTerm = 2;   // 120 Ohms
      } else if (IsDualRank) {
        DramTerm = 0;   // Disabled
      }

      // Program WrLvOdt
      MemRecNSetBitFieldNb (NBPtr, BFWrLvOdt, 0x0F);
    } else if (ChipSelect == (TargetCS + 1)) {
      if (Dimms == 1) {
        DramTerm = 1;   // 60 Ohms
      }
    }
  } else {
    // Dram nominal termination
    if (Dimms == 1) {
      DramTerm = 1;   // 60 Ohms
    } else {
      DramTerm = 3;   // 40 Ohms
    }

    // Dram dynamic termination
    if (Dimms < 2) {
      DramTermDyn = 0;    // Disabled
    } else {
      DramTermDyn = 2;    // 120 Ohms
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

  IDS_HDT_CONSOLE ("\t\t\tCS%d MR%d %04x\n",
              (MemRecNGetBitFieldNb (NBPtr, BFDramInitRegReg) >> 20) & 0xF,
              (MemRecNGetBitFieldNb (NBPtr, BFDramInitRegReg) >> 16) & 0xF,
              (MemRecNGetBitFieldNb (NBPtr, BFDramInitRegReg) & 0xFFFF));

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
