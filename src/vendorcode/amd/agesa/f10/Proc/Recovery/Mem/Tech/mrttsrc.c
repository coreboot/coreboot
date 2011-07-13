/**
 * @file
 *
 * mrttsrc.c
 *
 * Technology Software based DQS receiver enable training Recovery
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Proc/Recovery/Mem)
 * @e \$Revision: 44323 $ @e \$Date: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
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
#include "amdlib.h"
#include "Ids.h"
#include "mrport.h"
#include "mm.h"
#include "mn.h"
#include "mru.h"
#include "mt.h"
#include "Filecode.h"
#define FILECODE PROC_RECOVERY_MEM_TECH_MRTTSRC_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define MAX_BYTELANES 8             /* Max Bytelanes per channel */

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
MemRecTSetRcvrEnDly (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Receiver,
  IN       UINT16 RcvEnDly
  );

VOID
STATIC
MemRecTLoadRcvrEnDly (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Receiver
  );

BOOLEAN
STATIC
MemRecTSaveRcvrEnDly (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Receiver,
  IN       UINT16 RcvEnDly,
  IN       UINT8 CmpResult
  );

UINT8
STATIC
MemRecTCompare1ClPattern (
  IN       UINT8 Buffer[],
  IN       UINT8 Pattern[],
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

VOID
STATIC
MemRecTBeginTraining (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

VOID
STATIC
MemRecTEndTraining (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function executes receiver enable training for BSP
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

VOID
MemRecTTrainRcvrEnSw (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  _16BYTE_ALIGN  UINT8  PatternBuffer[3 * 64];
  UINT8  TestBuffer[120];
  UINT8  *PatternBufPtr[2];
  UINT32 TestAddr[4];
  UINT8  TestResult;
  UINT8 Receiver;
  UINT8 i;
  UINT8 j;
  UINT16 RcvrEnDly;

  MEM_DATA_STRUCT *MemPtr;
  DIE_STRUCT *MCTPtr;
  MEM_NB_BLOCK  *NBPtr;

  NBPtr = TechPtr->NBPtr;
  MemPtr = NBPtr->MemPtr;
  MCTPtr = NBPtr->MCTPtr;

  AGESA_TESTPOINT (TpProcMemReceiverEnableTraining, &(MemPtr->StdHeader));

  // Set environment settings before training
  MemRecTBeginTraining (TechPtr);

  PatternBufPtr[0] = PatternBuffer;
  MemRecUFillTrainPattern (TestPattern0, PatternBufPtr[0], 64, &(MemPtr->StdHeader));
  PatternBufPtr[1] = PatternBufPtr[0] + 128;
  MemRecUFillTrainPattern (TestPattern1, PatternBufPtr[1], 64, &(MemPtr->StdHeader));

  // Begin receiver enable training
  MemRecTSetWrDatRdDqs (TechPtr, 0);

  // there are four receiver pairs, loosely associated with chipselects.
  Receiver = NBPtr->DimmToBeUsed << 1;
  TechPtr->DqsRcvEnSaved = 0;

  TestAddr[0] = NBPtr->GetSysAddrRec (NBPtr);
  TestAddr[1] = TestAddr[0] + BIGPAGE_X8;

  IDS_HDT_CONSOLE ("\tDct %d\n", NBPtr->Dct);
  IDS_HDT_CONSOLE ("\t\tCS %d\n", Receiver);
  IDS_HDT_CONSOLE ("\t\t\tWrite to addresses:  %04x0000\n", TestAddr[0]);

  // Sweep receiver enable delays
  AGESA_TESTPOINT (TpProcMemRcvrStartSweep, &(MemPtr->StdHeader));
  for (RcvrEnDly = 0; RcvrEnDly < 0xFF; RcvrEnDly++) {

    TestResult = 0xFF;
    for (i = 0; i < 2; i++) {

      // Set RcvrEn delay for all byte lanes
      AGESA_TESTPOINT (TpProcMemRcvrSetDelay, &(MemPtr->StdHeader));
      MemRecTSetRcvrEnDly (TechPtr, Receiver, RcvrEnDly);

      // Swap the test pointers such that even and odd steps alternate.
      j = ((RcvrEnDly & 1) != 0) ? (i ^ 1) : i;

      // Write, read and compare the first beat of data
      AGESA_TESTPOINT (TpProcMemRcvrWritePattern, &(MemPtr->StdHeader));
      MemRecUWrite1CL (TestAddr[j], PatternBufPtr[j]);
      AGESA_TESTPOINT (TpProcMemRcvrReadPattern, &(MemPtr->StdHeader));
      MemRecURead1CL (TestBuffer, TestAddr[j]);
      AGESA_TESTPOINT (TpProcMemRcvrTestPattern, &(MemPtr->StdHeader));
      TestResult &= MemRecTCompare1ClPattern (TestBuffer, PatternBufPtr[j], &(MemPtr->StdHeader));
      MemRecUProcIOClFlush (TestAddr[j], MemPtr);
    }
    IDS_HDT_CONSOLE ("%v0");
    IDS_HDT_CONSOLE ("\t\t\tDly %3x%vh1\n", RcvrEnDly);
    IDS_HDT_CONSOLE ("\t\t\t       %vh2\n");
    IDS_HDT_CONSOLE ("\t\t\t       %vh3\n\n");

    if (MemRecTSaveRcvrEnDly (TechPtr, Receiver, RcvrEnDly, TestResult)) {
      // if all bytelanes pass
      break;
    }
  }   // End of delay sweep

  if (RcvrEnDly == 0xFF) {
    // no passing window
    // LibAmdEventLog (AGESA_ERROR, MEM_ERROR_RCVR_EN_NO_PASSING_WINDOW, 0, NBPtr->Dct, NBPtr->Channel, 0);   //@attention commented out since it is not defined in recovery code
    SetMemRecError (AGESA_ERROR, MCTPtr);
  }

  // set final delays
  MemRecTLoadRcvrEnDly (TechPtr, Receiver);

  // Clear training bit when done
  NBPtr->SetBitField (NBPtr, BFDqsRcvEnTrain, 0);

  IDS_HDT_CONSOLE ("\t\tMaxRcvrEnDly: %03x\n", RcvrEnDly + 0x20);

  // Set Max Latency for both channels
  NBPtr->SetMaxLatency (NBPtr, RcvrEnDly + 0x20);

  // Restore environment settings after training
  MemRecTEndTraining (TechPtr);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      If WrDatDly is 0, this function sets the DQS Positions in preparation
 *      for Receiver Enable Training. (Write Position is no delay, Read Position is 1.5 Memclock delay).
 *      Otherwise it will set WrDat and RdDqs to center of data eye.
 *
 *     @param[in,out]   *TechPtr - Pointer to the MEM_TECH_BLOCK
 *     @param[in]       WrDatDly - either 0 or 0x0F
 *
 */

VOID
MemRecTSetWrDatRdDqs (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 WrDatDly
  )
{
  UINT8 ByteLane;
  UINT8 Dimm;
  UINT8 WrDqs;
  UINT8 RdDqs;
  MEM_NB_BLOCK  *NBPtr;

  NBPtr = TechPtr->NBPtr;

  Dimm = NBPtr->DimmToBeUsed;
  for (ByteLane = 0; ByteLane < 8; ByteLane++) {
    WrDqs = NBPtr->ChannelPtr->WrDqsDlys[(Dimm * MAX_BYTELANES) + ByteLane];
    NBPtr->SetTrainDly (NBPtr, AccessWrDatDly, DIMM_BYTE_ACCESS (Dimm, ByteLane), WrDqs + WrDatDly);
    RdDqs = (WrDatDly == 0) ? 0x2F : 0x0F;
    NBPtr->SetTrainDly (NBPtr, AccessRdDqsDly, DIMM_BYTE_ACCESS (Dimm, ByteLane), RdDqs);
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  This function programs DqsRcvEnDly to additional index for DQS receiver enabled training
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]       Receiver  - Current Chip select value
 *     @param[in]       RcvEnDly  - receiver enable delay to be saved
 *
 */

VOID
STATIC
MemRecTSetRcvrEnDly (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Receiver,
  IN       UINT16 RcvEnDly
  )
{
  UINT8 ByteLane;

  ASSERT (Receiver <= MAX_CS_PER_CHANNEL);
  for (ByteLane = 0; ByteLane < 8; ByteLane++) {
    TechPtr->NBPtr->SetTrainDly (TechPtr->NBPtr, AccessRcvEnDly, DIMM_BYTE_ACCESS (Receiver >> 1, ByteLane), RcvEnDly);
  }
}


/* -----------------------------------------------------------------------------*/
/**
 *
 *       This function compares test pattern with data in buffer and return a pass/fail bitmap
 *       for 8 Bytes
 *
 *     @param[in]       Buffer[]  -  Buffer data from DRAM (Measured data from DRAM) to compare
 *     @param[in]       Pattern[]  - Pattern (Expected data in ROM/CACHE) to compare against
 *     @param[in,out] StdHeader - The Pointer of AGESA Header.
 *
 *     @return  PASS - Bit map of results of comparison
 */

UINT8
STATIC
MemRecTCompare1ClPattern (
  IN       UINT8 Buffer[],
  IN       UINT8 Pattern[],
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8 i;
  UINT8 Pass;

  Pass = 0xFF;
  IDS_HDT_CONSOLE ("%v1 -");
  IDS_HDT_CONSOLE ("%v2 -");
  IDS_HDT_CONSOLE ("%v3 -");
  for (i = 0; i < 8; i++) {
    if (Buffer[i] != Pattern[i]) {
      // if bytelane n fails
      Pass &= ~((UINT16) 1 << (i % 8));    // clear bit n
    }
    IDS_HDT_CONSOLE ("%v1  %c", (Buffer[i] == Pattern[i]) ? 'P' : '.');
    IDS_HDT_CONSOLE ("%v2 %02x", Buffer[i]);
    IDS_HDT_CONSOLE ("%v3 %02x", Pattern[i]);
  }
  return Pass;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  This function saves passing DqsRcvEnDly values to the stack
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]       Receiver  - Current Chip select value
 *     @param[in]       RcvEnDly  - receiver enable delay to be saved
 *     @param[in]       CmpResult - compare result for Rank 0
 *
 *     @return  TRUE - All bytelanes pass
 *     @return  FALSE - Some bytelanes fail
 */

BOOLEAN
STATIC
MemRecTSaveRcvrEnDly (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Receiver,
  IN       UINT16 RcvEnDly,
  IN       UINT8 CmpResult
  )
{
  UINT8 i;
  UINT8 Passed;
  UINT8 Saved;
  UINT8 Mask;
  UINT8 Dimm;

  ASSERT (Receiver <= MAX_CS_PER_CHANNEL);

  Passed = CmpResult;
  Saved = (UINT8) (TechPtr->DqsRcvEnSaved & Passed); //@todo - false passes filter (subject to be replaced with a better solution)
  Dimm = Receiver >> 1;
  Mask = 1;
  for (i = 0; i < 8; i++) {
    if ((Passed & Mask) != 0) {
      if ((Saved & Mask) == 0) {
        TechPtr->NBPtr->ChannelPtr->RcvEnDlys[Dimm * MAX_BYTELANES + i] = (UINT8) (RcvEnDly + 0x20);
        IDS_HDT_CONSOLE ("\t\t\tBL %d = %02x", i, RcvEnDly + 0x20);
      }
      Saved |= Mask;
    }
    Mask <<= 1;
  }
  TechPtr->DqsRcvEnSaved = Saved;

  if (Saved == 0xFF) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  This function loads the DqsRcvEnDly from saved data and program to additional index
 *  for DQS receiver enabled training
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]       Receiver  - Current Chip select value
 *
 */

VOID
STATIC
MemRecTLoadRcvrEnDly (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Receiver
  )
{
  UINT8 i;
  UINT8 Dimm;
  UINT16 Saved;
  CH_DEF_STRUCT *ChannelPtr;

  ASSERT (Receiver <= MAX_CS_PER_CHANNEL);
  ChannelPtr = TechPtr->NBPtr->ChannelPtr;

  Dimm = Receiver >> 1;
  Saved = TechPtr->DqsRcvEnSaved;
  for (i = 0; i < 8; i++) {
    if ((Saved & 1) != 0) {
      TechPtr->NBPtr->SetTrainDly (TechPtr->NBPtr, AccessRcvEnDly, DIMM_BYTE_ACCESS (Receiver >> 1, i),
      ChannelPtr->RcvEnDlys[Dimm * MAX_BYTELANES + i]);
    }
    Saved >>= 1;
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function sets the initial controller environment before training.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

VOID
STATIC
MemRecTBeginTraining (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  S_UINT64 SMsr;
  MEM_DATA_STRUCT *MemPtr;
  MEM_NB_BLOCK  *NBPtr;

  NBPtr = TechPtr->NBPtr;
  MemPtr = NBPtr->MemPtr;

  LibAmdReadCpuReg (CR4_REG, &TechPtr->CR4reg);
  LibAmdWriteCpuReg (CR4_REG, TechPtr->CR4reg | ((UINT32) 1 << 9)); // enable SSE2

  LibAmdMsrRead (HWCR, (UINT64 *) (&SMsr), &MemPtr->StdHeader);            // HWCR
  TechPtr->HwcrLo = SMsr.lo;
  SMsr.lo |= 0x00020000;                  // turn on HWCR.wrap32dis
  SMsr.lo &= 0xFFFF7FFF;                  // turn off HWCR.SSEDIS
  LibAmdMsrWrite (HWCR, (UINT64 *) (&SMsr), &MemPtr->StdHeader);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function sets the final controller environment after training.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

VOID
STATIC
MemRecTEndTraining (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  S_UINT64 SMsr;
  MEM_DATA_STRUCT *MemPtr;
  MEM_NB_BLOCK  *NBPtr;

  NBPtr = TechPtr->NBPtr;
  MemPtr = NBPtr->MemPtr;

  LibAmdWriteCpuReg (CR4_REG, TechPtr->CR4reg);

  LibAmdMsrRead (HWCR, (UINT64 *)&SMsr, &MemPtr->StdHeader);
  SMsr.lo = TechPtr->HwcrLo;
  LibAmdMsrWrite (HWCR, (UINT64 *)&SMsr, &MemPtr->StdHeader);
}
