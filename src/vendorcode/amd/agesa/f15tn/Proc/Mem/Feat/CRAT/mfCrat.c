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