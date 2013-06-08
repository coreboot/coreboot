/* $NoKeywords:$ */
/**
 * @file
 *
 * mfCrat.c
 *
 * Feature CRAT table support
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Main)
 * @e \$Revision: 64574 $ @e \$Date: 2012-01-25 01:01:51 -0600 (Wed, 25 Jan 2012) $
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
#include "heapManager.h"
#include "cpuServices.h"
#include "mm.h"
#include "mn.h"
#include "mu.h"
#include "mfCrat.h"
#include "GeneralServices.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE (0xF095)
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define FOURGB 0x010000ul

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

CRAT_MEMORY_AFFINITY_INFO_ENTRY
STATIC
*MakeMemAffinityInfoEntry (
  IN UINT8                            Domain,
  IN UINT32                           Base,
  IN UINT32                           Size,
  IN CRAT_MEMORY_AFFINITY_INFO_ENTRY  *BufferLocPtr
  );

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

BOOLEAN
MemFCratSupport (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  );

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *  This function gets CRAT memory affinity info and stores the info into heap
 *
 *     @param[in,out]   *MemMainPtr   - Pointer to the MEM_MAIN_DATA_BLOCK
 *
 */
BOOLEAN
MemFCratSupport (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  )
{
  UINT8    Node;
  UINT32   DramLeng;
  UINT32   DramBase;
  UINT32   DramLimit;
  UINT8    DramRngRE;
  UINT8    DramRngWE;
  UINT8    Domain;
  UINT8    DomainForBase640K;
  UINT32   ValueLimit;
  UINT32   ValueTOM;
  UINT64   MsrValue;
  BOOLEAN  isModified;
  UINT8    MaxNumOfMemAffinityInfoEntries;
  UINT8    NumOfMemAffinityInfoEntries;
  UINT32   TopOfMemoryAbove4Gb;
  CRAT_MEMORY_AFFINITY_INFO_HEADER *MemAffinityInfoHeaderPtr;
  CRAT_MEMORY_AFFINITY_INFO_ENTRY *MemAffinityInfoEntryPtr;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;

  MEM_NB_BLOCK    *NBPtr;
  MEM_DATA_STRUCT *MemPtr;
  DIE_STRUCT      *MCTPtr;
  MEM_SHARED_DATA *SharedPtr;

  NBPtr     = MemMainPtr->NBPtr;
  MemPtr    = MemMainPtr->MemPtr;
  MCTPtr    = NBPtr->MCTPtr;
  SharedPtr = NBPtr->SharedPtr;

  // The maximum number of entries take the following two factors into consideration
  // 1. The entry for conventional memory less than 640k
  // 2. The memory hole below 4G may divide the memory range across the hole into two
  MaxNumOfMemAffinityInfoEntries = NBPtr->NodeCount + 2;

  // Allocate heap for CRAT memory affinity info entry
  AllocHeapParams.RequestedBufferSize = MaxNumOfMemAffinityInfoEntries * sizeof (CRAT_MEMORY_AFFINITY_INFO_ENTRY) +
                                        sizeof (CRAT_MEMORY_AFFINITY_INFO_HEADER);
  AllocHeapParams.BufferHandle = AMD_MEM_CRAT_INFO_BUFFER_HANDLE;
  AllocHeapParams.Persist = HEAP_SYSTEM_MEM;
  if (AGESA_SUCCESS != HeapAllocateBuffer (&AllocHeapParams, &MemPtr->StdHeader)) {
    // Could not allocate heap for CRAT memory affinity info
    PutEventLog (AGESA_CRITICAL, MEM_ERROR_HEAP_ALLOCATE_FOR_CRAT_MEM_AFFINITY, NBPtr->Node, 0, 0, 0, &MemPtr->StdHeader);
    SetMemError (AGESA_CRITICAL, MCTPtr);
    ASSERT (FALSE);
    return FALSE;
  }

  MemAffinityInfoHeaderPtr = (CRAT_MEMORY_AFFINITY_INFO_HEADER *) ((UINT8 *) (AllocHeapParams.BufferPtr));
  MemAffinityInfoHeaderPtr ++;
  MemAffinityInfoEntryPtr  = (CRAT_MEMORY_AFFINITY_INFO_ENTRY *) MemAffinityInfoHeaderPtr;
  MemAffinityInfoHeaderPtr --;

  NumOfMemAffinityInfoEntries = 0;
  DomainForBase640K = 0xFF;

  for (Node = 0; Node < NBPtr->NodeCount; Node++) {
    // FALSE means normal update procedure
    isModified = FALSE;
    // Get DRAM Base Address
    DramBase = MemNGetBitFieldNb (NBPtr, BFDramBaseReg0 + Node);
    DramRngRE = (UINT8) MemNGetBitFieldNb (NBPtr, BFDramRngRE0 + Node);
    DramRngWE = (UINT8) MemNGetBitFieldNb (NBPtr, BFDramRngWE0 + Node);
    if ((DramRngRE == 0) || (DramRngWE == 0)) {
      // 0:1 set if memory range enabled
      // Not set, so we don't have an enabled range
      // Proceed to next Base register
      continue;
    }

    // Get DRAM Limit
    DramLimit = MemNGetBitFieldNb (NBPtr, BFDramLimitReg0 + Node);
    if (DramLimit == 0xFFFFFFFF) {
      // Node not installed(all FF's)?
      // Proceed to next Base register
      continue;
    }

    // Node ID is assigned to Domain
    Domain = (UINT8) MemNGetBitFieldNb (NBPtr, BFDramRngDstNode0 + Node);
    // Get DRAM Limit addr [47:24]
    DramLimit = ((((MemNGetBitFieldNb (NBPtr, BFDramLimitHiReg0 + Node) & 0xFF) << 16) | DramLimit >> 16));
    // Add 1 for potential length
    DramLimit++;
    DramLimit <<= 8;

    // Get DRAM Base Address
    // Get DRAM Base Base value [47:24]
    DramBase = (((MemNGetBitFieldNb (NBPtr, BFDramBaseHiReg0 + Node) & 0xFF) << 24) | (DramBase >> 8) & 0xFFFFFF00);
    // Subtract base from limit to get length
    DramLeng = DramLimit - DramBase;

    // Leave hole for conventional memory (Less than 640K).  It must be on CPU 0.
    if (DramBase == 0) {
      if (DomainForBase640K == 0xFF) {
        // It is the first time that the range start at 0.
        // If Yes, then Place 1MB memory gap and save Domain to PDomainForBase640K
        MemAffinityInfoEntryPtr = MakeMemAffinityInfoEntry (
                                    Domain,
                                    0,              // Base = 0
                                    0xA0000 >> 16,  // Put it into format used in DRAM regs..
                                    MemAffinityInfoEntryPtr
                                    );
        NumOfMemAffinityInfoEntries ++;

        // Add 1MB, so range = 1MB to Top of Region
        DramBase += 0x10;
        // Also subtract 1MB from the length
        DramLeng -= 0x10;
        // Save Domain number for memory Less than 640K
        DomainForBase640K = Domain;
      } else {
        // If No, there are more than one memory range less than 640K, it should that
        // node interleaving is enabled. All nodes have the same memory ranges
        // and all cores in these nodes belong to the same domain.
        Domain = DomainForBase640K;
        break;
      }
    }
    LibAmdMsrRead (TOP_MEM, &MsrValue, &MemPtr->StdHeader);
    // Save it in 39:24 format
    ValueTOM   = (UINT32) MsrValue >> 16;
    // We need to know how large region is
    ValueLimit = DramBase + DramLeng;

    LibAmdMsrRead (SYS_CFG, &MsrValue, &MemPtr->StdHeader);
    if ((MsrValue & BIT21) != 0) {
      LibAmdMsrRead (TOP_MEM2, &MsrValue, &MemPtr->StdHeader);
      // Save it in 47:16 format
      TopOfMemoryAbove4Gb = (UINT32) (MsrValue >> 16);
    } else {
      TopOfMemoryAbove4Gb = 0xFFFFFFFF;
    }

    //  SPECIAL CASES:
    //
    //  Several conditions require that we process the values of the memory range differently.
    //  Here are descriptions of the corner cases.
    //
    //  1. TRUNCATE LOW - Memory range starts below TOM, ends in TOM (memory hole).  For this case,
    //     the range must be truncated to end at TOM.
    //  *******************************            *******************************
    //  *                    *        *      ->    *                             *
    //  *******************************            *******************************
    //  2                    TOM      4            2                             TOM
    //
    //  2. TRUNCATE HIGH - Memory range starts below 4GB, ends above 4GB.  This is handled by changing the
    //     start base to 4GB.
    //          ****************                        **********
    //          *     *        *      ->                *        *
    //          ****************                        **********
    //  TOM     3.8   4        6            TOM   3.8   4        6
    //
    //  3. Memory range starts below TOM, ends above 4GB.  For this case, the range must be truncated
    //     to end at TOM.  Note that this scenario creates two ranges, as the second comparison below
    //     will find that it ends above 4GB since base and limit have been restored after first truncation,
    //     and a second range will be written based at 4GB ending at original end address.
    //  *******************************            ****************          **********
    //  *              *     *        *      ->    *              *          *        *
    //  *******************************            ****************          **********
    //  2              TOM   4        6            2              TOM        4        6
    //
    //  4. Memory range starts above TOM, ends below or equal to 4GB.  This invalid range should simply
    //     be ignored.
    //          *******
    //          *     *      ->    < NULL >
    //          *******
    //  TOM     3.8   4
    //
    //  5. Memory range starts below TOM2, and ends beyond TOM2.  This range must be truncated to TOM2.
    //  ************************         *******************************
    //  *              *       *   ->    *                             *
    //  ************************         *******************************
    //  768          TOM2   1024         768                        TOM2
    //
    //  6. Memory range starts above TOM2.  This invalid range should simply be ignored.
    //          ********************
    //          *                  *    ->    < NULL >
    //          ********************
    //  TOM2    1024            1280

    if (((DramBase < ValueTOM) && (ValueLimit <= FOURGB) && (ValueLimit > ValueTOM))
        || ((DramBase < ValueTOM) && (ValueLimit > FOURGB))) {
      //   TRUNCATE LOW!!! Shrink entry below TOM...
      //   Base = DramBase, Size = TOM - DramBase
      MemAffinityInfoEntryPtr = MakeMemAffinityInfoEntry (Domain, DramBase, (ValueTOM - DramBase), MemAffinityInfoEntryPtr);
      NumOfMemAffinityInfoEntries ++;
      isModified = TRUE;
    }

    if ((ValueLimit > FOURGB) && (DramBase < FOURGB)) {
      //   TRUNCATE HIGH!!! Shrink entry above 4GB...
      //   Size = Base + Size - 4GB, Base = 4GB
      MemAffinityInfoEntryPtr = MakeMemAffinityInfoEntry (Domain, FOURGB, (DramLeng + DramBase - FOURGB), MemAffinityInfoEntryPtr);
      NumOfMemAffinityInfoEntries ++;
      isModified = TRUE;
    }

    if ((DramBase >= ValueTOM) && (ValueLimit <= FOURGB)) {
      //   IGNORE!!!  Entry located entirely within memory hole
      isModified = TRUE;
    }

    if ((DramBase < TopOfMemoryAbove4Gb) && (ValueLimit > TopOfMemoryAbove4Gb)) {
      //   Truncate to TOM2
      //   Base = DramBase, Size = TOM2 - DramBase
      MemAffinityInfoEntryPtr = MakeMemAffinityInfoEntry (Domain, DramBase, (TopOfMemoryAbove4Gb - DramBase), MemAffinityInfoEntryPtr);
      NumOfMemAffinityInfoEntries ++;
      isModified = TRUE;
    }

    if (DramBase >= TopOfMemoryAbove4Gb) {
      //   IGNORE!!!  Entry located entirely above TOM2
      isModified = TRUE;
    }

    //    If special range(isModified), we are done.
    //    If not, finally write the memory entry.
    if (isModified == FALSE) {
      // Finally write the memory entry.
      MemAffinityInfoEntryPtr = MakeMemAffinityInfoEntry (Domain, DramBase, DramLeng, MemAffinityInfoEntryPtr);
      NumOfMemAffinityInfoEntries ++;
    }
  }

  MemAffinityInfoHeaderPtr->NumOfMemAffinityInfoEntries = NumOfMemAffinityInfoEntries;
  MemAffinityInfoHeaderPtr->MemoryWidth = NBPtr->MemNGetMemoryWidth (NBPtr);

  return TRUE;
}

/*---------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 *
 * This function will add Memory entry.
 *
 *  Parameters:
 *    @param[in]  Domain               Proximity Domain
 *    @param[in]  Base                  Memory Base
 *    @param[in]  Size                  Memory Size
 *    @param[in]  BufferLocPtr          Point to the address of buffer
 *
 *    @retval       CRAT_MEMORY_AFFINITY_INFO_ENTRY * (new buffer location ptr)
 */
CRAT_MEMORY_AFFINITY_INFO_ENTRY
STATIC
*MakeMemAffinityInfoEntry (
  IN  UINT8                             Domain,
  IN  UINT32                            Base,
  IN  UINT32                            Size,
  IN  CRAT_MEMORY_AFFINITY_INFO_ENTRY   *BufferLocPtr
  )
{
  BufferLocPtr->Domain = Domain;
  BufferLocPtr->BaseAddressLow = Base << 16;
  BufferLocPtr->BaseAddressHigh = Base >> 16;
  BufferLocPtr->LengthLow = Size << 16;
  BufferLocPtr->LengthHigh = Size >> 16;
  BufferLocPtr ++;

  return BufferLocPtr;
}