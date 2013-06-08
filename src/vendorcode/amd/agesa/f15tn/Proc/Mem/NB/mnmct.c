/* $NoKeywords:$ */
/**
 * @file
 *
 * mnmct.c
 *
 * Northbridge Common MCT supporting functions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB)
 * @e \$Revision: 63425 $ @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 **/
/*****************************************************************************
*
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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
#include "mu.h"
#include "OptionMemory.h"
#include "PlatformMemoryConfiguration.h"
#include "GeneralServices.h"
#include "cpuFeatures.h"
#include "merrhdl.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_NB_MNMCT_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define _16MB_RJ16  0x0100

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
BOOLEAN
STATIC
MemNSetMTRRrangeNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 Base,
  IN OUT   UINT32 *LimitPtr,
  IN       UINT32 MtrrAddr,
  IN       UINT8 MtrrType
  );

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
extern BUILD_OPT_CFG UserOptions;

/* -----------------------------------------------------------------------------*/
/**
 *
 *     Get max frequency from OEM platform definition, from
 *     any user override (limiting) of max frequency, and
 *     from any Si Revision Specific information.  Return
 *     the least of these three in DIE_STRUCT.Timings.TargetSpeed.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNSyncTargetSpeedNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  CONST UINT16 DdrMaxRateTab[] = {
    UNSUPPORTED_DDR_FREQUENCY,
    DDR1600_FREQUENCY,
    DDR1333_FREQUENCY,
    DDR1066_FREQUENCY,
    DDR800_FREQUENCY,
    DDR667_FREQUENCY,
    DDR533_FREQUENCY,
    DDR400_FREQUENCY
  };

  UINT8  Dct;
  UINT8  Channel;
  UINT16 MinSpeed;
  UINT16 DdrMaxRate;
  DCT_STRUCT *DCTPtr;
  USER_MEMORY_TIMING_MODE *ChnlTmgMod;
  USER_MEMORY_TIMING_MODE Mode[MAX_CHANNELS_PER_SOCKET];
  MEMORY_BUS_SPEED MemClkFreq;
  MEMORY_BUS_SPEED ProposedFreq;

  ASSERT (NBPtr->DctCount <= sizeof (Mode));
  MinSpeed = 16000;
  DdrMaxRate = 16000;
  if (NBPtr->IsSupported[CheckMaxDramRate]) {
    // Check maximum DRAM data rate that the processor is designed to support.
    DdrMaxRate = DdrMaxRateTab[MemNGetBitFieldNb (NBPtr, BFDdrMaxRate)];
    NBPtr->FamilySpecificHook[GetDdrMaxRate] (NBPtr, &DdrMaxRate);
    IDS_OPTION_HOOK (IDS_SKIP_FUSED_MAX_RATE, &DdrMaxRate, &NBPtr->MemPtr->StdHeader);
  }

  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    MemNSwitchDCTNb (NBPtr, Dct);
    DCTPtr = NBPtr->DCTPtr;

    // Check if input user time mode is valid or not
    ASSERT ((NBPtr->RefPtr->UserTimingMode == TIMING_MODE_SPECIFIC) ||
        (NBPtr->RefPtr->UserTimingMode == TIMING_MODE_LIMITED) ||
        (NBPtr->RefPtr->UserTimingMode == TIMING_MODE_AUTO));
    Mode[Dct] = NBPtr->RefPtr->UserTimingMode;
    // Check if input clock value is valid or not
    ASSERT ((NBPtr->ChannelPtr->TechType == DDR3_TECHNOLOGY) ?
      (NBPtr->RefPtr->MemClockValue >= DDR667_FREQUENCY) :
      (NBPtr->RefPtr->MemClockValue <= DDR1066_FREQUENCY));
    MemClkFreq = NBPtr->RefPtr->MemClockValue;
    if (DCTPtr->Timings.DctDimmValid != 0) {
      Channel = MemNGetSocketRelativeChannelNb (NBPtr, Dct, 0);
      ChnlTmgMod = (USER_MEMORY_TIMING_MODE *) FindPSOverrideEntry (NBPtr->RefPtr->PlatformMemoryConfiguration, PSO_BUS_SPEED, NBPtr->MCTPtr->SocketId, Channel, 0,
                                                                    &(NBPtr->MCTPtr->LogicalCpuid), &(NBPtr->MemPtr->StdHeader));
      if (ChnlTmgMod != NULL) {
        // Check if input user timing mode is valid or not
        ASSERT ((ChnlTmgMod[0] == TIMING_MODE_SPECIFIC) || (ChnlTmgMod[0] == TIMING_MODE_LIMITED) ||
          (ChnlTmgMod[0] != TIMING_MODE_AUTO));
        if (ChnlTmgMod[0] != TIMING_MODE_AUTO) {
          Mode[Dct] = ChnlTmgMod[0];
          // Check if input clock value is valid or not
          ASSERT ((NBPtr->ChannelPtr->TechType == DDR3_TECHNOLOGY) ?
            ((MEMORY_BUS_SPEED)ChnlTmgMod[1] >= DDR667_FREQUENCY) :
            ((MEMORY_BUS_SPEED)ChnlTmgMod[1] <= DDR1066_FREQUENCY));
          MemClkFreq = (MEMORY_BUS_SPEED)ChnlTmgMod[1];
        }
      }

      ProposedFreq = UserOptions.CfgMemoryBusFrequencyLimit;
      if (Mode[Dct] == TIMING_MODE_LIMITED) {
        if (MemClkFreq < ProposedFreq) {
          ProposedFreq = MemClkFreq;
        }
      } else if (Mode[Dct] == TIMING_MODE_SPECIFIC) {
        ProposedFreq = MemClkFreq;
      }

      if (Mode[Dct] == TIMING_MODE_SPECIFIC) {
        DCTPtr->Timings.TargetSpeed = (UINT16) ProposedFreq;
      } else {
        // "limit" mode
        if (DCTPtr->Timings.TargetSpeed > ProposedFreq) {
          DCTPtr->Timings.TargetSpeed = (UINT16) ProposedFreq;
        }
      }

      NBPtr->MemNCapSpeedBatteryLife (NBPtr);

      if (DCTPtr->Timings.TargetSpeed > DdrMaxRate) {
        if (Mode[Dct] == TIMING_MODE_SPECIFIC) {
          PutEventLog (AGESA_ALERT, MEM_ALERT_USER_TMG_MODE_OVERRULED, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
          SetMemError (AGESA_ALERT, NBPtr->MCTPtr);
        }
        DCTPtr->Timings.TargetSpeed = DdrMaxRate;
      }

      IDS_SKIP_HOOK (IDS_POR_MEM_FREQ, NBPtr, &NBPtr->MemPtr->StdHeader) {
        //
        //Call Platform POR Frequency Override
        //
        if (!MemProcessConditionalOverrides (NBPtr->RefPtr->PlatformMemoryConfiguration, NBPtr, PSO_ACTION_SPEEDLIMIT, ALL_DIMMS)) {
          //
          // Get the POR frequency limit
          //
          NBPtr->PsPtr->MemPGetPORFreqLimit (NBPtr);
        }
      }
      IDS_OPTION_HOOK (IDS_STRETCH_FREQUENCY_LIMIT, NBPtr, &NBPtr->MemPtr->StdHeader);

      if (MinSpeed > DCTPtr->Timings.TargetSpeed) {
        MinSpeed = DCTPtr->Timings.TargetSpeed;
      }
    }
  }

  if (MinSpeed == DDR667_FREQUENCY) {
    NBPtr->StartupSpeed = DDR667_FREQUENCY;
  }

  // Sync all DCTs to the same speed
  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    MemNSwitchDCTNb (NBPtr, Dct);
    NBPtr->DCTPtr->Timings.TargetSpeed = MinSpeed;
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function waits for all DCTs to be ready
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */

BOOLEAN
MemNSyncDctsReadyNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  if (NBPtr->MCTPtr->DimmValid) {
    MemNPollBitFieldNb (NBPtr, BFDramEnabled, 1, PCI_ACCESS_TIMEOUT, FALSE);
    // Re-enable phy compensation engine after Dram init has completed
    MemNSwitchDCTNb (NBPtr, 0);
    MemNSetBitFieldNb (NBPtr, BFDisAutoComp, 0);
  }
  // Wait 750 us for the phy compensation engine to reinitialize.
  MemUWait10ns (75000, NBPtr->MemPtr);

  MemNSyncAddrMapToAllNodesNb (NBPtr);
  return (BOOLEAN) (NBPtr->MCTPtr->ErrCode < AGESA_FATAL);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function create the HT memory map
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */

BOOLEAN
MemNHtMemMapInitNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT32 BottomIo;
  UINT32 HoleOffset;
  UINT32 DctSelBaseAddr;
  UINT32 NodeSysBase;
  UINT32 NodeSysLimit;
  MEM_PARAMETER_STRUCT *RefPtr;
  DIE_STRUCT *MCTPtr;

  RefPtr = NBPtr->RefPtr;
  MCTPtr = NBPtr->MCTPtr;
  //
  // Physical addresses in this function are right adjusted by 16 bits ([47:16])
  // They are BottomIO, HoleOffset, DctSelBaseAddr, NodeSysBase, NodeSysLimit.
  //

  // Enforce bottom of IO be be 128MB aligned
  ASSERT ((RefPtr->BottomIo < (_4GB_RJ16 >> 8)) && (RefPtr->BottomIo != 0));
  BottomIo = (RefPtr->BottomIo & 0xF8) << 8;

  if (!MCTPtr->GangedMode) {
    DctSelBaseAddr = MCTPtr->DctData[0].Timings.DctMemSize;
  } else {
    DctSelBaseAddr = 0;
  }

  if (MCTPtr->NodeMemSize) {
    NodeSysBase = NBPtr->SharedPtr->CurrentNodeSysBase;
    NodeSysLimit = NodeSysBase + MCTPtr->NodeMemSize - 1;
    DctSelBaseAddr += NodeSysBase;

    if ((NBPtr->IsSupported[ForceEnMemHoleRemapping]) || (RefPtr->MemHoleRemapping)) {
      if ((NodeSysBase < BottomIo) && (NodeSysLimit >= BottomIo)) {
        // HW Dram Remap
        MCTPtr->Status[SbHWHole] = TRUE;
        RefPtr->GStatus[GsbHWHole] = TRUE;
        MCTPtr->NodeHoleBase = BottomIo;
        RefPtr->HoleBase = BottomIo;

        HoleOffset = _4GB_RJ16 - BottomIo;

        NodeSysLimit += HoleOffset;

        if ((DctSelBaseAddr > 0) && (DctSelBaseAddr < BottomIo)) {
          HoleOffset += DctSelBaseAddr;
        } else {
          if (DctSelBaseAddr >= BottomIo) {
            DctSelBaseAddr += HoleOffset;
          }
          HoleOffset += NodeSysBase;
        }

        MemNSetBitFieldNb (NBPtr, BFDramHoleBase, BottomIo >> 8);
        MemNSetBitFieldNb (NBPtr, BFDramHoleOffset, HoleOffset >> 7);
        MemNSetBitFieldNb (NBPtr, BFDramHoleValid, 1);

      } else if (NodeSysBase == BottomIo) {
        // SW Node Hoist
        MCTPtr->Status[SbSWNodeHole] = TRUE;
        RefPtr->GStatus[GsbSpIntRemapHole] = TRUE;
        RefPtr->GStatus[GsbSoftHole] = TRUE;

        RefPtr->HoleBase = NodeSysBase;
        DctSelBaseAddr = _4GB_RJ16 + (DctSelBaseAddr - NodeSysBase);
        NodeSysLimit = _4GB_RJ16 + (NodeSysLimit - NodeSysBase);
        NodeSysBase = _4GB_RJ16;

      } else if ((NodeSysBase < HT_REGION_BASE_RJ16) && (NodeSysLimit >= HT_REGION_BASE_RJ16)) {
        if (!NBPtr->SharedPtr->UndoHoistingAbove1TB) {
          // SW Hoisting above 1TB to avoid HT Reserved region
          DctSelBaseAddr = _1TB_RJ16 + (DctSelBaseAddr - NodeSysBase);
          NodeSysLimit = _1TB_RJ16 + (NodeSysLimit - NodeSysBase);
          NodeSysBase = _1TB_RJ16;

          if (RefPtr->LimitMemoryToBelow1Tb) {
            // Flag to undo 1TB hoisting after training
            NBPtr->SharedPtr->UndoHoistingAbove1TB = TRUE;
          }
        }

      } else {
        // No Remapping.  Normal Contiguous mapping
      }
    } else {
      // No Remapping.  Normal Contiguous mapping
    }

    if (NBPtr->IsSupported[Check1GAlign]) {
      if (UserOptions.CfgNodeMem1GBAlign) {
        NBPtr->MemPNodeMemBoundaryNb (NBPtr, (UINT32 *)&NodeSysLimit);
      }
    }

    MCTPtr->NodeSysBase = NodeSysBase;
    MCTPtr->NodeSysLimit = NodeSysLimit;
    RefPtr->SysLimit = NodeSysLimit;
    RefPtr->Sub1THoleBase = (NodeSysLimit < HT_REGION_BASE_RJ16) ? (NodeSysLimit + 1) : RefPtr->Sub1THoleBase;
    IDS_OPTION_HOOK (IDS_MEM_SIZE_OVERLAY, NBPtr, &NBPtr->MemPtr->StdHeader);

    NBPtr->SharedPtr->TopNode = NBPtr->Node;

    NBPtr->SharedPtr->NodeMap[NBPtr->Node].IsValid = TRUE;
    NBPtr->SharedPtr->NodeMap[NBPtr->Node].SysBase = NodeSysBase;
    NBPtr->SharedPtr->NodeMap[NBPtr->Node].SysLimit = NodeSysLimit & 0xFFFFFF00;

    MemNSetBitFieldNb (NBPtr, BFDramBaseAddr, NodeSysBase >> (27 - 16));
    MemNSetBitFieldNb (NBPtr, BFDramLimitAddr, NodeSysLimit >> (27 - 16));

    if ((MCTPtr->DctData[1].Timings.DctMemSize != 0) && (!NBPtr->Ganged)) {
      MemNSetBitFieldNb (NBPtr, BFDctSelBaseAddr, DctSelBaseAddr >> 11);
      MemNSetBitFieldNb (NBPtr, BFDctSelHiRngEn, 1);
      MemNSetBitFieldNb (NBPtr, BFDctSelHi, 1);
      MemNSetBitFieldNb (NBPtr, BFDctSelBaseOffset, DctSelBaseAddr >> 10);
    }

    NBPtr->SharedPtr->CurrentNodeSysBase = (NodeSysLimit + 1) & 0xFFFFFFF0;
  }
  return (BOOLEAN) (MCTPtr->ErrCode < AGESA_FATAL);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *     Program system DRAM map to this node
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNSyncAddrMapToAllNodesNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Node;
  UINT32 NodeSysBase;
  UINT32 NodeSysLimit;
  UINT8  WeReMask;
  MEM_PARAMETER_STRUCT *RefPtr;

  RefPtr = NBPtr->RefPtr;
  for (Node = 0; Node < NBPtr->NodeCount; Node++) {
    NodeSysBase = NBPtr->SharedPtr->NodeMap[Node].SysBase;
    NodeSysLimit = NBPtr->SharedPtr->NodeMap[Node].SysLimit;
    if (NBPtr->SharedPtr->NodeMap[Node].IsValid) {
      WeReMask = 3;
    } else {
      WeReMask = 0;
    }
    // Set the Dram base and set the WE and RE flags in the base.
    MemNSetBitFieldNb (NBPtr, BFDramBaseReg0 + Node, (NodeSysBase << 8) | WeReMask);
    MemNSetBitFieldNb (NBPtr, BFDramBaseHiReg0 + Node, NodeSysBase >> 24);
    // Set the Dram limit and set DstNode.
    MemNSetBitFieldNb (NBPtr, BFDramLimitReg0 + Node, (NodeSysLimit << 8) | Node);
    MemNSetBitFieldNb (NBPtr, BFDramLimitHiReg0 + Node, NodeSysLimit >> 24);

    if (RefPtr->GStatus[GsbHWHole]) {
      MemNSetBitFieldNb (NBPtr, BFDramMemHoistValid, 1);
      MemNSetBitFieldNb (NBPtr, BFDramHoleBase, (RefPtr->HoleBase >> 8));
    }
  }

  NBPtr->FamilySpecificHook[InitExtMMIOAddr] (NBPtr, NULL);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function enables power down mode
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNPowerDownCtlNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  MEM_PARAMETER_STRUCT *RefPtr;
  UINT8 PowerDownMode;

  RefPtr = NBPtr->RefPtr;

  // we can't enable powerdown mode when doing WL
  if (RefPtr->EnablePowerDown) {
    MemNSetBitFieldNb (NBPtr, BFPowerDownEn, 1);
    PowerDownMode = (UINT8) ((UserOptions.CfgPowerDownMode == POWER_DOWN_MODE_AUTO) ? POWER_DOWN_BY_CHANNEL : UserOptions.CfgPowerDownMode);
    IDS_OPTION_HOOK (IDS_POWERDOWN_MODE, &PowerDownMode, &(NBPtr->MemPtr->StdHeader));
    if (PowerDownMode) {
      MemNSetBitFieldNb (NBPtr, BFPowerDownMode, 1);
    }
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function gets the Optimal Critical Gross Delay Difference between
 *   the delay parameters across all Dimms on each bytelane.  Then takes the
 *   largest of all the bytelanes.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       TrnDly1 - Type of first Gross Delay parameter
 *     @param[in]       TrnDly2 - Type of second Gross Delay parameter
 *
 *     @return  The largest difference between the largest and smallest
 *               of the two Gross delay types within a single bytelane
 */
INT8
MemNGetOptimalCGDDNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       TRN_DLY_TYPE TrnDly1,
  IN       TRN_DLY_TYPE TrnDly2
  )
{
  INT8  CGDD;
  INT8  GDD;
  UINT8  Dimm1;
  UINT8  Dimm2;
  UINT8  ByteLane;
  UINT16 CsEnabled;
  BOOLEAN CGDDInit;
  BOOLEAN SameDelayType;

  CGDD = 0;
  CGDDInit = FALSE;
  SameDelayType = (BOOLEAN) (TrnDly1 == TrnDly2);
  CsEnabled = NBPtr->DCTPtr->Timings.CsEnabled;

  // If the two delay types compared are the same type, then no need to compare the same
  // pair twice. Adjustments are made in the upper bound and lower bound of the loop to
  // handle this.
  for (Dimm1 = 0; Dimm1 < (SameDelayType ? (MAX_DIMMS_PER_CHANNEL - 1) : MAX_DIMMS_PER_CHANNEL); Dimm1 ++) {
    if (CsEnabled & (UINT16) (3 << (Dimm1 << 1))) {
      for (Dimm2 = (SameDelayType ? (Dimm1 + 1) : 0); Dimm2 < MAX_DIMMS_PER_CHANNEL; Dimm2 ++) {
        if ((CsEnabled & (UINT16) (3 << (Dimm2 << 1)))) {
          for (ByteLane = 0 ; ByteLane < 8 ; ByteLane++) {
            // check each byte lane delay pair
            GDD = (UINT8) (NBPtr->GetTrainDly (NBPtr, TrnDly1, DIMM_BYTE_ACCESS (Dimm1, ByteLane)) >> 5) -
                  (UINT8) (NBPtr->GetTrainDly (NBPtr, TrnDly2, DIMM_BYTE_ACCESS (Dimm2, ByteLane)) >> 5);
            // If the 2 delay types to be compared are the same, then keep the absolute difference
            if (SameDelayType && (GDD < 0)) {
              GDD = (-GDD);
            }

            // If CGDD is yet to be initialized, initialize it
            // Otherwise, keep the largest difference so far
            CGDD = (!CGDDInit) ? GDD : ((CGDD > GDD) ? CGDD : GDD);
            if (!CGDDInit) {
              CGDDInit = TRUE;
            }
          }
        }
      }
    }
  }
  return CGDD;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function calculates the critical delay difference (CDD)
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       TrnDlyType1 - Type of first Gross Delay parameter
 *     @param[in]       TrnDlyType2 - Type of second Gross Delay parameter
 *     @param[in]       SameDimm - CDD of same DIMMs
 *     @param[in]       DiffDimm - CDD of different DIMMs
 *
 *     @return          CDD term - in 1/2 MEMCLK
 */
INT16
MemNCalcCDDNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       TRN_DLY_TYPE TrnDlyType1,
  IN       TRN_DLY_TYPE TrnDlyType2,
  IN       BOOLEAN SameDimm,
  IN       BOOLEAN DiffDimm
  )
{
  INT16 CDD;
  INT16 CDDtemp;
  UINT16 TrnDly1;
  UINT16 TrnDly2;
  UINT8 i;
  UINT8 j;
  UINT8 ByteLane;
  UINT16 CsEnabled;
  BOOLEAN SameDlyType;

  SameDlyType = (BOOLEAN) (TrnDlyType1 == TrnDlyType2);
  CsEnabled = NBPtr->DCTPtr->Timings.CsEnabled;
  CDD = -127;
  // If the two delay types compared are the same type, then no need to compare the same
  // pair twice. Adjustments are made in the upper bound and lower bound of the loop to
  // handle this.
  for (i = 0; i < (SameDlyType ? (NBPtr->CsPerChannel - NBPtr->CsPerDelay) : NBPtr->CsPerChannel); i = i + NBPtr->CsPerDelay) {
    if ((CsEnabled & ((UINT16) ((NBPtr->CsPerDelay == 2) ? 3 : 1) << i)) != 0) {
      for (j = SameDlyType ? (i + NBPtr->CsPerDelay) : 0; j < NBPtr->CsPerChannel; j = j + NBPtr->CsPerDelay) {
        if (((CsEnabled & ((UINT16) ((NBPtr->CsPerDelay == 2)? 3 : 1) << j)) != 0) &&
            ((SameDimm && ((i / 2) == (j / 2))) || (DiffDimm && ((i / 2) != (j / 2))))) {
          for (ByteLane = 0; ByteLane < ((NBPtr->MCTPtr->Status[SbEccDimms] && NBPtr->IsSupported[EccByteTraining]) ? 9 : 8); ByteLane++) {
            /// @todo: Gross delay mask should not be constant.
            TrnDly1 = GetTrainDlyFromHeapNb (NBPtr, TrnDlyType1, DIMM_BYTE_ACCESS (i / NBPtr->CsPerDelay, ByteLane)) >> 5;  // Gross delay only
            TrnDly2 = GetTrainDlyFromHeapNb (NBPtr, TrnDlyType2, DIMM_BYTE_ACCESS (j / NBPtr->CsPerDelay, ByteLane)) >> 5;  // Gross delay only

            CDDtemp = TrnDly1 - TrnDly2;
            // If the 2 delay types to be compared are the same, then keep the absolute difference
            if ((SameDlyType) && (CDDtemp < 0)) {
              CDDtemp = (-CDDtemp);
            }

            CDD = (CDD < CDDtemp) ? CDDtemp : CDD;
          }
        }
      }
    }
  }

  return CDD;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function gets DQS timing from data saved in heap.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]   TrnDlyType - type of delay to be set
 *     @param[in]   Drbn - encoding of Dimm-Rank-Byte-Nibble to be accessed
 *                  (use either DIMM_BYTE_ACCESS(dimm,byte) or CS_NBBL_ACCESS(cs,nibble) to use this encoding
 *
 *     @return      value of the target timing.
 */
UINT16
GetTrainDlyFromHeapNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       TRN_DLY_TYPE TrnDlyType,
  IN       DRBN Drbn
  )
{
  UINT8 Dimm;
  UINT8 Byte;
  UINT16 TrainDly;
  CH_DEF_STRUCT *ChannelPtr;
  MEM_TECH_BLOCK *TechPtr;

  Dimm = DRBN_DIMM (Drbn);
  Byte = DRBN_BYTE (Drbn);
  ChannelPtr = NBPtr->ChannelPtr;
  TechPtr = NBPtr->TechPtr;

  ASSERT (Dimm < (NBPtr->CsPerChannel / NBPtr->CsPerDelay));
  ASSERT (Byte <= ECC_DLY);

  if (NBPtr->MemPstate == MEMORY_PSTATE1) {
    switch (TrnDlyType) {
    case AccessRcvEnDly:
      TrainDly = ChannelPtr->RcvEnDlysMemPs1[Dimm * TechPtr->DlyTableWidth () + Byte];
      break;
    case AccessWrDqsDly:
      TrainDly = ChannelPtr->WrDqsDlysMemPs1[Dimm * TechPtr->DlyTableWidth () + Byte];
      break;
    case AccessWrDatDly:
      TrainDly = ChannelPtr->WrDatDlysMemPs1[Dimm * TechPtr->DlyTableWidth () + Byte];
      break;
    case AccessRdDqsDly:
      TrainDly = ChannelPtr->RdDqsDlysMemPs1[Dimm * TechPtr->DlyTableWidth () + Byte];
      break;
    default:
      TrainDly = 0;
      IDS_ERROR_TRAP;
    }
  } else {
    switch (TrnDlyType) {
    case AccessRcvEnDly:
      TrainDly = ChannelPtr->RcvEnDlys[Dimm * TechPtr->DlyTableWidth () + Byte];
      break;
    case AccessWrDqsDly:
      TrainDly = ChannelPtr->WrDqsDlys[Dimm * TechPtr->DlyTableWidth () + Byte];
      break;
    case AccessWrDatDly:
      TrainDly = ChannelPtr->WrDatDlys[Dimm * TechPtr->DlyTableWidth () + Byte];
      break;
    case AccessRdDqsDly:
      TrainDly = ChannelPtr->RdDqsDlys[Dimm * TechPtr->DlyTableWidth () + Byte];
      break;
    default:
      TrainDly = 0;
      IDS_ERROR_TRAP;
    }
  }

  return TrainDly;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function sets the fixed MTRRs for common legacy ranges.
 *   It sets TOP_MEM and TOM2 and some variable MTRRs with WB Uncacheable type.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE - An Error value lower than AGESA_FATAL may have occurred
 *     @return          FALSE - An Error value greater than or equal to AGESA_FATAL may have occurred
 */

BOOLEAN
MemNCPUMemTypingNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT32 Bottom32bIO;
  UINT32 Bottom40bIO;
  UINT32 Cache32bTOP;
  S_UINT64 SMsr;

  MEM_DATA_STRUCT *MemPtr;
  MEM_PARAMETER_STRUCT *RefPtr;
  RefPtr = NBPtr->RefPtr;
  MemPtr = NBPtr->MemPtr;

  //
  //======================================================================
  // Set temporary top of memory from Node structure data.
  // Adjust temp top of memory down to accommodate 32-bit IO space.
  //======================================================================
  //Bottom40bIO=top of memory, right justified 16 bits  (defines dram versus IO space type)
  //Bottom32bIO=sub 4GB top of memory, right justified 16 bits (defines dram versus IO space type)
  //Cache32bTOP=sub 4GB top of WB cacheable memory, right justified 16 bits
  //
  if (RefPtr->HoleBase != 0) {
    Bottom32bIO = RefPtr->HoleBase;
  } else if (RefPtr->BottomIo != 0) {
    Bottom32bIO = (UINT32)RefPtr->BottomIo << (24 - 16);
  } else {
    Bottom32bIO = (UINT32)1 << (24 - 16);
  }

  Cache32bTOP = RefPtr->SysLimit + 1;
  if (Cache32bTOP < _4GB_RJ16) {
    Bottom40bIO = 0;
    if (Bottom32bIO >= Cache32bTOP) {
      Bottom32bIO = Cache32bTOP;
    }
  } else {
    Bottom40bIO = Cache32bTOP;
  }

  Cache32bTOP = Bottom32bIO;


  //
  //======================================================================
  // Set default values for CPU registers
  //======================================================================
  //
  LibAmdMsrRead (SYS_CFG, (UINT64 *)&SMsr, &MemPtr->StdHeader);
  SMsr.lo |= 0x1C0000;                // turn on modification enable bit and
                                      // mtrr enable bits
  LibAmdMsrWrite (SYS_CFG, (UINT64 *)&SMsr, &MemPtr->StdHeader);

  SMsr.lo = SMsr.hi = 0x1E1E1E1E;
  LibAmdMsrWrite (0x250, (UINT64 *)&SMsr, &MemPtr->StdHeader);      // 0 - 512K = WB Mem
  LibAmdMsrWrite (0x258, (UINT64 *)&SMsr, &MemPtr->StdHeader);      // 512K - 640K = WB Mem

  //
  //======================================================================
  // Set variable MTRR values
  //======================================================================
  //
  MemNSetMTRRrangeNb (NBPtr, 0, &Cache32bTOP, 0x200, 6);

  RefPtr->Sub4GCacheTop = Cache32bTOP << 16;

  //
  //======================================================================
  // Set TOP_MEM and TOM2 CPU registers
  //======================================================================
  //
  SMsr.hi = Bottom32bIO >> (32 - 16);
  SMsr.lo = Bottom32bIO << 16;
  LibAmdMsrWrite (TOP_MEM, (UINT64 *)&SMsr, &MemPtr->StdHeader);
  IDS_HDT_CONSOLE (MEM_FLOW, "TOP_MEM:  %08x0000\n", Bottom32bIO);

  if (Bottom40bIO) {
    SMsr.hi = Bottom40bIO >> (32 - 16);
    SMsr.lo = Bottom40bIO << 16;
  } else {
    SMsr.hi = 0;
    SMsr.lo = 0;
  }
  LibAmdMsrWrite (TOP_MEM2, (UINT64 *)&SMsr, &MemPtr->StdHeader);

  LibAmdMsrRead (SYS_CFG, (UINT64 *)&SMsr, &MemPtr->StdHeader);
  if (Bottom40bIO) {
    IDS_HDT_CONSOLE (MEM_FLOW, "TOP_MEM2: %08x0000\n", Bottom40bIO);
    IDS_HDT_CONSOLE (MEM_FLOW, "Sub1THoleBase: %08x0000\n", RefPtr->Sub1THoleBase);
    // Enable TOM2
    SMsr.lo |= 0x00600000;
  } else {
    // Disable TOM2
    SMsr.lo &= ~0x00600000;
  }
  SMsr.lo &= 0xFFF7FFFF; // turn off modification enable bit
  LibAmdMsrWrite (SYS_CFG, (UINT64 *)&SMsr, &MemPtr->StdHeader);

  return (BOOLEAN) (NBPtr->MCTPtr->ErrCode < AGESA_FATAL);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function runs on the BSP only, it sets the fixed MTRRs for common legacy ranges.
 *   It sets TOP_MEM and TOM2 and some variable MTRRs with WB Uncacheable type.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNUMAMemTypingNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT32 Bottom32bIO;
  UINT32 Bottom32bUMA;
  UINT32 Cache32bTOP;
  UINT32 Value32;
  UINT8 BitCount;
  UINT8 i;

  MEM_PARAMETER_STRUCT *RefPtr;
  RefPtr = NBPtr->RefPtr;
  BitCount = 0;
  //
  //======================================================================
  // Adjust temp top of memory down to accommodate UMA memory start
  //======================================================================
  // Bottom32bIO=sub 4GB top of memory, right justified 16 bits (defines dram versus IO space type)
  // Cache32bTOP=sub 4GB top of WB cacheable memory, right justified 16 bits
  //
  Bottom32bIO = RefPtr->Sub4GCacheTop >> 16;
  Bottom32bUMA = RefPtr->UmaBase;

  if (Bottom32bUMA < Bottom32bIO) {
    Cache32bTOP = Bottom32bUMA;
    RefPtr->Sub4GCacheTop = Bottom32bUMA << 16;
    //
    //======================================================================
    //Set variable MTRR values
    //======================================================================
    //
    Value32 = Cache32bTOP;
    //Pre-check the bit count of bottom Uma to see if it is potentially running out of Mtrr while typing.
    while (Value32 != 0) {
      i = LibAmdBitScanForward (Value32);
      Value32 &= ~ (1 << i);
      BitCount++;
    }

    if (BitCount > 5) {
      NBPtr->RefPtr->GStatus[GsbMTRRshort] = TRUE;
      MemNSetMTRRUmaRegionUCNb (NBPtr, &Cache32bTOP, &Bottom32bIO);
    } else {
      MemNSetMTRRrangeNb (NBPtr, 0, &Cache32bTOP, 0x200, 6);
    }
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *  Program MTRRs to describe given range as given cache type.  Use MTRR pairs
 *  starting with the given MTRRphys Base address, and use as many as is
 *  required up to (excluding) MSR 020C, which is reserved for OS.
 *
 *  "Limit" in the context of this procedure is not the numerically correct
 *  limit, but rather the Last address+1, for purposes of coding efficiency
 *  and readability.  Size of a region is then Limit-Base.
 *
 *  1. Size of each range must be a power of two
 *  2. Each range must be naturally aligned (Base is same as size)
 *
 *  There are two code paths: the ascending path and descending path (analogous
 *  to bsf and bsr), where the next limit is a function of the next set bit in
 *  a forward or backward sequence of bits (as a function of the Limit).  We
 *  start with the ascending path, to ensure that regions are naturally aligned,
 *  then we switch to the descending path to maximize MTRR usage efficiency.
 *  Base=0 is a special case where we start with the descending path.
 *  Correct Mask for region is 2comp(Size-1)-1,
 *  which is 2comp(Limit-Base-1)-1 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]     Base - Base address[47:16] of specified range.
 *     @param[in]     *LimitPtr - Limit address[47:16] of specified range.
 *     @param[in]     MtrrAddr - address of var MTRR pair to start using.
 *     @param[in]     MtrrType - Cache type for the range.
 *
 *     @return      TRUE - No failure occurred
 *     @return      FALSE - Failure occurred because run out of variable-size MTRRs before completion.
 */

BOOLEAN
STATIC
MemNSetMTRRrangeNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 Base,
  IN OUT   UINT32 *LimitPtr,
  IN       UINT32 MtrrAddr,
  IN       UINT8 MtrrType
  )
{
  S_UINT64 SMsr;
  UINT32 CurBase;
  UINT32 CurLimit;
  UINT32 CurSize;
  UINT32 CurAddr;
  UINT32 Value32;

  CurBase = Base;
  CurLimit = *LimitPtr;
  CurAddr = MtrrAddr;

  while ((CurAddr >= 0x200) && (CurAddr < 0x20A) && (CurBase < *LimitPtr)) {
    CurSize = CurLimit = (UINT32)1 << LibAmdBitScanForward (CurBase);
    CurLimit += CurBase;
    if ((CurBase == 0) || (*LimitPtr < CurLimit)) {
      CurLimit = *LimitPtr - CurBase;
      CurSize = CurLimit = (UINT32)1 << LibAmdBitScanReverse (CurLimit);
      CurLimit += CurBase;
    }

    // prog. MTRR with current region Base
    SMsr.lo = (CurBase << 16) | (UINT32)MtrrType;
    SMsr.hi = CurBase >> (32 - 16);
    LibAmdMsrWrite (CurAddr, (UINT64 *)&SMsr, &NBPtr->MemPtr->StdHeader);

    // prog. MTRR with current region Mask
    CurAddr++;                              // other half of MSR pair
    Value32 = CurSize - (UINT32)1;
    Value32 = ~Value32;
    SMsr.hi = (Value32 >> (32 - 16)) & NBPtr->VarMtrrHiMsk;
    SMsr.lo = (Value32 << 16) | ((UINT32)1 << MTRR_VALID);
    LibAmdMsrWrite (CurAddr, (UINT64 *)&SMsr, &NBPtr->MemPtr->StdHeader);

    CurBase = CurLimit;
    CurAddr++;                              // next MSR pair
  }

  if (CurLimit < *LimitPtr) {
    // Announce failure
    *LimitPtr = CurLimit;
    IDS_ERROR_TRAP;
  }

  while ((CurAddr >= 0x200) && (CurAddr < 0x20C)) {
    SMsr.lo = SMsr.hi = 0;
    LibAmdMsrWrite (CurAddr, (UINT64 *)&SMsr, &NBPtr->MemPtr->StdHeader);
    CurAddr++;
  }

  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *     Program one MTRR to describe Uma region as UC cache type if we detect running out of
 *     Mtrr circumstance.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]     *BasePtr - Base address[47:24] of specified range.
 *     @param[in]     *LimitPtr - Limit address[47:24] of specified range.
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */
BOOLEAN
MemNSetMTRRUmaRegionUCNb  (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 *BasePtr,
  IN OUT   UINT32 *LimitPtr
  )
{
  S_UINT64 SMsr;
  UINT32 Mtrr;
  UINT32 Size;
  UINT32 Value32;

  Size = *LimitPtr - *BasePtr;
  // Check if Size is a power of 2
  if ((Size & (Size - 1)) != 0) {
    for (Mtrr = 0x200; Mtrr < 0x20A; Mtrr += 2) {
      LibAmdMsrRead (Mtrr + 1, (UINT64 *)&SMsr, &NBPtr->MemPtr->StdHeader);
      if ((SMsr.lo & ((UINT32) 1 << 11)) == 0) {
        MemNSetMTRRrangeNb (NBPtr, *BasePtr, LimitPtr, Mtrr, 0);
        break;
      }
    }
    if (Mtrr == 0x20A) {
      // Run out of MTRRs
      IDS_ERROR_TRAP;
    }
  } else {
    Mtrr = 0x20A; //Reserved pair of MTRR for UMA region.

    // prog. MTRR with current region Base
    SMsr.lo = *BasePtr  << 16;
    SMsr.hi = *BasePtr >> (32 - 16);
    LibAmdMsrWrite (Mtrr, (UINT64 *)&SMsr, &NBPtr->MemPtr->StdHeader);

    // prog. MTRR with current region Mask
    Mtrr++;                              // other half of MSR pair
    Value32 = Size - (UINT32)1;
    Value32 = ~Value32;
    SMsr.hi = (Value32 >> (32 - 16)) & NBPtr->VarMtrrHiMsk;
    SMsr.lo = (Value32 << 16) | ((UINT32)1 << MTRR_VALID);
    LibAmdMsrWrite (Mtrr, (UINT64 *)&SMsr, &NBPtr->MemPtr->StdHeader);
  }

  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *     Report the Uma size that is going to be allocated.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          Uma size [31:0] = Addr [47:16]
 */
UINT32
MemNGetUmaSizeNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return 0;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function allocates 16MB of memory for C6 storage when it is requested to be enabled
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */
VOID
MemNAllocateC6StorageClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT32 SysLimit;

  if (IsFeatureEnabled (C6Cstate, NBPtr->MemPtr->PlatFormConfig, &(NBPtr->MemPtr->StdHeader))) {
    SysLimit = NBPtr->RefPtr->SysLimit;
    SysLimit -= _16MB_RJ16;

    // Set Dram Limit
    NBPtr->MCTPtr->NodeSysLimit = SysLimit;
    NBPtr->RefPtr->SysLimit = SysLimit;
    MemNSetBitFieldNb (NBPtr, BFDramLimitReg0, ((SysLimit << 8) & 0xFFFF0000));

    // Set TOPMEM and MTRRs
    MemNC6AdjustMSRs (NBPtr);

    // Set C6Base
    MemNSetBitFieldNb (NBPtr, BFC6Base, (SysLimit + 1) >> (24 - 16));

    // C6DramLock will be set in FinalizeMCT
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function allocates 16MB of memory for C6 storage when it is requested to be enabled
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */
VOID
MemNAllocateC6StorageUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8  Node;
  UINT32 SysLimit;
  UINT32 DramLimitReg;

  if (NBPtr->SharedPtr->C6Enabled || IsFeatureEnabled (C6Cstate, NBPtr->MemPtr->PlatFormConfig, &(NBPtr->MemPtr->StdHeader))) {

    SysLimit = NBPtr->RefPtr->SysLimit;

    // Calculate new SysLimit
    if (!NBPtr->SharedPtr->C6Enabled) {
      if (NBPtr->SharedPtr->NodeIntlv.NodeCnt >= 2) {
        // Node Interleave is enabled, system memory available is reduced by 16MB * number of nodes
        SysLimit -= _16MB_RJ16 * NBPtr->SharedPtr->NodeIntlv.NodeCnt;
      } else {
        // Otherwise, system memory available is reduced by 16MB
        SysLimit -= _16MB_RJ16;
      }
      NBPtr->RefPtr->SysLimit = SysLimit;
      NBPtr->SharedPtr->C6Enabled = TRUE;

      // Set TOPMEM and MTRRs (only need to be done once for BSC)
      MemNC6AdjustMSRs (NBPtr);
    }

    // Set Dram Limit
    if (NBPtr->SharedPtr->NodeIntlv.NodeCnt >= 2) {
      for (Node = 0; Node < NBPtr->NodeCount; Node++) {
        DramLimitReg = MemNGetBitFieldNb (NBPtr, BFDramLimitReg0 + Node);
        if ((DramLimitReg & 0xFFFF0000) != 0) {
          MemNSetBitFieldNb (NBPtr, BFDramLimitReg0 + Node, ((SysLimit << 8) & 0xFFFF0000) | (DramLimitReg & 0xFFFF));
          MemNSetBitFieldNb (NBPtr, BFDramLimitHiReg0 + Node, SysLimit >> 24);
        }
      }
      // Node Interleave is enabled, CoreStateSaveDestNode points to its own node
      MemNSetBitFieldNb (NBPtr, BFCoreStateSaveDestNode, NBPtr->Node);
      NBPtr->MCTPtr->NodeSysLimit = SysLimit;
    } else {
      DramLimitReg = MemNGetBitFieldNb (NBPtr, BFDramLimitReg0 + NBPtr->SharedPtr->TopNode) & 0x0000FFFF;
      MemNSetBitFieldNb (NBPtr, BFDramLimitReg0 + NBPtr->SharedPtr->TopNode, ((SysLimit << 8) & 0xFFFF0000) | DramLimitReg);
      MemNSetBitFieldNb (NBPtr, BFDramLimitHiReg0 + NBPtr->SharedPtr->TopNode, SysLimit >> 24);

      // Node Interleave is not enabled, CoreStateSaveDestNode points to the node that contains top memory
      MemNSetBitFieldNb (NBPtr, BFCoreStateSaveDestNode, NBPtr->SharedPtr->TopNode);

      if (NBPtr->Node == NBPtr->SharedPtr->TopNode) {
        NBPtr->MCTPtr->NodeSysLimit = SysLimit;
      }
    }

    // Set BFCC6SaveEn
    MemNSetBitFieldNb (NBPtr, BFCC6SaveEn, 1);

    // LockDramCfg will be set in FinalizeMCT
  }
}


/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function readjusts TOPMEM and MTRRs after allocating storage for C6
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */
VOID
MemNC6AdjustMSRs (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT32 SysLimit;
  UINT32 CurAddr;
  S_UINT64 SMsr;

  SysLimit = NBPtr->RefPtr->SysLimit + 1;
  SMsr.hi = SysLimit >> (32 - 16);
  SMsr.lo = SysLimit << 16;
  if (SysLimit < _4GB_RJ16) {
    LibAmdMsrWrite (TOP_MEM, (UINT64 *)&SMsr, &(NBPtr->MemPtr->StdHeader));
    IDS_HDT_CONSOLE (MEM_FLOW, "TOP_MEM:  %08x0000\n", SysLimit);
    // If there is no UMA buffer, then set top of cache and MTRR.
    // Otherwise, top of cache and MTRR will be set when UMA buffer is set up.
    if (NBPtr->RefPtr->UmaMode == UMA_NONE) {
      NBPtr->RefPtr->Sub4GCacheTop = (SysLimit << 16);
      // Find unused MTRR to set C6 region to UC
      for (CurAddr = 0x200; CurAddr < 0x20C; CurAddr += 2) {
        LibAmdMsrRead (CurAddr + 1, (UINT64 *)&SMsr, &NBPtr->MemPtr->StdHeader);
        if ((SMsr.lo & ((UINT32) 1 << 11)) == 0) {
          // Set region base as TOM
          SMsr.hi = SysLimit >> (32 - 16);
          SMsr.lo = SysLimit << 16;
          LibAmdMsrWrite (CurAddr, (UINT64 *)&SMsr, &NBPtr->MemPtr->StdHeader);

          // set region mask to 16MB
          SMsr.hi = NBPtr->VarMtrrHiMsk;
          SMsr.lo = 0xFF000800;
          LibAmdMsrWrite (CurAddr + 1, (UINT64 *)&SMsr, &NBPtr->MemPtr->StdHeader);

          break;
        }
      }
    }
  } else {
    LibAmdMsrWrite (TOP_MEM2, (UINT64 *)&SMsr, &(NBPtr->MemPtr->StdHeader));
    IDS_HDT_CONSOLE (MEM_FLOW, "TOP_MEM2: %08x0000\n", SysLimit);
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     Family-specific hook to override the DdrMaxRate value for families with a
 *     non-GH-compatible encoding for BFDdrMaxRate
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *DdrMaxRate - Void pointer to DdrMaxRate.  Used as INT16.
 *
 *     @return          TRUE
 *
 */
BOOLEAN
MemNGetMaxDdrRateUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       VOID *DdrMaxRate
  )
{
  UINT8 DdrMaxRateEncoded;

  DdrMaxRateEncoded = (UINT8) MemNGetBitFieldNb (NBPtr, BFDdrMaxRate);

  if (DdrMaxRateEncoded == 0) {
    * (UINT16 *) DdrMaxRate = UNSUPPORTED_DDR_FREQUENCY;
  } else {
    * (UINT16 *) DdrMaxRate = MemNGetMemClkFreqUnb (NBPtr, DdrMaxRateEncoded);
  }
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function performs the action after save/restore execution
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]  OptParam   - Optional parameter
 *
 *     @return    TRUE
 *
 */

BOOLEAN
MemNAfterSaveRestoreUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  // Sync. up DctCfgSel value with NBPtr->Dct
  MemNSetBitFieldNb (NBPtr, BFDctCfgSel, NBPtr->Dct);

  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function performs the action before and after excluding dimms on CNB
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]  *IsBefore   - If the function is called before excluding dimms
 *
 *     @return    TRUE
 *
 */

BOOLEAN
MemNBfAfExcludeDimmClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *IsBefore
  )
{
  if (*(BOOLEAN *) IsBefore == TRUE) {
    NBPtr->BrdcstSet (NBPtr, BFEnterSelfRef, 1);
    NBPtr->PollBitField (NBPtr, BFEnterSelfRef, 0, PCI_ACCESS_TIMEOUT, TRUE);
  } else {
    NBPtr->BrdcstSet (NBPtr, BFExitSelfRef, 1);
    NBPtr->PollBitField (NBPtr, BFExitSelfRef, 0, PCI_ACCESS_TIMEOUT, TRUE);
  }

  return TRUE;
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
