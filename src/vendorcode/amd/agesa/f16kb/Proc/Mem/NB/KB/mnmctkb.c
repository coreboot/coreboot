/* $NoKeywords:$ */
/**
 * @file
 *
 * mnmctkb.c
 *
 * Northbridge KB MCT supporting functions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/KB)
 * @e \$Revision: 87696 $ @e \$Date: 2013-02-07 12:35:03 -0600 (Thu, 07 Feb 2013) $
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
#include "Gnb.h"
#include "GnbPcie.h"
#include "GnbHandleLib.h"
#include "GnbRegistersKB.h"
#include "GnbRegisterAccKB.h"
#include "mm.h"
#include "mn.h"
#include "OptionMemory.h"
#include "mnkb.h"
#include "cpuFeatures.h"
#include "Filecode.h"
#include "mftds.h"
#include "mu.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)


#define FILECODE PROC_MEM_NB_KB_MNMCTKB_FILECODE
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
MemNInitializeMctKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  MemNSetBitFieldNb (NBPtr, BFMemPsSel, 0);
  MemNSetBitFieldNb (NBPtr, BFEnSplitMctDatBuffers, 1);

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
MemNFinalizeMctKB (
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

  MemPtr = NBPtr->MemPtr;
  RefPtr = MemPtr->ParameterListPtr;
  DramPrefetchMode = MemPtr->PlatFormConfig->PlatformProfile.AdvancedPerformanceProfile.DramPrefetchMode;
  Speed = NBPtr->DCTPtr->Timings.Speed;

  //
  // F2x11C
  //
  MemNSetBitFieldNb (NBPtr, BFMctCfgHiReg, 0x0CE00F30);
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
 *   This function create the HT memory map for KB
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */

BOOLEAN
MemNHtMemMapInitKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 WeReMask;
  UINT8 LgcyMmioHoleEn;
  UINT32 BottomIo;
  UINT32 HoleOffset;
  UINT32 DctLimitAddr;
  UINT32 NodeSysBase;
  UINT32 NodeSysLimit;
  MEM_PARAMETER_STRUCT *RefPtr;
  DIE_STRUCT *MCTPtr;

  RefPtr = NBPtr->RefPtr;
  MCTPtr = NBPtr->MCTPtr;
  //
  // Physical addresses in this function are right adjusted by 16 bits ([47:16])
  // They are BottomIO, HoleOffset, DctLimitAddr, NodeSysBase, NodeSysLimit.
  //

  // Enforce bottom of IO be be 128MB aligned
  BottomIo = (RefPtr->BottomIo & 0xF8) << 8;

  if (MCTPtr->NodeMemSize != 0) {
    NodeSysBase = 0;
    NodeSysLimit = MCTPtr->NodeMemSize - 1;
    DctLimitAddr = MCTPtr->DctData[0].Timings.DctMemSize;
    LgcyMmioHoleEn = 0;

    if (NodeSysLimit >= BottomIo) {
      // HW Dram Remap
      MCTPtr->Status[SbHWHole] = TRUE;
      RefPtr->GStatus[GsbHWHole] = TRUE;
      MCTPtr->NodeHoleBase = BottomIo;
      RefPtr->HoleBase = BottomIo;

      HoleOffset = _4GB_RJ16 - BottomIo;

      NodeSysLimit += HoleOffset;

      if ((DctLimitAddr > 0) && (DctLimitAddr < BottomIo)) {
        HoleOffset += DctLimitAddr;
      } else {
        if (DctLimitAddr >= BottomIo) {
          DctLimitAddr += HoleOffset;
        }
        HoleOffset += NodeSysBase;
        LgcyMmioHoleEn = 1;
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
    // Set the Dram limit and set DstNode.
    MemNSetBitFieldNb (NBPtr, BFDramLimitReg0, ((NodeSysLimit << 8) & 0xFFFF0000));

    MemNSetBitFieldNb (NBPtr, BFDramBaseAddr, NodeSysBase >> (27 - 16));
    MemNSetBitFieldNb (NBPtr, BFDramLimitAddr, NodeSysLimit >> (27 - 16));

    MemNSetBitFieldNb (NBPtr, BFDramCtrlBaseReg0, 1);
    MemNSetBitFieldNb (NBPtr, BFLgcyMmioHoleEn0, LgcyMmioHoleEn);
    MemNSetBitFieldNb (NBPtr, BFDramCtrlLimitReg0, ((DctLimitAddr - 1) >> (27 - 16 - 11)) & 0xFFFFF800);
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
MemNGetUmaSizeKB (
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
MemNAllocateC6StorageKB (
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

    // Set BFCC6SaveEn and LockDramCfg
    MemNSetBitFieldNb (NBPtr, BFCC6SaveEn, 1);
    IDS_SKIP_HOOK (IDS_LOCK_DRAM_CFG, NBPtr, &NBPtr->MemPtr->StdHeader) {
      MemNSetBitFieldNb (NBPtr, BFLockDramCfg, 1);
    }
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function programs Powerdown and other power saving features for KB
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */

BOOLEAN
MemNPowerSavingKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Dct;

  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_KB; Dct++) {
    MemNSwitchDCTNb (NBPtr, Dct);

    if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
      //
      // Phy Power Saving
      //
      MemNPhyPowerSavingMPstateKB (NBPtr);
      if (NBPtr->MemPstateStage == MEMORY_PSTATE_3RD_STAGE) {
        MemNChangeMemPStateContextNb (NBPtr, 1);
        MemNPhyPowerSavingMPstateKB (NBPtr);
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

  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function force the ECC symbol size
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *Size    - ECC symbol size
 *
 *     @return    TRUE
 */

BOOLEAN
MemNForceEccSymbolSizeKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *Size
  )
{
  *(BOOLEAN *)Size = FALSE;

  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function disables scrubber temporarily for write access to
 *     DCT additional address space
 *
 *     @param[in,out]   *NBPtr            - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNDisableScrubberKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  NBPtr->DisDramScrub = NBPtr->GetBitField (NBPtr, BFDisDramScrub);
  NBPtr->SetBitField (NBPtr, BFDisDramScrub, 1);
  NBPtr->GetBitField (NBPtr, BFDisDramScrub);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function restores scrubber settings
 *
 *     @param[in,out]   *NBPtr            - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNRestoreScrubberKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  NBPtr->SetBitField (NBPtr, BFDisDramScrub, NBPtr->DisDramScrub);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function disable memory hole mappings
 *
 *     @param[in,out]   *NBPtr             - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *OptParam          - Optional parameter
 *
 *     @return    TRUE
 */

BOOLEAN
MemNDisableMemHoleMappingKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  UINT8 Range;
  CONST BIT_FIELD_NAME LgcyMmioHoleEnBF[] = {BFLgcyMmioHoleEn0};

  for (Range = 0; Range < sizeof (LgcyMmioHoleEnBF) / sizeof (BIT_FIELD_NAME); Range++) {
    NBPtr->LgcyMmioHoleEnMap &= ~(1 << Range);
    NBPtr->LgcyMmioHoleEnMap |= (NBPtr->GetBitField (NBPtr, LgcyMmioHoleEnBF[Range]) << Range);
    NBPtr->SetBitField (NBPtr, LgcyMmioHoleEnBF[Range], 0);
  }
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function restore memory hole mappings
 *
 *     @param[in,out]   *NBPtr             - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *OptParam          - Optional parameter
 *
 *     @return    TRUE
 */

BOOLEAN
MemNRestoreMemHoleMappingKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  UINT8 Range;
  CONST BIT_FIELD_NAME LgcyMmioHoleEnBF[] = {BFLgcyMmioHoleEn0};

  for (Range = 0; Range < sizeof (LgcyMmioHoleEnBF) / sizeof (BIT_FIELD_NAME); Range++) {
    NBPtr->SetBitField (NBPtr, LgcyMmioHoleEnBF[Range], (NBPtr->LgcyMmioHoleEnMap >> Range) & 1);
  }
  return TRUE;
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
