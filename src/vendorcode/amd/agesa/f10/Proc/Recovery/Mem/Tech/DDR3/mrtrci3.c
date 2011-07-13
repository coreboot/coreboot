/**
 * @file
 *
 * mrtrci3.c
 *
 * Technology Control word initialization for DDR3 Recovery
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
#include "Ids.h"
#include "mm.h"
#include "mn.h"
#include "mru.h"
#include "mt.h"
#include "Filecode.h"
#define FILECODE PROC_RECOVERY_MEM_TECH_DDR3_MRTRCI3_FILECODE
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

UINT8
STATIC
MemRecTGetCtlWord3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 CtrlWordNum
  );

VOID
STATIC
MemRecTSendCtlWord3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 CmdNum,
  IN       UINT8 Value
  );

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function sends control words
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  pDCT->Timings.TrwtTO updated
 */

VOID
MemRecTDramControlRegInit3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  UINT8 RCNum;
  UINT8 Data;

  MEM_DATA_STRUCT *MemPtr;
  MEM_NB_BLOCK  *NBPtr;

  NBPtr = TechPtr->NBPtr;
  MemPtr = NBPtr->MemPtr;

  // wait 8us TACT must be changed to optimize to 8 MEM CLKs
  // and wait 6us for PLL LOCK
  MemRecUWait10ns (80 + 60, MemPtr);

  // 2. Program F2x[1, 0]A8[CtrlWordCS]=bit mask for target chip selects.
  NBPtr->SetBitField (NBPtr, BFCtrlWordCS, 3 << (NBPtr->DimmToBeUsed << 1));

  for (RCNum = 0; RCNum <= 15; RCNum++) {
    // wait 8us for TMRD, must be changed to optimize to 8 MEM CLKs
    MemRecUWait10ns (80, MemPtr);

    if ((RCNum != 6) && (RCNum != 7)) {
      Data = MemRecTGetCtlWord3 (TechPtr, RCNum);
      MemRecTSendCtlWord3 (TechPtr, RCNum, Data);
    }
  }

  MemRecUWait10ns (60, MemPtr);   // wait 6us for TSTAB
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function calculates the ControlRC value
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]     CtrlWordNum  -  control Word number.
 *
 *     @return  Control Word value
 */

UINT8
STATIC
MemRecTGetCtlWord3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 CtrlWordNum
  )
{
  UINT8  Data;

  CH_DEF_STRUCT *ChannelPtr;

  ChannelPtr = TechPtr->NBPtr->ChannelPtr;

  Data = 0;  //Default value for all control words is 0
  switch (CtrlWordNum) {
  case 0:
    Data = 0x02;  // DA4=1
    break;
  case 1:
    Data = 0x0C;  // if single rank, set DBA1 and DBA0
    break;
  case 3:
    Data = ChannelPtr->CtrlWrd03[TechPtr->NBPtr->DimmToBeUsed];
    break;
  case 4:
    Data = ChannelPtr->CtrlWrd04[TechPtr->NBPtr->DimmToBeUsed];
    break;
  case 5:
    Data = ChannelPtr->CtrlWrd05[TechPtr->NBPtr->DimmToBeUsed];
    break;
  case 9:
    Data = 0x0D;
    break;
  default:;
  }

  return (Data&0x0F);
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function sends control word command
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]     CmdNum  -  control number.
 *     @param[in]     Value  -  value to send
 *
 */

VOID
STATIC
MemRecTSendCtlWord3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 CmdNum,
  IN       UINT8 Value
  )
{
  MEM_NB_BLOCK  *NBPtr;

  ASSERT (CmdNum < 16);
  ASSERT (Value < 16);

  NBPtr = TechPtr->NBPtr;

  // 1. Program MrsBank and MrsAddress.
  //    n = [BA2, A2, A1, A0].
  //    data = [BA1, BA0, A4, A3].
  //    Set all other bits in MrsAddress to zero.
  //
  NBPtr->SetBitField (NBPtr, BFMrsBank, ((CmdNum&8) >> 1) | (Value >> 2));
  NBPtr->SetBitField (NBPtr, BFMrsAddress, ((Value&3) << 3) | (CmdNum&7));
  IDS_HDT_CONSOLE ("\t\tCS%d RC%02d %04x\n",
              (NBPtr->GetBitField (NBPtr, BFDramInitRegReg) >> 20) & 0xF,
              ((NBPtr->GetBitField (NBPtr, BFDramInitRegReg) >> 15) & 8) |
              (NBPtr->GetBitField (NBPtr, BFDramInitRegReg) & 7),
              ((NBPtr->GetBitField (NBPtr, BFDramInitRegReg) >> 14) & 0xC) |
              ((NBPtr->GetBitField (NBPtr, BFDramInitRegReg) >> 3) & 3));

  // 2.Set SendCtrlWord=1
  NBPtr->SetBitField (NBPtr, BFSendCtrlWord, 1);
  // 3.Wait for BFSendCtrlWord=0
  while (NBPtr->GetBitField (NBPtr, BFSendCtrlWord) != 0) {}
}
