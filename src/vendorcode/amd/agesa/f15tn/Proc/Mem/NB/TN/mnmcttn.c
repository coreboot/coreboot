/* $NoKeywords:$ */
/**
 * @file
 *
 * mnmcttn.c
 *
 * Northbridge TN MCT supporting functions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/TN)
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
#include "GnbRegistersTN.h"
#include "GnbRegisterAccTN.h"
#include "mm.h"
#include "mn.h"
#include "OptionMemory.h"
#include "mntn.h"
#include "cpuFeatures.h"
#include "Filecode.h"
#include "mftds.h"
#include "mu.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)


#define FILECODE PROC_MEM_NB_TN_MNMCTTN_FILECODE
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


/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
extern BUILD_OPT_CFG UserOptions;

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function force memory Pstate to M0
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */

BOOLEAN
MemNInitializeMctTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  MemNSetBitFieldNb (NBPtr, BFMemPsSel, 0);
  MemNSetBitFieldNb (NBPtr, BFEnSplitMctDatBuffers, 1);

  MemUMFenceInstr ();
  MemNSetBitFieldNb (NBPtr, BFMctEccDisLatOptEn, 1);
  MemUMFenceInstr ();

  MemNBrdcstSetUnConditionalNb (NBPtr, BFPStateToAccess, 0);

  MemNForcePhyToM0Unb (NBPtr);

  return (BOOLEAN) (NBPtr->MCTPtr->ErrCode < AGESA_FATAL);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function sets final values for specific registers.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */

BOOLEAN
MemNFinalizeMctTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  MEM_DATA_STRUCT *MemPtr;
  MEM_PARAMETER_STRUCT *RefPtr;
  DRAM_PREFETCH_MODE DramPrefetchMode;
  UINT16 Speed;
  UINT32 Value32;
  UINT8 DcqBwThrotWm1;
  UINT8 DcqBwThrotWm2;
  UINT8 Dct;

  MemPtr = NBPtr->MemPtr;
  RefPtr = MemPtr->ParameterListPtr;
  DramPrefetchMode = MemPtr->PlatFormConfig->PlatformProfile.AdvancedPerformanceProfile.DramPrefetchMode;
  Speed = NBPtr->DCTPtr->Timings.Speed;

  //
  // F2x11C
  //
  MemNSetBitFieldNb (NBPtr, BFMctCfgHiReg, 0x0CE00F31);
  if (DramPrefetchMode == DISABLE_DRAM_PREFETCH_FOR_IO || DramPrefetchMode == DISABLE_DRAM_PREFETCHER) {
    MemNSetBitFieldNb (NBPtr, BFPrefIoDis, 1);
  }

  if (DramPrefetchMode == DISABLE_DRAM_PREFETCH_FOR_CPU || DramPrefetchMode == DISABLE_DRAM_PREFETCHER) {
    MemNSetBitFieldNb (NBPtr, BFPrefCpuDis, 1);
  }


  if (Speed == DDR667_FREQUENCY) {
    DcqBwThrotWm1 = 3;
    DcqBwThrotWm2 = 4;
  } else if (Speed == DDR800_FREQUENCY) {
    DcqBwThrotWm1 = 3;
    DcqBwThrotWm2 = 5;
  } else if (Speed == DDR1066_FREQUENCY) {
    DcqBwThrotWm1 = 4;
    DcqBwThrotWm2 = 6;
  } else if (Speed == DDR1333_FREQUENCY) {
    DcqBwThrotWm1 = 5;
    DcqBwThrotWm2 = 8;
  } else if  (Speed == DDR1600_FREQUENCY) {
    DcqBwThrotWm1 = 6;
    DcqBwThrotWm2 = 9;
  } else if  (Speed == DDR1866_FREQUENCY) {
    DcqBwThrotWm1 = 7;
    DcqBwThrotWm2 = 10;
  } else {
    DcqBwThrotWm1 = 8;
    DcqBwThrotWm2 = 12;
  }
  //
  // F2x1B0
  //
  Value32 = MemNGetBitFieldNb (NBPtr, BFExtMctCfgLoReg);
  Value32 &= 0x003FE8C0;
  Value32 |= 0x0FC01001;
  MemNSetBitFieldNb (NBPtr, BFExtMctCfgLoReg, Value32);

  //
  // F2x1B4
  //
  Value32 = MemNGetBitFieldNb (NBPtr, BFExtMctCfgHiReg);
  Value32 &= 0xFFFFFC00;
  Value32 |= (((UINT32) DcqBwThrotWm2 << 5) | (UINT32) DcqBwThrotWm1);
  MemNSetBitFieldNb (NBPtr, BFExtMctCfgHiReg, Value32);

  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_TN; Dct++) {
    MemNSwitchDCTNb (NBPtr, Dct);

    if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
      //
      // Phy Power Saving
      //
      MemNPhyPowerSavingMPstateUnb (NBPtr);
      if (NBPtr->MemPstateStage == MEMORY_PSTATE_3RD_STAGE) {
        MemNChangeMemPStateContextNb (NBPtr, 1);
        MemNPhyPowerSavingMPstateUnb (NBPtr);
        MemFInitTableDrive (NBPtr, MTAfterSettingMemoryPstate1);
        MemNChangeMemPStateContextNb (NBPtr, 0);
      }
      //
      // Power Down Enable
      //
      if (NBPtr->RefPtr->EnablePowerDown) {
        MemNSetBitFieldNb (NBPtr, BFPowerDownEn, 1);
      }
    }
  }

  // Set LockDramCfg
  if (IsFeatureEnabled (C6Cstate, NBPtr->MemPtr->PlatFormConfig, &(NBPtr->MemPtr->StdHeader))) {
    IDS_SKIP_HOOK (IDS_LOCK_DRAM_CFG, NBPtr, &NBPtr->MemPtr->StdHeader) {
      MemNSetBitFieldNb (NBPtr, BFLockDramCfg, 1);
    }
  }

  return (BOOLEAN) (NBPtr->MCTPtr->ErrCode < AGESA_FATAL);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function create the HT memory map for TN
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */

BOOLEAN
MemNHtMemMapInitTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 WeReMask;
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
  BottomIo = (RefPtr->BottomIo & 0xF8) << 8;

  if (MCTPtr->NodeMemSize != 0) {
    NodeSysBase = 0;
    NodeSysLimit = MCTPtr->NodeMemSize - 1;
    DctSelBaseAddr = MCTPtr->DctData[0].Timings.DctMemSize;

    if (NodeSysLimit >= BottomIo) {
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
      MemNSetBitFieldNb (NBPtr, BFDramMemHoistValid, 1);
    } else {
      // No Remapping.  Normal Contiguous mapping
    }
    MCTPtr->NodeSysBase = NodeSysBase;
    MCTPtr->NodeSysLimit = NodeSysLimit;
    RefPtr->SysLimit = MCTPtr->NodeSysLimit;

    WeReMask = 3;
    // Set the Dram base and set the WE and RE flags in the base.
    MemNSetBitFieldNb (NBPtr, BFDramBaseReg0, (NodeSysBase << 8) | WeReMask);
    MemNSetBitFieldNb (NBPtr, BFDramBaseHiReg0, NodeSysBase >> 24);
    // Set the Dram limit and set DstNode.
    MemNSetBitFieldNb (NBPtr, BFDramLimitReg0, ((NodeSysLimit << 8) & 0xFFFF0000));
    MemNSetBitFieldNb (NBPtr, BFDramLimitHiReg0, NodeSysLimit >> 24);

    MemNSetBitFieldNb (NBPtr, BFDramBaseAddr, NodeSysBase >> (27 - 16));
    MemNSetBitFieldNb (NBPtr, BFDramLimitAddr, NodeSysLimit >> (27 - 16));

    if ((MCTPtr->DctData[1].Timings.DctMemSize != 0) && (!NBPtr->Ganged)) {
      MemNSetBitFieldNb (NBPtr, BFDctSelBaseAddr, DctSelBaseAddr >> 11);
      MemNSetBitFieldNb (NBPtr, BFDctSelHiRngEn, 1);
      MemNSetBitFieldNb (NBPtr, BFDctSelHi, 1);
      MemNSetBitFieldNb (NBPtr, BFDctSelBaseOffset, DctSelBaseAddr >> 10);
    }
  }
  return (BOOLEAN) (NBPtr->MCTPtr->ErrCode < AGESA_FATAL);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *     Report the Uma size that is going to be allocated.
 *     Total system memory   UMASize
 *     >= 2G                 512M
 *     >=1G                  256M
 *     <1G                    64M
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          Uma size [31:0] = Addr [47:16]
 */
UINT32
MemNGetUmaSizeTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT32 SysMemSize;
  UINT32 SizeOfUma;

  SysMemSize = NBPtr->RefPtr->SysLimit + 1;
  SysMemSize = (SysMemSize + 0x100) & 0xFFFFF000;   // Ignore 16MB allocated for C6 when finding UMA size
  if (SysMemSize >= 0x8000) {
    SizeOfUma = 512 << (20 - 16);
  } else if (SysMemSize >= 0x4000) {
    SizeOfUma = 256 << (20 - 16);
  } else {
    SizeOfUma = 64 << (20 - 16);
  }

  return SizeOfUma;
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
MemNAllocateC6StorageTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT32 SysLimit;
  UINT32 DramLimitReg;

  if (NBPtr->SharedPtr->C6Enabled || IsFeatureEnabled (C6Cstate, NBPtr->MemPtr->PlatFormConfig, &(NBPtr->MemPtr->StdHeader))) {

    SysLimit = NBPtr->RefPtr->SysLimit;

    // Calculate new SysLimit
    if (!NBPtr->SharedPtr->C6Enabled) {
      // System memory available is reduced by 16MB
      SysLimit -= _16MB_RJ16;

      NBPtr->MCTPtr->NodeSysLimit = SysLimit;
      NBPtr->RefPtr->SysLimit = SysLimit;
      NBPtr->SharedPtr->C6Enabled = TRUE;

      // Set TOPMEM and MTRRs (only need to be done once for BSC)
      MemNC6AdjustMSRs (NBPtr);
    }

    // Set Dram Limit
    DramLimitReg = MemNGetBitFieldNb (NBPtr, BFDramLimitReg0) & 0x0000FFFF;
    MemNSetBitFieldNb (NBPtr, BFDramLimitReg0, ((SysLimit << 8) & 0xFFFF0000) | DramLimitReg);
    MemNSetBitFieldNb (NBPtr, BFDramLimitHiReg0, SysLimit >> 24);

    MemNSetBitFieldNb (NBPtr, BFCoreStateSaveDestNode, 0);

    // Set BFCC6SaveEn
    MemNSetBitFieldNb (NBPtr, BFCC6SaveEn, 1);
    // LockDramCfg will be set in FinalizeMCT
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function adjusts NB pstate norbridge voltage for TN
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */
VOID
MemNAdjustNBPstateVolTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{

  D0F0xBC_xE0104168_STRUCT D0F0xBC_xE0104168;
  D0F0xBC_xE010416C_STRUCT D0F0xBC_xE010416C;
  D0F0xBC_xE0104170_STRUCT D0F0xBC_xE0104170;
  UINT8 MemClkVidHi;
  UINT8 MemClkVidLo;
  UINT8 MemPstate;
  UINT8 NbVid;
  UINT8 NbPs;
  UINT8 NbPstateMaxVal;

  IDS_HDT_CONSOLE (MEM_FLOW, "\nStart NB Pstate voltage adjustment.\n");

  GnbRegisterReadTN (TYPE_D0F0xBC, D0F0xBC_xE0104168_ADDRESS, &D0F0xBC_xE0104168.Value, 0, &(NBPtr->MemPtr->StdHeader));
  GnbRegisterReadTN (TYPE_D0F0xBC, D0F0xBC_xE010416C_ADDRESS, &D0F0xBC_xE010416C.Value, 0, &(NBPtr->MemPtr->StdHeader));
  GnbRegisterReadTN (TYPE_D0F0xBC, D0F0xBC_xE0104170_ADDRESS, &D0F0xBC_xE0104170.Value, 0, &(NBPtr->MemPtr->StdHeader));

  IDS_HDT_CONSOLE (MEM_FLOW, "\t\tD0F0xBC_xE0104168: %08x\n", D0F0xBC_xE0104168.Value);
  IDS_HDT_CONSOLE (MEM_FLOW, "\t\tD0F0xBC_xE010416C: %08x\n", D0F0xBC_xE010416C.Value);
  IDS_HDT_CONSOLE (MEM_FLOW, "\t\tD0F0xBC_xE0104170: %08x\n", D0F0xBC_xE0104170.Value);

  // MemClkVidHi = read D0F0xBC_xE0104168 through D0F0xBC_xE0104170 to find the VID code corresponding
  // to the M0 MEMCLK. If the M0 MEMCLK is not found, use the next higher defined MEMCLK as the target.
  switch (NBPtr->DCTPtr->Timings.TargetSpeed) {
  case DDR667_FREQUENCY:
    MemClkVidHi = (UINT8) D0F0xBC_xE0104168.Field.MemClkVid0_7_0;
    break;
  case DDR800_FREQUENCY:
    MemClkVidHi = (UINT8) D0F0xBC_xE0104168.Field.MemClkVid1_7_0;
    break;
  case DDR1066_FREQUENCY:
    MemClkVidHi = (UINT8) D0F0xBC_xE0104168.Field.MemClkVid2_7_0;
    break;
  case DDR1333_FREQUENCY:
    MemClkVidHi = (UINT8) D0F0xBC_xE0104168.Field.MemClkVid3_1_0 | ((UINT8) D0F0xBC_xE010416C.Field.MemClkVid3_7_2 << 2);
    break;
  case DDR1600_FREQUENCY:
    MemClkVidHi = (UINT8) D0F0xBC_xE010416C.Field.MemClkVid4_7_0;
    break;
  case DDR1866_FREQUENCY:
    MemClkVidHi = (UINT8) D0F0xBC_xE010416C.Field.MemClkVid5_7_0;
    break;
  case DDR2100_FREQUENCY:
    MemClkVidHi = (UINT8) D0F0xBC_xE010416C.Field.MemClkVid6_7_0;
    break;
  case DDR2133_FREQUENCY:
    MemClkVidHi = (UINT8) D0F0xBC_xE010416C.Field.MemClkVid7_1_0 | ((UINT8) D0F0xBC_xE0104170.Field.MemClkVid7_7_2 << 2);
    break;
  case DDR2400_FREQUENCY:
    MemClkVidHi = (UINT8) D0F0xBC_xE0104170.Field.MemClkVid8_7_0;
    break;
  default:
    // If the M0 MEMCLK is greater than MemClkVid8, use the MemClkVid8 VID as the target
    MemClkVidHi = (UINT8) D0F0xBC_xE0104170.Field.MemClkVid8_7_0;
  }

  // MemClkVidLo = read D0F0xBC_xE0104168 through D0F0xBC_xE0104170 to find the VID code corresponding
  // to the M1 MEMCLK. If the M1 MEMCLK is not found, use the next higher defined MEMCLK as the target.
  MemClkVidLo = (UINT8) D0F0xBC_xE0104168.Field.MemClkVid0_7_0;

  IDS_HDT_CONSOLE (MEM_FLOW, "\t\tOriginal MemClkVidLo: %02x\n", MemClkVidLo);
  IDS_HDT_CONSOLE (MEM_FLOW, "\t\tOriginal MemClkVidHi: %02x\n", MemClkVidHi);

  // If D18F5x188[NbOffsetTrim] == 01b, MemClkVid = Fuse[MemClkVid] - 4 (-25mV offset so add 25mV to VID)
  // Else if D18F5x188[NbOffsetTrim] == 11b, MemClkVid = Fuse[MemClkVid] + 4 (+25mV offset so decrease 25mV from VID)
  // Else MemClkVid = Fuse[MemClkVid]
  if (MemNGetBitFieldNb (NBPtr, BFNbOffsetTrim) == 1) {
    MemClkVidLo -= 4;
    MemClkVidHi -= 4;
    IDS_HDT_CONSOLE (MEM_FLOW, "\t\tAdd 25mV\n");
  } else if (MemNGetBitFieldNb (NBPtr, BFNbOffsetTrim) == 3) {
    MemClkVidLo += 4;
    MemClkVidHi += 4;
    IDS_HDT_CONSOLE (MEM_FLOW, "\t\tDecrease 25mV\n");
  }
  IDS_HDT_CONSOLE (MEM_FLOW, "\t\tAdjusted MemClkVidLo: %02x\n", MemClkVidLo);
  IDS_HDT_CONSOLE (MEM_FLOW, "\t\tAdjusted MemClkVidHi: %02x\n", MemClkVidHi);

  // For each NB P-state from NBP0 through D18F5x170[NbPstateMaxVal]:
  // If ((D18F5x1[6C:60][MemPstate] == 0) && (MemClkVidHi voltage > D18F5x1[6C:60][NbVid] voltage)):
  // Program D18F5x1[6C:60][NbVid] == MemClkVidHi.
  // If ((D18F5x1[6C:60][MemPstate] == 1) && (MemClkVidLo voltage > D18F5x1[6C:60][NbVid] voltage)):
  // Program D18F5x1[6C:60][NbVid] == MemClkVidLo.
  IDS_HDT_CONSOLE (MEM_FLOW, "\t\tNBPs\tNbVid\tMemPstate\tOverride\n");
  NbPstateMaxVal = (UINT8) MemNGetBitFieldNb (NBPtr, BFNbPstateMaxVal);
  for (NbPs = 0; NbPs <= NbPstateMaxVal; NbPs ++) {
    NbVid = (UINT8) MemNGetBitFieldNb (NBPtr, BFNbVid0 + (NbPs << 1));
    MemPstate = (UINT8) MemNGetBitFieldNb (NBPtr, BFMemPstate0 + NbPs);
    IDS_HDT_CONSOLE (MEM_FLOW, "\t\t  %01d \t  %02x  \t    %01d     \t", NbPs, NbVid, MemPstate);
    // higher voltage correspond to smaller VID
    if ((MemPstate == 0) && (MemClkVidHi < NbVid)) {
      MemNSetBitFieldNb (NBPtr, BFNbVid0 + (NbPs << 1), MemClkVidHi);
      IDS_HDT_CONSOLE (MEM_FLOW, "MemClkVidHi\n");
    } else if ((MemPstate == 1) && (MemClkVidLo < NbVid)) {
      MemNSetBitFieldNb (NBPtr, BFNbVid0 + (NbPs << 1), MemClkVidLo);
      IDS_HDT_CONSOLE (MEM_FLOW, "MemClkVidLo\n");
    } else {
      IDS_HDT_CONSOLE (MEM_FLOW, "No change\n");
    }
  }
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
