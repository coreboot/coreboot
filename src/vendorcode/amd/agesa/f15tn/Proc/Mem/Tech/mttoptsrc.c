/* $NoKeywords:$ */
/**
 * @file
 *
 * mttoptsrc.c
 *
 * New Technology Software based DQS receiver enable training
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Tech)
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
#include "Ids.h"
#include "mm.h"
#include "mn.h"
#include "mu.h"
#include "mt.h"
#include "merrhdl.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_TECH_MTTOPTSRC_FILECODE
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
MemTDqsTrainOptRcvrEnSw (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Pass
  );

BOOLEAN
MemTNewRevTrainingSupport (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  return TRUE;
}

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
MemTTrainOptRcvrEnSwPass1 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  return MemTDqsTrainOptRcvrEnSw (TechPtr, 1);
}

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
MemTDqsTrainOptRcvrEnSw (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Pass
  )
{
  _16BYTE_ALIGN  UINT8  PatternBuffer[6 * 64];
  UINT8  TestBuffer[256];
  UINT8  *PatternBufPtr[6];
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


  MEM_DATA_STRUCT *MemPtr;
  DIE_STRUCT *MCTPtr;
  DCT_STRUCT *DCTPtr;
  MEM_NB_BLOCK  *NBPtr;

  NBPtr = TechPtr->NBPtr;
  MemPtr = NBPtr->MemPtr;
  MCTPtr = NBPtr->MCTPtr;
  TechPtr->TrainingType = TRN_RCVR_ENABLE;


  TempAddrRJ16 = 0;
  TempPtr = NULL;
  MaxDelayCha = 0;
  TimesRetrain = DEFAULT_TRAINING_TIMES;
  IDS_OPTION_HOOK (IDS_MEM_RETRAIN_TIMES, &TimesRetrain, &MemPtr->StdHeader);

  IDS_HDT_CONSOLE (MEM_STATUS, "\nStart Optimized SW RxEn training\n");
  // Set environment settings before training
  MemTBeginTraining (TechPtr);

  PatternBufPtr[0] = PatternBufPtr[2] = PatternBuffer;
  // These two patterns used for first Test Address
  MemUFillTrainPattern (TestPattern0, PatternBufPtr[0], 64);
  // Second Cacheline used for Dummy Read is the inverse of
  //  the first so that is is not mistaken for the real read
  MemUFillTrainPattern (TestPattern1, PatternBufPtr[0] + 64, 64);
  PatternBufPtr[1] = PatternBufPtr[3] = PatternBufPtr[0] + 128;
  // These two patterns used for second Test Address
  MemUFillTrainPattern (TestPattern1, PatternBufPtr[1], 64);
  // Second Cacheline used for Dummy Read is the inverse of
  //  the first so that is is not mistaken for the real read
  MemUFillTrainPattern (TestPattern0, PatternBufPtr[1] + 64, 64);

  // Fill pattern for flush after every sweep
  PatternBufPtr[4] = PatternBufPtr[0] + 256;
  MemUFillTrainPattern (TestPattern3, PatternBufPtr[4], 64);

  // Fill pattern for initial dummy read
  PatternBufPtr[5] = PatternBufPtr[0] + 320;
  MemUFillTrainPattern (TestPattern4, PatternBufPtr[5], 64);


  // Begin receiver enable training
  AGESA_TESTPOINT (TpProcMemReceiverEnableTraining, &(MemPtr->StdHeader));
  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    IDS_HDT_CONSOLE (MEM_STATUS, "\tDct %d\n", Dct);
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
        IDS_HDT_CONSOLE (MEM_STATUS, "\t\tCS %d\n", Receiver);

        RcvrEnDlyLimit = 0x1FF;      // @attention - limit depends on proc type
        TechPtr->DqsRcvEnSaved = 0;
        RcvrEnDly = RcvrEnDlyLimit;
        RealAddr = 0;

        TechPtr->GetFirstPassVal = FALSE;
        TechPtr->DqsRcvEnFirstPassVal = 0;
        TechPtr->RevertPassVal = FALSE;
        TechPtr->InitializeVariablesOpt (TechPtr);

        // Write the test patterns
        AGESA_TESTPOINT (TpProcMemRcvrWritePattern, &(MemPtr->StdHeader));
        IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tWrite to addresses: ");
        for (i = (S0En ? 0 : 2); i < (S1En ? 4 : 2); i++) {
          RealAddr = MemUSetUpperFSbase (TestAddrRJ16[i], MemPtr);
          // One cacheline of data to be tested and one of dummy data
          MemUWriteCachelines (RealAddr, PatternBufPtr[i], 2);
          // This is dummy data with a different pattern used for the first dummy read.
          MemUWriteCachelines (RealAddr + 128, PatternBufPtr[5], 1);
          IDS_HDT_CONSOLE (MEM_FLOW, " %04x0000 ", TestAddrRJ16[i]);
        }
        IDS_HDT_CONSOLE (MEM_FLOW, "\n");

        // Sweep receiver enable delays
        AGESA_TESTPOINT (TpProcMemRcvrStartSweep, &(MemPtr->StdHeader));
        TimesFail = 0;
        ERROR_HANDLE_RETRAIN_BEGIN (TimesFail, TimesRetrain)
        {
          TechPtr->LoadInitialRcvrEnDlyOpt (TechPtr, Receiver);
          while (!TechPtr->CheckRcvrEnDlyLimitOpt (TechPtr)) {
            AGESA_TESTPOINT (TpProcMemRcvrSetDelay, &(MemPtr->StdHeader));
            TechPtr->SetRcvrEnDlyOpt (TechPtr, Receiver, RcvrEnDly);
            // Read and compare the first beat of data
            for (i = (S0En ? 0 : 2); i < (S1En ? 4 : 2); i++) {
              AGESA_TESTPOINT (TpProcMemRcvrReadPattern, &(MemPtr->StdHeader));
              RealAddr = MemUSetUpperFSbase (TestAddrRJ16[i], MemPtr);
              //
              // Issue dummy cacheline reads
              //
              MemUReadCachelines (TestBuffer + 128, RealAddr + 128, 1);
              MemUReadCachelines (TestBuffer, RealAddr, 1);
              MemUProcIOClFlush (TestAddrRJ16[i], 2, MemPtr);
              //
              // Perform actual read which will be compared
              //
              MemUReadCachelines (TestBuffer + 64, RealAddr + 64, 1);
              AGESA_TESTPOINT (TpProcMemRcvrTestPattern, &(MemPtr->StdHeader));
              CurTest[i] = TechPtr->Compare1ClPatternOpt (TechPtr, TestBuffer + 64 , PatternBufPtr[i] + 64, i, Receiver, S1En);
              // Due to speculative execution during MemUReadCachelines, we must
              //  flush one more cache line than we read.
              MemUProcIOClFlush (TestAddrRJ16[i], 4, MemPtr);
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
          }   // End of delay sweep
          ERROR_HANDLE_RETRAIN_END (!TechPtr->SetSweepErrorOpt (TechPtr, Receiver, Dct, TRUE), TimesFail)
        }

        if (!TechPtr->SetSweepErrorOpt (TechPtr, Receiver, Dct, FALSE)) {
          return FALSE;
        }

        TechPtr->LoadRcvrEnDlyOpt (TechPtr, Receiver);     // set final delays
        //
        // Flush AA and 55 patterns by reading a dummy pattern to fill in FIFO
        //
        // Aquire a new FSBase, based on the last test address that we stored.
        RealAddr = MemUSetUpperFSbase (TempAddrRJ16, MemPtr);
        ASSERT (RealAddr != 0);
        MemUWriteCachelines (RealAddr, PatternBufPtr[4], 1);
        MemUWriteCachelines (RealAddr + 64, PatternBufPtr[4], 1);
        MemUReadCachelines (TestBuffer, RealAddr, 2);
        // Due to speculative execution during MemUReadCachelines, we must
        //  flush one more cache line than we read.
        MemUProcIOClFlush (TempAddrRJ16, 3, MemPtr);
      }
    }   // End while Receiver < 8

    // Clear training bit when done
    NBPtr->SetBitField (NBPtr, BFDqsRcvEnTrain, 0);

    // Set Max Latency for both channels
    MaxRcvrEnDly = TechPtr->GetMaxValueOpt (TechPtr);
    IDS_HDT_CONSOLE (MEM_FLOW, "\t\tMaxRcvrEnDly: %03x\n", MaxRcvrEnDly);
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
  IDS_HDT_CONSOLE (MEM_FLOW, "End Optimized SW RxEn training\n\n");
  return (BOOLEAN) (MCTPtr->ErrCode < AGESA_FATAL);
}

/*-----------------------------------------------------------------------------
 *
 *  This function saves passing DqsRcvEnDly values to the stack
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]       Receiver  - Current Chip select value
 *     @param[in]       RcvEnDly  - receiver enable delay to be saved
 *     @param[in]       cmpResultRank0 - compare result for Rank 0
 *     @param[in]       cmpResultRank0 - compare result for Rank 1
 *
 *     @retval  TRUE - All bytelanes pass
 *              FALSE - Some bytelanes fail
 * ----------------------------------------------------------------------------
 */

BOOLEAN
MemTSaveRcvrEnDlyByteFilterOpt (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Receiver,
  IN       UINT16 RcvEnDly,
  IN       UINT16 CmpResultRank0,
  IN       UINT16 CmpResultRank1
  )
{
  UINT8 i;
  UINT8 Passed;
  UINT8 Dimm;
  CH_DEF_STRUCT *ChannelPtr;

  ASSERT (Receiver < MAX_CS_PER_CHANNEL);
  ChannelPtr = TechPtr->NBPtr->ChannelPtr;

  Passed = (UINT8) ((CmpResultRank0 & CmpResultRank1) & 0xFF);

  Dimm = Receiver >> 1;

  if (TechPtr->GetFirstPassVal && (RcvEnDly - TechPtr->DqsRcvEnFirstPassVal) >= 0x30) {
    for (i = 0; i < 8; i++) {
      ChannelPtr->RcvEnDlys[Dimm * TechPtr->DlyTableWidth () + i] = TechPtr->DqsRcvEnFirstPassVal + NEW_RECEIVER_FINAL_OFFSETVALUE;
    }
    TechPtr->DqsRcvEnSaved = 0xFF;
  }

  if (Passed == 0xFF) {
    if (!TechPtr->GetFirstPassVal) {
      TechPtr->DqsRcvEnFirstPassVal = RcvEnDly;
      TechPtr->GetFirstPassVal = TRUE;
    }
    return TRUE;
  } else {
    TechPtr->DqsRcvEnFirstPassVal = 0;

    // We have got first passing value, but later, we meet with glitch
    if (TechPtr->GetFirstPassVal) {
      TechPtr->DqsRcvEnFirstPassVal = 0xFF;
      TechPtr->GetFirstPassVal = FALSE;
    }
    return FALSE;
  }
}
