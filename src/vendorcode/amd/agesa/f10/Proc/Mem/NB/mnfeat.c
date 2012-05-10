/**
 * @file
 *
 * mnfeat.c
 *
 * Common Northbridge features
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB)
 * @e \$Revision: 7081 $ @e \$Date: 2008-07-31 01:47:27 -0500 (Thu, 31 Jul 2008) $
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
#include "AdvancedApi.h"
#include "amdlib.h"
#include "Ids.h"
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "mu.h"
#include "Filecode.h"
#define FILECODE PROC_MEM_NB_MNFEAT_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define MAX_CL_CONT_READ  32
#define MAX_CL_CONT_WRITE 32

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
MemNContWritePatternNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 Address,
  IN       UINT8 Pattern[],
  IN       UINT16 ClCount
  );

VOID
STATIC
MemNContReadPatternNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT32 Address,
  IN       UINT16 ClCount
  );

VOID
STATIC
MemNGenHwRcvEnReadsNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 Address
  );

UINT16
STATIC
MemNCompareTestPatternClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT8 Pattern[],
  IN       UINT16 ByteCount
  );

UINT16
STATIC
MemNInsDlyCompareTestPatternClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT8 Pattern[],
  IN       UINT16 ByteCount
  );

VOID
STATIC
MemNContWritePatternClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 Address,
  IN       UINT8 Pattern[],
  IN       UINT16 ClCount
  );

VOID
STATIC
MemNContReadPatternClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT32 Address,
  IN       UINT16 ClCount
  );

VOID
STATIC
MemNGenHwRcvEnReadsClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 Address
  );
/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
VOID
MemNInitCPGClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function assigns read/write function pointers to CPG read/write modules.
 *
 *     @param[in,out] NBPtr  - Pointer to the MEM_NB_BLOCK
 *
 */
VOID
MemNInitCPGNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );
VOID
MemNInitCPGNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  NBPtr->WritePattern = MemNContWritePatternNb;
  NBPtr->ReadPattern = MemNContReadPatternNb;
  NBPtr->GenHwRcvEnReads = MemNGenHwRcvEnReadsNb;
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function writes 9 or 18 cache lines continuously using GH CPG engine
 *
 *     @param[in,out] NBPtr  - Pointer to the MEM_NB_BLOCK
 *     @param[in]     Pattern - Array of bytes that will be written to DRAM
 *     @param[in]     Address - System Address [47:16]
 *     @param[in] ClCount - Number of cache lines
 *
 */
VOID
STATIC
MemNContWritePatternNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 Address,
  IN       UINT8 Pattern[],
  IN       UINT16 ClCount
  )
{
  UINT16 ClDiff;
  if (ClCount > MAX_CL_CONT_WRITE) {
    ClDiff = ClCount - MAX_CL_CONT_WRITE;
    ClCount = MAX_CL_CONT_WRITE;
  } else {
    ClDiff = 0;
  }

  // Set F2x11C[MctWrLimit] to desired number of cachelines in the burst.
  MemNSetBitFieldNb (NBPtr, BFMctWrLimit, MAX_CL_CONT_WRITE - ClCount);

  // Issue the stream of writes. When F2x11C[MctWrLimit] is reached (or when F2x11C[FlushWr] is set
  // again), all the writes are written to DRAM.
  Address = MemUSetUpperFSbase (Address, NBPtr->MemPtr);
  MemUWriteCachelines (Address, Pattern, ClCount);

  // Flush out prior writes by setting F2x11C[FlushWr].
  MemNSetBitFieldNb (NBPtr, BFFlushWr, 1);
  // Wait for F2x11C[FlushWr] to clear, indicating prior writes have been flushed.
  while (MemNGetBitFieldNb (NBPtr, BFFlushWr) != 0) {}

  // Set F2x11C[MctWrLimit] to 1Fh to disable write bursting.
  MemNSetBitFieldNb (NBPtr, BFMctWrLimit, 0x1F);

  if (ClDiff > 0) {
    MemNContWritePatternNb (NBPtr, Address + (MAX_CL_CONT_WRITE * 64), Pattern + (MAX_CL_CONT_WRITE * 64), ClDiff);
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function reads 9 or 18 cache lines continuously using GH CPG engine
 *
 *     @param[in,out] NBPtr  - Pointer to the MEM_NB_BLOCK
 *     @param[in,out] Buffer - Array of bytes to be filled with data read from DRAM
 *     @param[in]     Address - System Address [47:16]
 *     @param[in] ClCount - Number of cache lines
 *
 */

VOID
STATIC
MemNContReadPatternNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT32 Address,
  IN       UINT16 ClCount
  )
{
  BOOLEAN DisAutoRefresh;
  UINT16 ClDiff;
  if (ClCount > MAX_CL_CONT_READ) {
    ClDiff = ClCount - MAX_CL_CONT_READ;
    ClCount = MAX_CL_CONT_READ;
  } else {
    ClDiff = 0;
  }

  Address = MemUSetUpperFSbase (Address, NBPtr->MemPtr);

  // 1. BIOS ensures that the only accesses outstanding to the MCT are training reads.
  // 2. If F2x[1, 0]90[BurstLength32]=1, then BIOS ensures that the DCTs and DRAMs are configured for 64
  // byte bursts (8-beat burst length). This requires that BIOS issue MRS commands to the devices
  // to change to an 8-beat burst length and then to restore the desired burst length after training
  // is complete.

  if (MemNGetBitFieldNb (NBPtr, BFDisAutoRefresh) == 0) {
    DisAutoRefresh = FALSE;
    // 3. BIOS programs F2x[1, 0]90[ForceAutoPchg] = 0 and F2x[1, 0]8C[DisAutoRefresh] = 1.
    // 4. If necessary, BIOS programs F2x[1, 0]78[EarlyArbEn] = 1 at this time. See register description.
    MemNSetBitFieldNb (NBPtr, BFDisAutoRefresh, 1);
    // MemNSetBitFieldNb (NBPtr, BFForceAutoPchg, 0);  // ForceAutoPchg is 0 by default.
    MemNSetBitFieldNb (NBPtr, BFZqcsInterval, 0);
  } else {
    DisAutoRefresh = TRUE;
  }

  MemNSetBitFieldNb (NBPtr, BFPrefCpuDis, 0);

  // 5. BIOS sets F2x11C[MctPrefReqLimit] to the number of training reads (Ntrain) it wishes to generate in the
  // training sequence.
  MemNSetBitFieldNb (NBPtr, BFMctPrefReqLimit, ClCount - 1);

  // 6. BIOS sets F2x11C[PrefDramTrainMode] bit.
  // 7. The act of setting F2x11C[PrefDramTrainMode] causes the MCT to flush out the prefetch stride predictor
  // table (removing any existing prefetch stride patterns).
  MemNSetBitFieldNb (NBPtr, BFPrefDramTrainMode, 1);

  // 8. BIOS issues an SFENCE (or other serializing instruction) to ensure that the prior write completes.
  // 9. For revision C and earlier processors, BIOS generates two training reads. For revision D processors BIOS
  // generates three training reads. Three are required to detect the stride with DCQ buddy enabled. These must
  // be to consecutive cache lines (i.e. 64 bytes apart) and must not cross a naturally aligned 4 Kbyte boundary.
  // 10. These reads set up a stride pattern which is detected by the prefetcher. The prefetcher then continues to
  // issue prefetches until F2x11C[MctPrefReqLimit] is reached, at which point the MCT clears
  // F2x11C[PrefDramTrainMode].
  MemUDummyCLRead (Address);
  MemUDummyCLRead (Address + 0x40);
  if (NBPtr->IsSupported[CheckDummyCLRead]) {
    MemUDummyCLRead (Address + 0x80);
  }
  // 11. BIOS issues the remaining (Ntrain - 2 for revisions C and earlier or Ntrain - 3 for revision D) reads after
  // checking that F2x11C[PrefDramTrainMode] is cleared. These reads must be to consecutive cache lines
  // (i.e., 64 bytes apart) and must not cross a naturally aligned 4KB boundary. These reads hit the prefetches
  // and read the data from the prefetch buffer.
  while (MemNGetBitFieldNb (NBPtr, BFPrefDramTrainMode) != 0) {}
  MemUReadCachelines (Buffer, Address, ClCount);

  // 14. BIOS restores the target values for F2x[1, 0]90[ForceAutoPchg], F2x[1, 0]8C[DisAutoRefresh] and
  // F2x[1, 0]90[BurstLength32].
  if (!DisAutoRefresh) {
    MemNSetBitFieldNb (NBPtr, BFDisAutoRefresh, 0);
    MemNSetBitFieldNb (NBPtr, BFZqcsInterval, 2);
  }

  if (ClDiff > 0) {
    MemNContReadPatternNb (NBPtr, Buffer + (MAX_CL_CONT_READ * 64), Address + (MAX_CL_CONT_READ * 64), ClDiff);
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function generates a continuous burst of reads during HW RcvEn training.
 *
 *     @param[in,out] NBPtr  - Pointer to the MEM_NB_BLOCK
 *     @param[in]     Address - System Address [47:16]
 *
 */
VOID
STATIC
MemNGenHwRcvEnReadsNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 Address
  )
{
  UINT8  TempBuffer[12 * 64];
  UINT8  BurstCount;

  for (BurstCount = 0; BurstCount < 10; BurstCount++) {
    NBPtr->ReadPattern (NBPtr, TempBuffer, Address, 12);
    NBPtr->FlushPattern (NBPtr, Address, 12);
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function writes cache lines continuously using TCB CPG engine
 *
 *     @param[in,out] NBPtr  - Pointer to the MEM_NB_BLOCK
 *     @param[in]     Pattern - Array of bytes that will be written to DRAM
 *     @param[in]     Address - System Address [47:16]
 *     @param[in] ClCount - Number of cache lines
 *
 */
VOID
STATIC
MemNContWritePatternClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 Address,
  IN       UINT8 Pattern[],
  IN       UINT16 ClCount
  )
{
  UINT32 PatternHash;
  UINT32 *DwordPtr;
  UINT16 i;

  // 1. Program D18F2x1C0[WrDramTrainMode]=1.
  MemNSetBitFieldNb (NBPtr, BFWrDramTrainMode, 1);

  PatternHash = (ClCount << 24) | (Pattern[9] << 16) | (Pattern[64 + 9] << 8) | Pattern[2 * 64 + 9];
  if (NBPtr->CPGInit != PatternHash) {
    // If write training buffer has not been initialized, initialize it
    // 2. Program D18F2x1C0[TrainLength] to the appropriate number of cache lines.
    MemNSetBitFieldNb (NBPtr, BFTrainLength, ClCount);

    // 3. Program D18F2x1D0[WrTrainBufAddr]=000h.
    MemNSetBitFieldNb (NBPtr, BFWrTrainBufAddr, 0);

    // 4. Successively write each dword of the training pattern to D18F2x1D4.
    DwordPtr = (UINT32 *) Pattern;
    for (i = 0; i < (ClCount * 16); i++) {
      MemNSetBitFieldNb (NBPtr, BFWrTrainBufDat, DwordPtr[i]);
    }

    NBPtr->CPGInit = PatternHash;
  }

  // 5. Program D18F2x1D0[WrTrainBufAddr]=000h
  MemNSetBitFieldNb (NBPtr, BFWrTrainBufAddr, 0);

  // 6. Program the DRAM training address
  MemNSetBitFieldNb (NBPtr, BFWrTrainAdrPtrLo, Address << (16 - 6));
  MemNSetBitFieldNb (NBPtr, BFWrTrainAdrPtrHi, (Address >> (38 - 16)) & 3);

  // 7. Program D18F2x1C0[WrTrainGo]=1.
  MemNSetBitFieldNb (NBPtr, BFWrTrainGo, 1);

  // 8. Wait for D18F2x1C0[WrTrainGo]=0.
  while (MemNGetBitFieldNb (NBPtr, BFWrTrainGo) != 0) {}

  // 9. Program D18F2x1C0[WrDramTrainMode]=0.
  MemNSetBitFieldNb (NBPtr, BFWrDramTrainMode, 0);
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
STATIC
MemNContReadPatternClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT32 Address,
  IN       UINT16 ClCount
  )
{
  // 1. Program D18F2x1C0[RdDramTrainMode]=1.
  MemNSetBitFieldNb (NBPtr, BFRdDramTrainMode, 1);

  // 2. Program D18F2x1C0[TrainLength] to the appropriate number of cache lines.
  MemNSetBitFieldNb (NBPtr, BFTrainLength, ClCount);

  // 3. Program the DRAM training address as follows:
  MemNSetBitFieldNb (NBPtr, BFWrTrainAdrPtrLo, Address << (16 - 6));
  MemNSetBitFieldNb (NBPtr, BFWrTrainAdrPtrHi, (Address >> (38 - 16)) & 3);

  // 4. Program D18F2x1D0[WrTrainBufAddr]=000h
  MemNSetBitFieldNb (NBPtr, BFWrTrainBufAddr, 0);

  // 5. Program D18F2x1C0[RdTrainGo]=1.
  MemNSetBitFieldNb (NBPtr, BFRdTrainGo, 1);

  // 6. Wait for D18F2x1C0[RdTrainGo]=0.
  while (MemNGetBitFieldNb (NBPtr, BFRdTrainGo) != 0) {}

  // 7. Read D18F2x1E8[TrainCmpSts] and D18F2x1E8[TrainCmpSts2].
  // This step will be accomplished in Compare routine.

  // 8. Program D18F2x1C0[RdDramTrainMode]=0.
  MemNSetBitFieldNb (NBPtr, BFRdDramTrainMode, 0);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function generates a continuous burst of reads during HW RcvEn training.
 *
 *     @param[in,out] NBPtr  - Pointer to the MEM_NB_BLOCK
 *     @param[in]     Address - System Address [47:16]
 *
 */
VOID
STATIC
MemNGenHwRcvEnReadsClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 Address
  )
{
  UINT8  TempBuffer[64];
  UINT8  Count;

  for (Count = 0; Count < 3; Count++) {
    NBPtr->ReadPattern (NBPtr, TempBuffer, Address, 64);
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function assigns read/write function pointers to CPG read/write modules.
 *
 *     @param[in,out] NBPtr  - Pointer to the MEM_NB_BLOCK
 *
 */
VOID
MemNInitCPGClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  NBPtr->WritePattern = MemNContWritePatternClientNb;
  NBPtr->ReadPattern = MemNContReadPatternClientNb;
  NBPtr->GenHwRcvEnReads = MemNGenHwRcvEnReadsClientNb;
  NBPtr->FlushPattern = (VOID (*) (MEM_NB_BLOCK *, UINT32, UINT16)) memDefRet;
  NBPtr->CompareTestPattern = MemNCompareTestPatternClientNb;
  NBPtr->InsDlyCompareTestPattern = MemNInsDlyCompareTestPatternClientNb;
  NBPtr->CPGInit = 0;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *       This function compares test pattern with data in buffer and
 *       return a pass/fail bitmap for 8 bytelanes (upper 8 bits are reserved)
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       Buffer[]  -  Buffer data from DRAM (Measured data from DRAM) to compare
 *     @param[in]       Pattern[]  - Pattern (Expected data in ROM/CACHE) to compare against
 *     @param[in]       ByteCount - Byte count
 *
 *     @return  PASS - Bitmap of results of comparison
 */

UINT16
STATIC
MemNCompareTestPatternClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT8 Pattern[],
  IN       UINT16 ByteCount
  )
{
  return ~((UINT16) MemNGetBitFieldNb (NBPtr, BFTrainCmpSts));
}

/*-----------------------------------------------------------------------------*/
/**
 *
 *       This function compares test pattern with data in buffer and
 *       return a pass/fail bitmap for 8 bytelanes (upper 8 bits are reserved)
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       Buffer[]  -  Buffer data from DRAM (Measured data from DRAM) to compare
 *     @param[in]       Pattern[]  - Pattern (Expected data in ROM/CACHE) to compare against
 *     @param[in]       ByteCount - Byte count
 *
 *     @retval  Bitmap of results of comparison
 */
UINT16
STATIC
MemNInsDlyCompareTestPatternClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT8 Pattern[],
  IN       UINT16 ByteCount
  )
{
  return ~((UINT16) MemNGetBitFieldNb (NBPtr, BFTrainCmpSts2));
}