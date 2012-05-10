/**
 * @file
 *
 * mttml.c
 *
 * Technology Max Latency Training support
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
#include "Ids.h"
#include "mm.h"
#include "mn.h"
#include "mu.h"
#include "mt.h"
#include "merrhdl.h"
#include "GeneralServices.h"
#include "Filecode.h"
#define FILECODE PROC_MEM_TECH_MTTML_FILECODE
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
/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function trains Max latency for all dies
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */

BOOLEAN
MemTTrainMaxLatency (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  UINT32 TestAddrRJ16;
  UINT8 Dct;
  UINT8 ChipSel;
  _16BYTE_ALIGN  UINT8  PatternBuffer[6 * 64];
  UINT8  TestBuffer[6 * 64];
  UINT8  *PatternBufPtr;
  UINT16 MaxLatDly;
  UINT16 MaxLatLimit;
  UINT16 Margin;
  UINT16 CurTest;
  UINT8 _CL_;
  UINT8 TimesFail;
  UINT8 TimesRetrain;

  MEM_DATA_STRUCT *MemPtr;
  DIE_STRUCT *MCTPtr;
  MEM_NB_BLOCK  *NBPtr;

  NBPtr = TechPtr->NBPtr;
  MCTPtr = NBPtr->MCTPtr;
  MemPtr = NBPtr->MemPtr;
  TimesRetrain = DEFAULT_TRAINING_TIMES;
  IDS_OPTION_HOOK (IDS_MEM_RETRAIN_TIMES, &TimesRetrain, &MemPtr->StdHeader);

  IDS_HDT_CONSOLE ("!\nStart MaxRdLat training\n");
  // Set environment settings before training
  AGESA_TESTPOINT (TpProcMemMaxRdLatencyTraining, &(MemPtr->StdHeader));
  MemTBeginTraining (TechPtr);

  _CL_ = (MCTPtr->Status[Sb128bitmode]) ? 6 : 3;
  PatternBufPtr = PatternBuffer;
  MemUFillTrainPattern (TestPatternML, PatternBufPtr, _CL_ * 64);

  // Begin max latency training
  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    if (MCTPtr->Status[Sb128bitmode] && (Dct != 0)) {
      break;
    }

    IDS_HDT_CONSOLE ("!\tDct %d\n", Dct);
    NBPtr->SwitchDCT (NBPtr, Dct);

    if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
      if (TechPtr->FindMaxDlyForMaxRdLat (TechPtr, &ChipSel)) {

        if (NBPtr->GetSysAddr (NBPtr, ChipSel, &TestAddrRJ16)) {
          IDS_HDT_CONSOLE ("!\t\tCS %d\n", ChipSel);
          IDS_HDT_CONSOLE ("\t\t\tWrite to address: %04lx0000\n", TestAddrRJ16);

          // Write the test patterns
          AGESA_TESTPOINT (TpProcMemMaxRdLatWritePattern, &(MemPtr->StdHeader));
          NBPtr->WritePattern (NBPtr, TestAddrRJ16, PatternBufPtr, _CL_);

          // Sweep max latency delays
          NBPtr->getMaxLatParams (NBPtr, TechPtr->MaxDlyForMaxRdLat, &MaxLatDly, &MaxLatLimit, &Margin);
          AGESA_TESTPOINT (TpProcMemMaxRdLatStartSweep, &(MemPtr->StdHeader));

          TimesFail = 0;
          ERROR_HANDLE_RETRAIN_BEGIN (TimesFail, TimesRetrain)
          {
            for (; MaxLatDly < MaxLatLimit; MaxLatDly++) {
              NBPtr->SetBitField (NBPtr, BFMaxLatency, MaxLatDly);
              IDS_HDT_CONSOLE ("\t\t\tDly %3x", MaxLatDly);
              TechPtr->ResetDCTWrPtr (TechPtr, 6);

              AGESA_TESTPOINT (TpProcMemMaxRdLatReadPattern, &(MemPtr->StdHeader));
              NBPtr->ReadPattern (NBPtr, TestBuffer, TestAddrRJ16, _CL_);
              AGESA_TESTPOINT (TpProcMemMaxRdLatTestPattern, &(MemPtr->StdHeader));
              CurTest = NBPtr->CompareTestPattern (NBPtr, TestBuffer, PatternBufPtr, _CL_ * 64);
              NBPtr->FlushPattern (NBPtr, TestAddrRJ16, _CL_);

	      // Traditional training increments MaxLatDly until the test passes
	      // and uses it as left edge
              if (CurTest == 0xFFFF) {
		      IDS_HDT_CONSOLE ("  P");
		      IDS_HDT_CONSOLE ("\n");
                break;
	      } else {
		      MaxLatDly++;
              }
	      IDS_HDT_CONSOLE ("\n");
            }// End of delay sweep
            ERROR_HANDLE_RETRAIN_END ((MaxLatDly >= MaxLatLimit), TimesFail)
          }

          AGESA_TESTPOINT (TpProcMemMaxRdLatSetDelay, &(MemPtr->StdHeader));

          if (MaxLatDly >= MaxLatLimit) {
            PutEventLog (AGESA_ERROR, MEM_ERROR_MAX_LAT_NO_WINDOW, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
            SetMemError (AGESA_ERROR, MCTPtr);
            NBPtr->DCTPtr->Timings.CsTrainFail |= NBPtr->DCTPtr->Timings.CsPresent;
            MCTPtr->ChannelTrainFail |= (UINT32)1 << Dct;
            if (!NBPtr->MemPtr->ErrorHandling (MCTPtr, NBPtr->Dct, EXCLUDE_ALL_CHIPSEL, &NBPtr->MemPtr->StdHeader)) {
              return FALSE;
            }
          } else {
            // set final delays
            NBPtr->SetBitField (NBPtr, BFMaxLatency, MaxLatDly + Margin);
          }
        }
      }
    }
  }

  // Restore environment settings after training
  MemTEndTraining (TechPtr);
  IDS_HDT_CONSOLE ("End MaxRdLat training\n\n");
  return (BOOLEAN) (MCTPtr->ErrCode < AGESA_FATAL);
}
