/* $NoKeywords:$ */
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
 * @e \$Revision: 38442 $ @e \$Date: 2010-09-24 06:39:57 +0800 (Fri, 24 Sep 2010) $
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
#include "AdvancedApi.h"
#include "amdlib.h"
#include "Ids.h"
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "mu.h"
#include "PlatformMemoryConfiguration.h"
#include "merrhdl.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

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
MemNInitCPGNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNInitDqsTrainRcvrEnHwNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNDisableDqsTrainRcvrEnHwNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

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

BOOLEAN
STATIC
MemNBeforeMemClrClientNb (
  IN OUT   MEM_NB_BLOCK  *NBPtr,
  IN       VOID *UnUsed
  );

VOID
STATIC
MemNGenHwRcvEnReadsUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 Address
  );

VOID
STATIC
MemNRrwActivateCmd (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 ChipSelect,
  IN       UINT8 Bank,
  IN       UINT32 RowAddress
  );

VOID
STATIC
MemNRrwPrechargeCmd (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 ChipSelect,
  IN       UINT8 Bank
  );

VOID
STATIC
MemNContReadPatternUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT32 Address,
  IN       UINT16 ClCount
  );

VOID
STATIC
MemNContWritePatternUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 Address,
  IN       UINT8 Pattern[],
  IN       UINT16 ClCount
  );

VOID
MemNInitCPGClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNInitCPGUnb (
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
 *      This function assigns read/write function pointers to CPG read/write modules.
 *
 *     @param[in,out] NBPtr  - Pointer to the MEM_NB_BLOCK
 *
 */
VOID
MemNInitCPGNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  NBPtr->WritePattern = MemNContWritePatternNb;
  NBPtr->ReadPattern = MemNContReadPatternNb;
  NBPtr->GenHwRcvEnReads = MemNGenHwRcvEnReadsNb;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function initializes member functions of HW Rx En Training.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNInitDqsTrainRcvrEnHwNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  NBPtr->MemNPrepareRcvrEnDlySeed = MemNPrepareRcvrEnDlySeedNb;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function disables member functions of Hw Rx En Training.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNDisableDqsTrainRcvrEnHwNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  NBPtr->MemNPrepareRcvrEnDlySeed = (VOID (*) (MEM_NB_BLOCK *)) memDefRet;
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
  UINT16 j;
  UINT16 Multiplier;

  Multiplier = 1;

  // 1. Program D18F2x1C0[WrDramTrainMode]=1.
  MemNSetBitFieldNb (NBPtr, BFWrDramTrainMode, 1);

  PatternHash = ClCount << 24;
  for (i = 0; i < 3; i ++) {
    PatternHash |= (Pattern[i * ClCount * 24 + 9] << (8 * i));
  }
  if (NBPtr->CPGInit != PatternHash) {

    if (ClCount == 3) {
      // Double pattern length for MaxRdLat training
      Multiplier = 2;
    }

    // If write training buffer has not been initialized, initialize it
    // 2. Program D18F2x1C0[TrainLength] to the appropriate number of cache lines.
    MemNSetBitFieldNb (NBPtr, BFTrainLength, ClCount * Multiplier);

    // 3. Program D18F2x1D0[WrTrainBufAddr]=000h.
    MemNSetBitFieldNb (NBPtr, BFWrTrainBufAddr, 0);

    // 4. Successively write each dword of the training pattern to D18F2x1D4.
    DwordPtr = (UINT32 *) Pattern;
    for (j = 0; j < Multiplier; j++) {
      for (i = 0; i < (ClCount * 16); i++) {
        MemNSetBitFieldNb (NBPtr, BFWrTrainBufDat, DwordPtr[i]);
      }
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
  UINT16 Multiplier;

  Multiplier = 1;
  if (ClCount == 3) {
    // Double pattern length for MaxRdLat training
    Multiplier = 2;
  }

  // 1. Program D18F2x1C0[RdDramTrainMode]=1.
  MemNSetBitFieldNb (NBPtr, BFRdDramTrainMode, 1);

  // 2. Program D18F2x1C0[TrainLength] to the appropriate number of cache lines.
  MemNSetBitFieldNb (NBPtr, BFTrainLength, ClCount * Multiplier);

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
  NBPtr->FamilySpecificHook[BeforeMemClr] = MemNBeforeMemClrClientNb;
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

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function calculates RcvEn seed value for each rank
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */
VOID
MemNPrepareRcvrEnDlySeedNb (
  IN OUT   MEM_NB_BLOCK  *NBPtr
  )
{
  MEM_TECH_BLOCK *TechPtr;
  CH_DEF_STRUCT *ChannelPtr;
  DIE_STRUCT *MCTPtr;
  UINT16 SeedTotal;
  UINT16 SeedFine;
  UINT16 SeedGross;
  UINT16 SeedPreGross;
  UINT16 SeedTotalPreScaling;
  UINT8  ByteLane;
  UINT16 Speed;
  UINT16 PlatEst;
  UINT8 ChipSel;
  UINT8 Pass;
  UINT16 *PlatEstSeed;
  UINT16 SeedValue[9];
  UINT16 SeedTtl[9];
  UINT16 SeedPre[9];

  TechPtr = NBPtr->TechPtr;
  MCTPtr = NBPtr->MCTPtr;
  ChannelPtr = TechPtr->NBPtr->ChannelPtr;
  Speed = NBPtr->DCTPtr->Timings.Speed;
  SeedTotalPreScaling = 0;
  ChipSel = TechPtr->ChipSel;
  Pass = TechPtr->Pass;

  for (ByteLane = 0; ByteLane < (MCTPtr->Status[SbEccDimms] ? 9 : 8); ByteLane++) {
    TechPtr->Bytelane = ByteLane;
    if (Pass == 1) {
      // Get platform override seed
      PlatEstSeed = (UINT16 *) FindPSOverrideEntry (NBPtr->RefPtr->PlatformMemoryConfiguration, PSO_RXEN_SEED, MCTPtr->SocketId, ChannelPtr->ChannelID);
      // For Pass1, BIOS starts with the delay value obtained from the first pass of write
      // levelization training that was done in DDR3 Training and add a delay value of 3Bh.
      PlatEst = 0x3B;
      NBPtr->FamilySpecificHook[OverrideRcvEnSeed] (NBPtr, &PlatEst);
      PlatEst = ((PlatEstSeed != NULL) ? PlatEstSeed[ByteLane] : PlatEst);
      SeedTotal = ChannelPtr->WrDqsDlys[(ChipSel >> 1) * TechPtr->DlyTableWidth () + ByteLane] + PlatEst;
      SeedValue[ByteLane] = PlatEst;
    } else {
      // For Pass2
      // SeedTotalPreScaling = (the total delay values in D18F2x[1,0]9C_x0000_00[24:10] from pass 1 of
      //  DQS receiver enable training) - 20h. Subtract 1UI to get back to preamble left edge.
      if ((ChipSel & 1) == 0) {
        // Save Seed for odd CS SeedTotalPreScaling RxEn Value
        TechPtr->PrevPassRcvEnDly[ByteLane] = ChannelPtr->RcvEnDlys[(ChipSel >> 1) * TechPtr->DlyTableWidth () + ByteLane];
      }
      SeedTotalPreScaling = TechPtr->PrevPassRcvEnDly[ByteLane] - 0x20;
      // SeedTotal = SeedTotalPreScaling*target frequency/lowest supported frequency.
      SeedTotal = (UINT16) (((UINT32) SeedTotalPreScaling * Speed) / TechPtr->PrevSpeed);
      NBPtr->FamilySpecificHook[OverrideRcvEnSeedPassN] (NBPtr, &SeedTotal);
    }
    SeedTtl[ByteLane] = SeedTotal;

    // SeedGross = SeedTotal DIV 32.
    SeedGross = SeedTotal >> 5;
    // SeedFine = SeedTotal MOD 32.
    SeedFine = SeedTotal & 0x1F;
    // Next, determine the gross component of SeedTotal. SeedGrossPass1=SeedTotal DIV 32.
    // Then, determine the fine delay component of SeedTotal. SeedFinePass1=SeedTotal MOD 32.
    // Use SeedGrossPass1 to determine SeedPreGrossPass1:

    if ((SeedGross & 0x1) != 0) {
      //if SeedGross is odd
      SeedPreGross = 1;
    } else {
      //if SeedGross is even
      SeedPreGross = 2;
    }
    // (SeedGross - SeedPreGross)
    TechPtr->DiffSeedGrossSeedPreGross[ByteLane] = (SeedGross - SeedPreGross) << 5;

    //BIOS programs registers F2x[1, 0]9C_x[51:50] and F2x[1, 0]9C_x52 with SeedPreGrossPass1
    //and SeedFinePass1 from the preceding steps.

    NBPtr->SetTrainDly (NBPtr, AccessPhRecDly, DIMM_BYTE_ACCESS (ChipSel >> 1, ByteLane), (SeedPreGross << 5) | SeedFine);
    SeedPre[ByteLane] = (SeedPreGross << 5) | SeedFine;

    // 202688: Program seed value to RcvEnDly also.
    NBPtr->SetTrainDly (NBPtr, AccessRcvEnDly, DIMM_BYTE_ACCESS (ChipSel >> 1, ByteLane), SeedGross << 5);
  }

  IDS_HDT_CONSOLE_DEBUG_CODE (
    if (Pass == 1) {
      IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tSeedValue: ");
      for (ByteLane = 0; ByteLane < (MCTPtr->Status[SbEccDimms] ? 9 : 8); ByteLane++) {
        IDS_HDT_CONSOLE (MEM_FLOW, "%03x ", SeedValue[ByteLane]);
      }
      IDS_HDT_CONSOLE (MEM_FLOW, "\n");
    }
    IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tSeedTotal: ");
    for (ByteLane = 0; ByteLane < (MCTPtr->Status[SbEccDimms] ? 9 : 8); ByteLane++) {
      IDS_HDT_CONSOLE (MEM_FLOW, "%03x ", SeedTtl[ByteLane]);
    }
    IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\t\t  SeedPRE: ");
    for (ByteLane = 0; ByteLane < (MCTPtr->Status[SbEccDimms] ? 9 : 8); ByteLane++) {
      IDS_HDT_CONSOLE (MEM_FLOW, "%03x ", SeedPre[ByteLane]);
    }
    IDS_HDT_CONSOLE (MEM_FLOW, "\n");
  );
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  Waits specified number of MEMCLKs
 *      @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *      @param[in] MemClkCount - Number of MEMCLKs
 *
 * ----------------------------------------------------------------------------
 */
VOID
MemNWaitXMemClksNb (
  IN OUT   MEM_NB_BLOCK  *NBPtr,
  IN       UINT32 MemClkCount
  )
{
  MemUWait10ns ((MemClkCount * 100 + NBPtr->DCTPtr->Timings.Speed - 1) / NBPtr->DCTPtr->Timings.Speed, NBPtr->MemPtr);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  Issues dummy TCB write read to zero out CL that is used for MemClr
 *      @param[in,out] *NBPtr  - Pointer to the MEM_NB_BLOCK
 *      @param[in,out] *UnUsed - unused
 *
 * ----------------------------------------------------------------------------
 */
BOOLEAN
STATIC
MemNBeforeMemClrClientNb (
  IN OUT   MEM_NB_BLOCK  *NBPtr,
  IN       VOID *UnUsed
  )
{
  UINT8 Pattern[64];
  UINT8 i;

  for (i = 0; i < 64; i++) {
    Pattern[i] = 0;
  }

  MemNContWritePatternClientNb (NBPtr, 0x20, Pattern, 1);
  MemNContReadPatternClientNb (NBPtr, Pattern, 0x20, 1);
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function uses the PRBS generator in the DCT to send a DDR Activate command
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]        ChipSelect - Chip select 0-7
 *     @param[in]        Bank - Bank Address 0-7
 *     @param[in]        RowAddress - Row Address [17:0]
 *
 */

VOID
STATIC
MemNRrwActivateCmd (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 ChipSelect,
  IN       UINT8 Bank,
  IN       UINT32 RowAddress
  )
{
  // Set Chip select
  MemNSetBitFieldNb (NBPtr, BFCmdChipSelect, (1 << ChipSelect));
  // Set Bank Address
  MemNSetBitFieldNb (NBPtr, BFCmdBank, Bank);
  // Set Row Address
  MemNSetBitFieldNb (NBPtr, BFCmdAddress, RowAddress);
  // Send the command
  MemNSetBitFieldNb (NBPtr, BFSendActCmd, 1);
  // Wait for command complete
  MemNPollBitFieldNb (NBPtr, BFSendActCmd, 0, PCI_ACCESS_TIMEOUT, FALSE);
  // Wait 75 MEMCLKs
  NBPtr->WaitXMemClks (NBPtr, 75);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function uses the PRBS generator in the DCT to send a DDR Precharge
 *     or Precharge All command
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]        ChipSelect - Chip select 0-7
 *     @param[in]        Bank - Bank Address 0-7, PRECHARGE_ALL_BANKS = Precharge All
 *
 *
 */

VOID
STATIC
MemNRrwPrechargeCmd (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 ChipSelect,
  IN       UINT8 Bank
  )
{
  // Wait 25 MEMCLKs
  NBPtr->WaitXMemClks (NBPtr, 25);
  // Set Chip select
  NBPtr->SetBitField (NBPtr, BFCmdChipSelect, (1 << ChipSelect));
  if (Bank == PRECHARGE_ALL_BANKS) {
    // Set Row Address, bit 10
    NBPtr->SetBitField (NBPtr, BFCmdAddress,  NBPtr->GetBitField (NBPtr, BFCmdAddress) | (1 << 10) );
  } else {
    // Clear Row Address, bit 10
    NBPtr->SetBitField (NBPtr, BFCmdAddress,  NBPtr->GetBitField (NBPtr, BFCmdAddress) & (~(1 << 10)) );
    // Set Bank Address
    NBPtr->SetBitField (NBPtr, BFCmdBank, Bank);
  }
  // Send the command
  NBPtr->SetBitField (NBPtr, BFSendPchgCmd, 1);
  // Wait for command complete
  NBPtr->PollBitField (NBPtr, BFSendPchgCmd, 0, PCI_ACCESS_TIMEOUT, FALSE);
  // Wait 25 MEMCLKs
  NBPtr->WaitXMemClks (NBPtr, 25);
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function generates a continuous burst of reads for HW RcvEn
 *        training using the Unified Northbridge Reliable Read/Write Engine.
 *
 *     @param[in,out] NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]     Address - Unused by this function
 *
 */
VOID
STATIC
MemNGenHwRcvEnReadsUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 Address
  )
{
  VOID  *DummyPtr;
  DummyPtr = NULL;
  //
  // Issue Stream of Reads from the Target Rank
  //
  NBPtr->ReadPattern (NBPtr, DummyPtr, 0, NBPtr->TechPtr->PatternLength);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function generates a continuous stream of reads from DRAM using the
 *       Unified Northbridge Reliable Read/Write Engine.
 *
 *     @param[in,out] NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out] Buffer  - Unused by this function
 *     @param[in]     Address - Unused by this function
 *     @param[in]     ClCount - Number of cache lines to read
 *
 *     Assumptions:
 *
 *
 *
 */

VOID
STATIC
MemNContReadPatternUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT32 Address,
  IN       UINT16 ClCount
  )
{
  MEM_TECH_BLOCK *TechPtr;
  RRW_SETTINGS *Rrw;
  UINT8 CmdTgt;
  UINT8 ChipSel;

  TechPtr = NBPtr->TechPtr;
  Rrw = &NBPtr->RrwSettings;

  ChipSel = TechPtr->ChipSel;
  CmdTgt = Rrw->CmdTgt;
  //
  // Wait for RRW Engine to be ready and turn it on
  //
  NBPtr->PollBitField (NBPtr, BFCmdSendInProg, 0, PCI_ACCESS_TIMEOUT, FALSE);
  NBPtr->SetBitField (NBPtr, BFCmdTestEnable, 1);
  //
  // Depending upon the Cmd Target, send Row Activate and set Chipselect
  //   for the Row or Rows that will be used
  //
  MemNRrwActivateCmd (NBPtr, ChipSel, Rrw->TgtBankAddressA, Rrw->TgtRowAddressA);
  NBPtr->SetBitField (NBPtr, BFTgtChipSelectA, ChipSel);
  if (CmdTgt == CMD_TGT_AB) {
    MemNRrwActivateCmd (NBPtr, ChipSel, Rrw->TgtBankAddressB, Rrw->TgtRowAddressB);
    NBPtr->SetBitField (NBPtr, BFTgtChipSelectB, ChipSel);
  }
  // Set Comparison Masks
  NBPtr->SetBitField (NBPtr, BFDramDqMaskLow, Rrw->CompareMaskLow);
  NBPtr->SetBitField (NBPtr, BFDramDqMaskHigh, Rrw->CompareMaskHigh);
  //
  // If All Dimms are ECC Capable Test ECC. Otherwise, mask it off
  //
  NBPtr->SetBitField (NBPtr, BFDramEccMask, (NBPtr->MCTPtr->Status[SbEccDimms] == TRUE) ? Rrw->CompareMaskEcc : 0xFF);
  //
  // Program the PRBS Seed
  //
  NBPtr->SetBitField (NBPtr, BFDataPrbsSeed, Rrw->DataPrbsSeed);
  //
  // Set the Command Count
  //
  NBPtr->SetBitField (NBPtr, BFCmdCount, ClCount);
  //
  // Program the Bubble Count and CmdStreamLen
  //
  NBPtr->SetBitField (NBPtr, BFBubbleCnt, 0);
  NBPtr->SetBitField (NBPtr, BFBubbleCnt2, 0);
  NBPtr->SetBitField (NBPtr, BFCmdStreamLen, 1);
  //
  // Program the Starting Address
  //
  NBPtr->SetBitField (NBPtr, BFTgtBankA, Rrw->TgtBankAddressA);
  NBPtr->SetBitField (NBPtr, BFTgtAddressA, Rrw->TgtColAddressA);
  if (CmdTgt == CMD_TGT_AB) {
    NBPtr->SetBitField (NBPtr, BFTgtBankB, Rrw->TgtBankAddressB);
    NBPtr->SetBitField (NBPtr, BFTgtAddressB, Rrw->TgtColAddressB);
  }
  //
  // Reset All Errors and Disable StopOnErr
  //
  NBPtr->SetBitField (NBPtr, BFResetAllErr, 1);
  NBPtr->SetBitField (NBPtr, BFStopOnErr, 0);
  //
  // Program the CmdTarget
  //
  NBPtr->SetBitField (NBPtr, BFCmdTgt, CmdTgt);
  //
  // Set CmdType to read
  //
  NBPtr->SetBitField (NBPtr, BFCmdType, CMD_TYPE_READ);
  //
  // Start the Commands
  //
  NBPtr->SetBitField (NBPtr, BFSendCmd, 1);
  //
  // Commands have started, wait for the reads to complete then clear the command
  //
  NBPtr->PollBitField (NBPtr, BFTestStatus, 1, PCI_ACCESS_TIMEOUT, FALSE);
  NBPtr->PollBitField (NBPtr, BFCmdSendInProg, 0, PCI_ACCESS_TIMEOUT, FALSE);
  NBPtr->SetBitField (NBPtr, BFSendCmd, 0);
  //
  // Send the Precharge All Command
  //
  MemNRrwPrechargeCmd (NBPtr, ChipSel, PRECHARGE_ALL_BANKS);
  //
  // Turn Off the RRW Engine
  //
  NBPtr->SetBitField (NBPtr, BFCmdTestEnable, 0);
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
STATIC
MemNContWritePatternUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 Address,
  IN       UINT8 Pattern[],
  IN       UINT16 ClCount
  )
{
  MEM_TECH_BLOCK *TechPtr;
  RRW_SETTINGS *Rrw;
  UINT8 CmdTgt;
  UINT8 ChipSel;

  TechPtr = NBPtr->TechPtr;
  Rrw = &NBPtr->RrwSettings;

  ChipSel = TechPtr->ChipSel;
  CmdTgt = Rrw->CmdTgt;
  //
  // Wait for RRW Engine to be ready and turn it on
  //
  NBPtr->PollBitField (NBPtr, BFCmdSendInProg, 0, PCI_ACCESS_TIMEOUT, FALSE);
  NBPtr->SetBitField (NBPtr, BFCmdTestEnable, 1);

  //
  // Depending upon the Cmd Target, send Row Activate and set Chipselect
  //   for the Row or Rows that will be used
  //
  MemNRrwActivateCmd (NBPtr, ChipSel, Rrw->TgtBankAddressA, Rrw->TgtRowAddressA);
  NBPtr->SetBitField (NBPtr, BFTgtChipSelectA, ChipSel);
  if (CmdTgt == CMD_TGT_AB) {
    MemNRrwActivateCmd (NBPtr, ChipSel, Rrw->TgtBankAddressB, Rrw->TgtRowAddressB);
    NBPtr->SetBitField (NBPtr, BFTgtChipSelectB, ChipSel);
  }
  //
  // Program the PRBS Seed
  //
  NBPtr->SetBitField (NBPtr, BFDataPrbsSeed, Rrw->DataPrbsSeed);
  //
  // Set the Command Count
  //
  NBPtr->SetBitField (NBPtr, BFCmdCount, ClCount);
  //
  // Program the Bubble Count and CmdStreamLen
  //
  NBPtr->SetBitField (NBPtr, BFBubbleCnt, 0);
  NBPtr->SetBitField (NBPtr, BFBubbleCnt2, 0);
  NBPtr->SetBitField (NBPtr, BFCmdStreamLen, 1);
  //
  // Program the Starting Address
  //
  NBPtr->SetBitField (NBPtr, BFTgtBankA, Rrw->TgtBankAddressA);
  NBPtr->SetBitField (NBPtr, BFTgtAddressA, Rrw->TgtColAddressA);
  if (CmdTgt == CMD_TGT_AB) {
    NBPtr->SetBitField (NBPtr, BFTgtBankB, Rrw->TgtBankAddressB);
    NBPtr->SetBitField (NBPtr, BFTgtAddressB, Rrw->TgtColAddressB);
  }
  //
  // Program the CmdTarget
  //
  NBPtr->SetBitField (NBPtr, BFCmdTgt, CmdTgt);
  //
  // Set CmdType to read
  //
  NBPtr->SetBitField (NBPtr, BFCmdType, CMD_TYPE_WRITE);
  //
  // Start the Commands
  //
  NBPtr->SetBitField (NBPtr, BFSendCmd, 1);
  //
  // Commands have started, wait for the writes to complete then clear the command
  //
  NBPtr->PollBitField (NBPtr, BFTestStatus, 1, PCI_ACCESS_TIMEOUT, FALSE);
  NBPtr->PollBitField (NBPtr, BFCmdSendInProg, 0, PCI_ACCESS_TIMEOUT, FALSE);
  NBPtr->SetBitField (NBPtr, BFSendCmd, 0);
  //
  // Send the Precharge All Command
  //
  MemNRrwPrechargeCmd (NBPtr, ChipSel, PRECHARGE_ALL_BANKS);
  //
  // Turn Off the RRW Engine
  //
  NBPtr->SetBitField (NBPtr, BFCmdTestEnable, 0);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *       This function checks the Error status bits for comparison results
 *
 *     @param[in,out]   *NBPtr    - Pointer to the MEM_NB_BLOCK
 *     @param[in]       Buffer[]  -  Not used in this implementation
 *     @param[in]       Pattern[] - Not used in this implementation
 *     @param[in]       ByteCount - Not used in this implementation
 *
 *     @return  PASS - Bitmap of results of comparison
 */

UINT16
STATIC
MemNCompareTestPatternUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT8 Pattern[],
  IN       UINT16 ByteCount
  )
{


  UINT16 i;
  UINT16 Pass;
  UINT8 ChipSel;
  UINT8 ColumnCount;
  UINT8* FailingBitMaskPtr;
  UINT8 FailingBitMask[9];
  UINT32 NibbleErrSts;

  ChipSel = NBPtr->TechPtr->ChipSel;
  ColumnCount = NBPtr->ChannelPtr->ColumnCount;
  // Calculate Failing Bitmask pointer
  FailingBitMaskPtr = &(NBPtr->ChannelPtr->FailingBitMask[(ColumnCount * NBPtr->TechPtr->ChipSel)]);

  //
  // Get Failing bit data
  //
  *((UINT32*)FailingBitMask) = NBPtr->GetBitField (NBPtr, BFDQErrLow);
  *((UINT32*)&FailingBitMask[4]) = NBPtr->GetBitField (NBPtr, BFDQErrHigh);
  FailingBitMask[8] = (UINT8)NBPtr->GetBitField (NBPtr, BFEccErr);

  Pass = 0x0000;
  //
  // Get Comparison Results - Convert Nibble Masks to Byte Masks
  //
  NibbleErrSts = NBPtr->GetBitField (NBPtr, BFNibbleErrSts);

  for (i = 0; i < ColumnCount ; i++) {
    Pass |= ((NibbleErrSts & 0x03) > 0 ) ? (1 << i) : 0;
    NibbleErrSts >>= 2;
    FailingBitMaskPtr[i] = FailingBitMask[i];
  }
  Pass = ~Pass;
  return Pass;
}

/*-----------------------------------------------------------------------------*/
/**
 *
 *       This function checks the Error status bits for offset comparison results
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
MemNInsDlyCompareTestPatternUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT8 Pattern[],
  IN       UINT16 ByteCount
  )
{
  UINT16 i;
  UINT16 Pass;
  UINT8 ColumnCount;
  UINT32 NibbleErr180Sts;

  ColumnCount = NBPtr->ChannelPtr->ColumnCount;
  Pass = 0x0000;
  //
  // Get Comparison Results - Convert Nibble Masks to Byte Masks
  //
  NibbleErr180Sts = NBPtr->GetBitField (NBPtr, BFNibbleErr180Sts);

  for (i = 0; i < ColumnCount ; i++) {
    Pass |= ((NibbleErr180Sts & 0x03) > 0 ) ? (1 << i) : 0;
    NibbleErr180Sts >>= 2;
  }
  Pass = ~Pass;

  return Pass;
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
MemNInitCPGUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  NBPtr->WritePattern = MemNContWritePatternUnb;
  NBPtr->ReadPattern = MemNContReadPatternUnb;
  NBPtr->GenHwRcvEnReads = MemNGenHwRcvEnReadsUnb;
  NBPtr->FlushPattern = (VOID (*) (MEM_NB_BLOCK *, UINT32, UINT16)) memDefRet;
  NBPtr->TrainingPatternInit = (AGESA_STATUS (*) (MEM_NB_BLOCK *)) memDefRetSuccess;
  NBPtr->TrainingPatternFinalize = (AGESA_STATUS (*) (MEM_NB_BLOCK *)) memDefRetSuccess;
  NBPtr->CompareTestPattern = MemNCompareTestPatternUnb;
  NBPtr->InsDlyCompareTestPattern = MemNInsDlyCompareTestPatternUnb;
  NBPtr->FamilySpecificHook[SetupHwTrainingEngine] = MemNSetupHwTrainingEngineUnb;
  NBPtr->CPGInit = 0;
}

