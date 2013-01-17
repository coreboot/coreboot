/* $NoKeywords:$ */
/**
 * @file
 *
 * mn.c
 *
 * Common Northbridge functions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/)
 * @e \$Revision: 63425 $ @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 **/
/*****************************************************************************
*
* Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
*
* AMD is granting you permission to use this software (the Materials)
* pursuant to the terms and conditions of your Software License Agreement
* with AMD.  This header does *NOT* give you permission to use the Materials
* or any rights under AMD's intellectual property.  Your use of any portion
* of these Materials shall constitute your acceptance of those terms and
* conditions.  If you do not agree to the terms and conditions of the Software
* License Agreement, please do not use any portion of these Materials.
*
* CONFIDENTIALITY:  The Materials and all other information, identified as
* confidential and provided to you by AMD shall be kept confidential in
* accordance with the terms and conditions of the Software License Agreement.
*
* LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
* PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
* MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
* OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
* IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
* (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
* INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
* GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
* RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
* EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
* THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
*
* AMD does not assume any responsibility for any errors which may appear in
* the Materials or any other related information provided to you by AMD, or
* result from use of the Materials or any related information.
*
* You agree that you will not reverse engineer or decompile the Materials.
*
* NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
* further information, software, technical information, know-how, or show-how
* available to you.  Additionally, AMD retains the right to modify the
* Materials at any time, without notice, and is not obligated to provide such
* modified Materials to you.
*
* U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
* "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
* subject to the restrictions as set forth in FAR 52.227-14 and
* DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
* Government constitutes acknowledgement of AMD's proprietary rights in them.
*
* EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
* direct product thereof will be exported directly or indirectly, into any
* country prohibited by the United States Export Administration Act and the
* regulations thereunder, without the required authorization from the U.S.
* government nor will be used for any purpose prohibited by the same.
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
#include "AdvancedApi.h"
#include "amdlib.h"
#include "Ids.h"
#include "mport.h"
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "mu.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_NB_MN_FILECODE


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
extern OPTION_MEM_FEATURE_NB* memNTrainFlowControl[];

extern BUILD_OPT_CFG UserOptions;

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function initializes member functions and variables of NB block.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNInitNBDataNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  INT32 i;
  UINT8 *BytePtr;

  NBPtr->DctCachePtr = NBPtr->DctCache;
  NBPtr->PsPtr = NBPtr->PSBlock;

  BytePtr = (UINT8 *) (NBPtr->DctCache);
  for (i = 0; i < sizeof (NBPtr->DctCache); i++) {
    *BytePtr++ = 0;
  }

  for (i = 0; i < EnumSize; i++) {
    NBPtr->IsSupported[i] = FALSE;
  }

  for (i = 0; i < NumberOfHooks; i++) {
    NBPtr->FamilySpecificHook[i] = MemNDefaultFamilyHookNb;
  }

  for (i = 0; i < NBPtr->DctCount; i++) {
    NBPtr->PSBlock[i].MemPGetPass1Seeds = (BOOLEAN (*) (MEM_NB_BLOCK *)) memDefTrue;
  }

  NBPtr->SwitchDCT = MemNSwitchDCTNb;
  NBPtr->SwitchChannel = MemNSwitchChannelNb;
  NBPtr->GetBitField = MemNGetBitFieldNb;
  NBPtr->SetBitField = MemNSetBitFieldNb;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   Get System address of Chipselect RJ 16 bits (Addr[47:16])
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       Receiver - Chipselect to be targeted [0-7]
 *     @param[out]      AddrPtr -  Pointer to System Address [47:16]
 *
 *     @return      TRUE  - Address is valid
 *     @return      FALSE  - Address is not valid
 */

BOOLEAN
MemNGetMCTSysAddrNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Receiver,
     OUT   UINT32 *AddrPtr
  )
{
  S_UINT64 SMsr;
  UINT32 CSBase;
  UINT32 HoleBase;
  UINT32 DctSelBaseAddr;
  UINT32 BottomUma;
  DIE_STRUCT *MCTPtr;
  MEM_DATA_STRUCT *MemPtr;

  MCTPtr = NBPtr->MCTPtr;
  MemPtr = NBPtr->MemPtr;

  ASSERT (Receiver < 8);

  CSBase = MemNGetBitFieldNb (NBPtr, BFCSBaseAddr0Reg + Receiver);
  if (CSBase & 1) {
    ASSERT ((CSBase & 0xE0) == 0);   // Should not enable CS interleaving before DQS training.

    // Scale base address from [39:8] to [47:16]
    CSBase >>= 8;

    HoleBase = MCTPtr->NodeHoleBase ? MCTPtr->NodeHoleBase : 0x7FFFFFFF;

    if ((MemNGetBitFieldNb (NBPtr, BFDctSelHiRngEn) == 1) && (NBPtr->Dct == MemNGetBitFieldNb (NBPtr, BFDctSelHi))) {
      DctSelBaseAddr = MemNGetBitFieldNb (NBPtr, BFDctSelBaseAddr) << (27 - 16);
      if (DctSelBaseAddr > HoleBase) {
        DctSelBaseAddr -= _4GB_RJ16 - HoleBase;
      }
      CSBase += DctSelBaseAddr;
    } else {
      CSBase += MCTPtr->NodeSysBase;
    }

    if (CSBase >= HoleBase) {
      CSBase += _4GB_RJ16 - HoleBase;
    }

    CSBase += (UINT32)1 << (21 - 16);  // Add 2MB offset to avoid compat area.
    if ((CSBase >= (MCT_TRNG_KEEPOUT_START >> 8)) && (CSBase <= (MCT_TRNG_KEEPOUT_END >> 8))) {
      CSBase += (((MCT_TRNG_KEEPOUT_END >> 8) - CSBase) + 0x0F) & 0xFFFFFFF0;
    }

    if (MCTPtr->Status[SbHWHole]) {
      if (MCTPtr->Status[SbSWNodeHole]) {
        LibAmdMsrRead (TOP_MEM, (UINT64 *)&SMsr, &MemPtr->StdHeader);

        if ((CSBase >= (SMsr.lo >> 16)) && (CSBase < _4GB_RJ16)) {
          return FALSE;
        }
      }
    }

    BottomUma = NBPtr->RefPtr->Sub4GCacheTop >> 16;
    if (BottomUma && (CSBase >= BottomUma) && (CSBase < _4GB_RJ16)) {
      return FALSE;
    }
    *AddrPtr = CSBase;
    return TRUE;
  }
  return FALSE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function determines if a Rank is enabled.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]    Receiver - Receiver to check
 *     @return - FALSE
 *
 */

BOOLEAN
MemNRankEnabledNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Receiver
  )
{
  UINT32 CSBase;
  CSBase = MemNGetBitFieldNb (NBPtr, BFCSBaseAddr0Reg + Receiver);
  if (CSBase & 1) {
    return  TRUE;
  } else {
    return  FALSE;
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *     This function sets the EccSymbolSize bit depending upon configurations
 *     and system override.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNSetEccSymbolSizeNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT16 X4DimmsOnly;
  BOOLEAN Size;
  DIE_STRUCT *MCTPtr;
  DCT_STRUCT *DCTPtr;

  ASSERT (NBPtr != NULL);

  MCTPtr = NBPtr->MCTPtr;
  DCTPtr = NBPtr->DCTPtr;

  // Determine if this node has only x4 DRAM parts
  X4DimmsOnly = (UINT16) ((!(DCTPtr->Timings.Dimmx8Present | DCTPtr->Timings.Dimmx16Present)) && DCTPtr->Timings.Dimmx4Present);
  //
  // Check if EccSymbolSize BKDG value is overridden
  //
  if (UserOptions.CfgEccSymbolSize != ECCSYMBOLSIZE_USE_BKDG) {
    Size = (UserOptions.CfgEccSymbolSize == ECCSYMBOLSIZE_FORCE_X4) ? FALSE : TRUE;
  } else {
    if (X4DimmsOnly && MCTPtr->GangedMode) {
      Size = FALSE;
    } else {
      Size = TRUE;
    }
  }
  IDS_OPTION_HOOK (IDS_ECCSYMBOLSIZE, &Size, &(NBPtr->MemPtr->StdHeader));
  MemNSetBitFieldNb (NBPtr, BFEccSymbolSize, (UINT32) Size);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function sets the training control flow
 *      The DDR3 mode bit must be set prior to calling this function
 *
 *      @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 */
BOOLEAN
MemNTrainingFlowNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  if (MemNGetBitFieldNb (NBPtr, BFDdr3Mode)!= 0) {
    memNTrainFlowControl[DDR3_TRAIN_FLOW] (NBPtr);
  } else {
    memNTrainFlowControl[DDR2_TRAIN_FLOW] (NBPtr);
  }
  return TRUE;
}

/*-----------------------------------------------------------------------------*/
/**
 *
 *      This function flushes the  training pattern
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in] Address - System Address [47:16]
 *     @param[in] ClCount - Number of cache lines
 *
 */

VOID
MemNFlushPatternNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 Address,
  IN       UINT16 ClCount
  )
{
  // Due to speculative execution during MemUReadCachelines, we must
  //  flush one more cache line than we read.
  MemUProcIOClFlush (Address, ClCount + 1, NBPtr->MemPtr);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function compares test pattern with data in buffer and
 *     return a pass/fail bitmap for 8 bytelanes (upper 8 bits are reserved)
 *
 *     @param[in,out]   *NBPtr    - Pointer to the MEM_NB_BLOCK
 *     @param[in]       Buffer[]  - Buffer data from DRAM (Measured data from DRAM) to compare
 *     @param[in]       Pattern[] - Pattern (Expected data in ROM/CACHE) to compare against
 *     @param[in]       ByteCount - Byte count
 *
 *     @return  PASS - Bitmap of results of comparison
 */

UINT16
MemNCompareTestPatternNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT8 Pattern[],
  IN       UINT16 ByteCount
  )
{
  UINT16 i;
  UINT16 Pass;
  UINT8 ColumnCount;
  UINT8 FailingBitMask[8];

  ASSERT ((ByteCount == 18 * 64) || (ByteCount == 9 * 64) || (ByteCount == 64 * 64) || (ByteCount == 32 * 64) || (ByteCount == 3 * 64));

  ColumnCount = NBPtr->ChannelPtr->ColumnCount;
  Pass = 0xFFFF;
  //
  // Clear Failing Bit Mask
  //
  for (i = 0; i < sizeof (FailingBitMask); i++) {
    FailingBitMask[i] = 0;
  }

  if (NBPtr->Ganged && (NBPtr->Dct != 0)) {
    i = 8;   // DCT 1 in ganged mode
  } else {
    i = 0;
  }

  for (; i < ByteCount; i++) {
    if (Buffer[i] != Pattern[i]) {
      // if bytelane n fails
      Pass &= ~((UINT16)1 << (i % 8));    // clear bit n
      FailingBitMask[i % NBPtr->TechPtr->MaxByteLanes ()] |= (Buffer[i] ^ Pattern[i]);
    }

    if (NBPtr->Ganged && ((i & 7) == 7)) {
      i += 8;     // if ganged, skip over other Channel's Data
    }
  }
  //
  // Accumulate Failing bit data
  //
  for (i = 0; i < sizeof (FailingBitMask); i++) {
    NBPtr->ChannelPtr->FailingBitMask[(ColumnCount * NBPtr->TechPtr->ChipSel) + i] &=
      FailingBitMask[i];
  }

  return Pass;
}

/*-----------------------------------------------------------------------------
 *
 *
 *     This function compares test pattern with data in buffer and
 *     return a pass/fail bitmap for 8 bytelanes (upper 8 bits are reserved)
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in]       Buffer[]  - Buffer data from DRAM (Measured data from DRAM) to compare
 *     @param[in]       Pattern[] - Pattern (Expected data in ROM/CACHE) to compare against
 *     @param[in]       ByteCount - Byte count
 *
 *     @retval  PASS - Bitmap of results of comparison
 * ----------------------------------------------------------------------------
 */
UINT16
MemNInsDlyCompareTestPatternNb (
  IN       MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT8 Pattern[],
  IN       UINT16 ByteCount
  )
{
  UINT16 i;
  UINT16 Pass;
  UINT16 BeatOffset;
  UINT16 BeatCnt;
  UINT8 ColumnCount;
  UINT8 FailingBitMask[8];

  ASSERT ((ByteCount == 18 * 64) || (ByteCount == 9 * 64) || (ByteCount == 64 * 64) || (ByteCount == 32 * 64) || (ByteCount == 3 * 64));

  ColumnCount = NBPtr->ChannelPtr->ColumnCount;
  Pass = 0xFFFF;
  //
  // Clear Failing Bit Mask
  //
  for (i = 0; i < sizeof (FailingBitMask); i++) {
    FailingBitMask[i] = 0;
  }

  if (NBPtr->Ganged && (NBPtr->Dct != 0)) {
    i = 8;   // DCT 1 in ganged mode
  } else {
    i = 0;
  }

  if (NBPtr->Ganged) {
    BeatOffset = 16;
  } else {
    BeatOffset = 8;
  }

  BeatCnt = 0;
  for (; i < ByteCount; i++) {

    if (Buffer[i] != Pattern[i + BeatOffset]) {
      // if bytelane n fails
      Pass &= ~((UINT16)1 << (i % 8));    // clear bit n
      FailingBitMask[i % NBPtr->TechPtr->MaxByteLanes ()] |= (Buffer[i] ^ Pattern[i + BeatOffset]);
    }

    if ((i & 7) == 7) {
      if (NBPtr->Ganged) {
        i += 8;     // if ganged, skip over other Channel's Data
      }
      BeatCnt++;
    }

    if ((BeatCnt & 3) == 3) {
      // Skip last data beat of a 4-beat burst.
      BeatCnt++;
      i = i + BeatOffset;
    }
  }
  //
  // Accumulate Failing bit data
  //
  for (i = 0; i < sizeof (FailingBitMask); i++) {
    NBPtr->ChannelPtr->FailingBitMask[(ColumnCount * NBPtr->TechPtr->ChipSel) + i] &=
      FailingBitMask[i];
  }

  return Pass;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function sets the training control flow for UNB
 *      The DDR3 mode bit must be set prior to calling this function
 *
 *      @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 */
VOID
MemNTrainingFlowUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  memNTrainFlowControl[DDR3_TRAIN_FLOW] (NBPtr);
  return;
}
/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

