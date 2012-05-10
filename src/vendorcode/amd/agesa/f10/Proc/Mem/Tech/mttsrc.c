/**
 * @file
 *
 * mttsrc.c
 *
 * Technology Software based DQS receiver enable training
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Tech)
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
#include "AdvancedApi.h"
#include "Ids.h"
#include "mm.h"
#include "mn.h"
#include "mu.h"
#include "mt.h"
#include "GeneralServices.h"
#include "merrhdl.h"
#include "Filecode.h"
#define FILECODE PROC_MEM_TECH_MTTSRC_FILECODE
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

BOOLEAN
STATIC
MemTDqsTrainRcvrEnSw (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Pass
  );

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function executes first pass of receiver enable training for all dies
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */

BOOLEAN
MemTTrainRcvrEnSwPass1 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  return MemTDqsTrainRcvrEnSw (TechPtr, 1);
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function executes receiver enable training for a specific die
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]  Pass - Pass of the receiver training
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */
BOOLEAN
STATIC
MemTDqsTrainRcvrEnSw (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Pass
  )
{
  _16BYTE_ALIGN  UINT8  PatternBuffer[3 * 64];
  UINT8  TestBuffer[120];
  UINT8  *PatternBufPtr[4];
  UINT8  *TempPtr;
  UINT32 TestAddrRJ16[4];
  UINT32 TempAddrRJ16;
  UINT32 RealAddr;
  UINT16 CurTest[4];
  UINT8 Dct;
  UINT8 Receiver;
  UINT8 i;
  UINT8 TimesFail;
  UINT8 TimesRetrain;
  UINT16 RcvrEnDly;
  UINT16 MaxRcvrEnDly;
  UINT16 RcvrEnDlyLimit;
  UINT16 MaxDelayCha;
  BOOLEAN IsDualRank;
  BOOLEAN S0En;
  BOOLEAN S1En;
  UINT8 MaxFilterDly;

  MEM_DATA_STRUCT *MemPtr;
  DIE_STRUCT *MCTPtr;
  DCT_STRUCT *DCTPtr;
  MEM_NB_BLOCK  *NBPtr;

  NBPtr = TechPtr->NBPtr;
  MemPtr = NBPtr->MemPtr;
  MCTPtr = NBPtr->MCTPtr;

  TempAddrRJ16 = 0;
  TempPtr = NULL;
  MaxDelayCha = 0;
  MaxFilterDly = TechPtr->MaxFilterDly;
  RcvrEnDlyLimit = NBPtr->RcvrEnDlyLimit;
  TimesRetrain = DEFAULT_TRAINING_TIMES;
  IDS_OPTION_HOOK (IDS_MEM_RETRAIN_TIMES, &TimesRetrain, &MemPtr->StdHeader);

  IDS_HDT_CONSOLE ("!\nStart SW RxEn training\n");
  // Set environment settings before training
  MemTBeginTraining (TechPtr);

  PatternBufPtr[0] = PatternBufPtr[2] = PatternBuffer;
  MemUFillTrainPattern (TestPattern0, PatternBufPtr[0], 64);
  PatternBufPtr[1] = PatternBufPtr[3] = PatternBufPtr[0] + 128;
  MemUFillTrainPattern (TestPattern1, PatternBufPtr[1], 64);

  // Begin receiver enable training
  AGESA_TESTPOINT (TpProcMemReceiverEnableTraining, &(MemPtr->StdHeader));
  MaxRcvrEnDly = 0;
  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    IDS_HDT_CONSOLE ("!\tDct %d\n", Dct);
    NBPtr->SwitchDCT (NBPtr, Dct);
    DCTPtr = NBPtr->DCTPtr;

    // Set training bit
    NBPtr->SetBitField (NBPtr, BFDqsRcvEnTrain, 1);

    // Relax Max Latency before training
    NBPtr->SetMaxLatency (NBPtr, 0xFFFF);

    if (Pass == FIRST_PASS) {
      TechPtr->InitDQSPos4RcvrEn (TechPtr);
    }

    // there are four receiver pairs, loosely associated with chipselects.
    Receiver = DCTPtr->Timings.CsEnabled ? 0 : 8;
    for (; Receiver < 8; Receiver += 2) {
      TechPtr->DqsRcvEnSaved = 0;
      RcvrEnDly = RcvrEnDlyLimit;
      S0En = NBPtr->GetSysAddr (NBPtr, Receiver, &TestAddrRJ16[0]);
      S1En = NBPtr->GetSysAddr (NBPtr, Receiver + 1, &TestAddrRJ16[2]);
      if (S0En) {
        TestAddrRJ16[1] = TestAddrRJ16[0] + BIGPAGE_X8_RJ16;
      }
      if (S1En) {
        TestAddrRJ16[3] = TestAddrRJ16[2] + BIGPAGE_X8_RJ16;
      }
      if (S0En && S1En) {
        IsDualRank = TRUE;
      } else {
        IsDualRank = FALSE;
      }

      if (S0En || S1En) {
        IDS_HDT_CONSOLE ("!\t\tCS %d\n", Receiver);

        // Write the test patterns
        AGESA_TESTPOINT (TpProcMemRcvrWritePattern, &(MemPtr->StdHeader));
        IDS_HDT_CONSOLE ("\t\t\tWrite to addresses: ");
        for (i = (S0En ? 0 : 2); i < (S1En ? 4 : 2); i++) {
          RealAddr = MemUSetUpperFSbase (TestAddrRJ16[i], MemPtr);
          MemUWriteCachelines (RealAddr, PatternBufPtr[i], 1);
          IDS_HDT_CONSOLE (" %04lx0000 ", TestAddrRJ16[i]);
        }
        IDS_HDT_CONSOLE ("\n");

        // Initialize RcvrEnDly value and other DCT stored values
        //                MCTPtr->DqsRcvEnPass = Pass ? 0xFF : 0;

        // Sweep receiver enable delays
        AGESA_TESTPOINT (TpProcMemRcvrStartSweep, &(MemPtr->StdHeader));
        TimesFail = 0;
        ERROR_HANDLE_RETRAIN_BEGIN (TimesFail, TimesRetrain)
        {
          for (RcvrEnDly = 0; RcvrEnDly < RcvrEnDlyLimit; RcvrEnDly++) {
            AGESA_TESTPOINT (TpProcMemRcvrSetDelay, &(MemPtr->StdHeader));
            TechPtr->SetRcvrEnDly (TechPtr, Receiver, RcvrEnDly);
            IDS_HDT_CONSOLE ("\t\t\tDly %3x", RcvrEnDly);

            // Read and compare the first beat of data
            for (i = (S0En ? 0 : 2); i < (S1En ? 4 : 2); i++) {
              AGESA_TESTPOINT (TpProcMemRcvrReadPattern, &(MemPtr->StdHeader));
              RealAddr = MemUSetUpperFSbase (TestAddrRJ16[i], MemPtr);
              MemUReadCachelines (TestBuffer, RealAddr, 1);
              AGESA_TESTPOINT (TpProcMemRcvrTestPattern, &(MemPtr->StdHeader));
              CurTest[i] = TechPtr->Compare1ClPattern (TechPtr, TestBuffer, PatternBufPtr[i]);
              // Due to speculative execution during MemUReadCachelines, we must
              //  flush one more cache line than we read.
              MemUProcIOClFlush (TestAddrRJ16[i], 2, MemPtr);
              TechPtr->ResetDCTWrPtr (TechPtr, Receiver);

              //
              // Swap the test pointers such that even and odd steps alternate.
              //
              if ((i % 2) == 0) {
                TempPtr = PatternBufPtr[i];
                PatternBufPtr[i] = PatternBufPtr[i + 1];

                TempAddrRJ16 = TestAddrRJ16[i];
                TestAddrRJ16[i] = TestAddrRJ16[i + 1];
              } else {
                PatternBufPtr[i] = TempPtr;
                TestAddrRJ16[i] = TempAddrRJ16;
              }
            }

            if (TechPtr->SaveRcvrEnDly (TechPtr, Receiver, RcvrEnDly, S0En ? (CurTest[0] & CurTest[1]) : 0xFFFF, S1En ? (CurTest[2] & CurTest[3]) : 0xFFFF)) {
              // if all bytelanes pass
              if (MaxRcvrEnDly < (RcvrEnDly - MaxFilterDly)) {
                MaxRcvrEnDly = RcvrEnDly - MaxFilterDly;
              }
              break;
            }
          } // End of delay sweep
          ERROR_HANDLE_RETRAIN_END ((RcvrEnDly > (RcvrEnDlyLimit - 1)), TimesFail)
        }

        if (RcvrEnDly == RcvrEnDlyLimit) {
          // no passing window
          PutEventLog (AGESA_ERROR, MEM_ERROR_RCVR_EN_NO_PASSING_WINDOW_EQUAL_LIMIT, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
          SetMemError (AGESA_ERROR, MCTPtr);
        }

        if (RcvrEnDly > (RcvrEnDlyLimit - 1)) {
        // passing window too narrow, too far delayed
          PutEventLog (AGESA_ERROR, MEM_ERROR_RCVR_EN_VALUE_TOO_LARGE_LIMIT_LESS_ONE, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
          SetMemError (AGESA_ERROR, MCTPtr);
          DCTPtr->Timings.CsTrainFail |= DCTPtr->Timings.CsPresent & (UINT16) (3 << Receiver);
          MCTPtr->ChannelTrainFail |= (UINT32)1 << Dct;
          if (!NBPtr->MemPtr->ErrorHandling (MCTPtr, NBPtr->Dct, DCTPtr->Timings.CsTrainFail, &NBPtr->MemPtr->StdHeader)) {
            return FALSE;
          }
        }
      }

      TechPtr->LoadRcvrEnDly (TechPtr, Receiver);     // set final delays
    }   // End while Receiver < 8

    // Clear training bit when done
    NBPtr->SetBitField (NBPtr, BFDqsRcvEnTrain, 0);

    // Set Max Latency for both channels
    MaxRcvrEnDly += 0x20;  // @attention -
    IDS_HDT_CONSOLE ("\t\tMaxRcvrEnDly: %03x\n", MaxRcvrEnDly);
    if (MCTPtr->GangedMode) {
      if (Dct == 0) {
        MaxDelayCha = MaxRcvrEnDly;
      } else if (MaxRcvrEnDly > MaxDelayCha) {
        NBPtr->SwitchDCT (NBPtr, 0);
        NBPtr->SetMaxLatency (NBPtr, MaxRcvrEnDly);
      }
    } else {
      NBPtr->SetMaxLatency (NBPtr, MaxRcvrEnDly);
    }
    TechPtr->ResetDCTWrPtr (TechPtr, 6);
  }

  // Restore environment settings after training
  MemTEndTraining (TechPtr);
  IDS_HDT_CONSOLE ("End SW RxEn training\n\n");
  return (BOOLEAN) (MCTPtr->ErrCode < AGESA_FATAL);
}

