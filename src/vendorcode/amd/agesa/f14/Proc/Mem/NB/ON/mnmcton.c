/* $NoKeywords:$ */
/**
 * @file
 *
 * mnmcton.c
 *
 * Northbridge ON MCT supporting functions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/ON)
 * @e \$Revision: 41275 $ @e \$Date: 2010-11-03 02:14:41 +0800 (Wed, 03 Nov 2010) $
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
#include "OptionMemory.h"       // need def for MEM_FEAT_BLOCK_NB
#include "mnon.h"
#include "mu.h"
#include "GeneralServices.h"
#include "Filecode.h"
#define FILECODE PROC_MEM_NB_ON_MNMCTON_FILECODE
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

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
extern BUILD_OPT_CFG UserOptions;
/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function create the HT memory map
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */

BOOLEAN
MemNHtMemMapInitON (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 WeReMask;
  UINT32 BottomIo;
  UINT32 HoleOffset;
  UINT32 NodeSysBase;
  UINT32 NodeSysLimit;
  MEM_PARAMETER_STRUCT *RefPtr;
  DIE_STRUCT *MCTPtr;

  RefPtr = NBPtr->RefPtr;
  MCTPtr = NBPtr->MCTPtr;
  //
  // Physical addresses in this function are right adjusted by 16 bits ([47:16])
  // They are BottomIO, HoleOffset, NodeSysBase, NodeSysLimit.
  //

  // Enforce bottom of IO be be 128MB aligned
  BottomIo = (RefPtr->BottomIo & 0xF8) << 8;

  NodeSysBase = 0;
  NodeSysLimit = MCTPtr->NodeMemSize - 1;

  if (NodeSysLimit >= BottomIo) {
    // HW Dram Remap
    MCTPtr->Status[SbHWHole] = TRUE;
    RefPtr->GStatus[GsbHWHole] = TRUE;
    MCTPtr->NodeHoleBase = BottomIo;
    RefPtr->HoleBase = BottomIo;

    HoleOffset = _4GB_RJ16 - BottomIo;
    NodeSysLimit += HoleOffset;

    MemNSetBitFieldNb (NBPtr, BFDramHoleBase, BottomIo >> 8);
    MemNSetBitFieldNb (NBPtr, BFDramHoleOffset, HoleOffset >> 7);
    MemNSetBitFieldNb (NBPtr, BFDramHoleValid, 1);

  } else {
    // No Remapping.  Normal Contiguous mapping
  }
  MCTPtr->NodeSysBase = NodeSysBase;
  MCTPtr->NodeSysLimit = NodeSysLimit;
  RefPtr->SysLimit = MCTPtr->NodeSysLimit;

  WeReMask = 3;
  // Set the Dram base and set the WE and RE flags in the base.
  MemNSetBitFieldNb (NBPtr, BFDramBaseReg0, (NodeSysBase << 8) | WeReMask);
  // Set the Dram limit
  MemNSetBitFieldNb (NBPtr, BFDramLimitReg0, ((NodeSysLimit << 8) & 0xFFFF0000));

  return (BOOLEAN) (NBPtr->MCTPtr->ErrCode < AGESA_FATAL);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *     Report the Uma size that is going to be allocated on Fusion.
 *     Total system memory   UMASize
 *     >=2G                   384M
 *     >=1G                   256M
 *     <1G                     64M
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          Uma size [31:0] = Addr [47:16]
 */
UINT32
MemNGetUmaSizeON (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT32 SysMemSize;
  UINT32 SizeOfUma;

  SysMemSize = NBPtr->RefPtr->SysLimit + 1;
  SysMemSize = (SysMemSize + 0x100) & 0xFFFFF000;   // Ignore 16MB allocated for C6 when finding UMA size
  if (SysMemSize >= 0x8000) {
    SizeOfUma = 384 << (20 - 16);
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
 *
 *   This function programs memory prefetch and priority control
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */

BOOLEAN
MemNFinalizeMctON (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT32 MctCfgLoReg;
  UINT32 MctCfgHiReg;

  // To support ODTS, with assumption that Tref is set to 7.8us always in AGESA
  MemNSetBitFieldNb (NBPtr, BFDoubleTrefRateEn, 1);

  MctCfgLoReg = MemNGetBitFieldNb (NBPtr, BFMctCfgLoReg);
  MemNSetBitFieldNb (NBPtr, BFMctCfgLoReg, (MctCfgLoReg & 0xFFFFF000) | 0x04A4);

  MctCfgHiReg = MemNGetBitFieldNb (NBPtr, BFMctCfgHiReg);
  MemNSetBitFieldNb (NBPtr, BFMctCfgHiReg, (MctCfgHiReg & 0x9E33AFFF) | 0x00404000);

  // PchgPdTxCClkGateDis is 0 by default
  // SelCsrPllPdMode and CsrPhySrPllPdMode is 0 by default
  // SkewMemClk is 0 by default

  //For self-refresh
  MemNSetBitFieldNb (NBPtr, BFDramSrEn, 1);
  MemNSetBitFieldNb (NBPtr, BFDramSrHys, 5);
  if (NBPtr->IsSupported[DramSrHys]) {
    MemNSetBitFieldNb (NBPtr, BFDramSrHysEn, 1);
  }

  MemNSetBitFieldNb (NBPtr, BFMemTriStateEn, 1);
  MemNSetBitFieldNb (NBPtr, BFAcpiPwrStsCtrlHi, MemNGetBitFieldNb (NBPtr, BFAcpiPwrStsCtrlHi) | 0x00060006);

  MemNPhyPowerSavingClientNb (NBPtr);

  // Release NB P-state force
  MemNSetBitFieldNb (NBPtr, BFNbPsCtrlDis, 0);
  MemNSetBitFieldNb (NBPtr, BFNbPsForceReq, 0);

  IDS_OPTION_HOOK (IDS_PHY_DLL_STANDBY_CTRL, NBPtr, &NBPtr->MemPtr->StdHeader);

  return (BOOLEAN) (NBPtr->MCTPtr->ErrCode < AGESA_FATAL);
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
