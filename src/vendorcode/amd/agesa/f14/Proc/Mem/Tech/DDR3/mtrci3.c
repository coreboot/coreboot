/* $NoKeywords:$ */
/**
 * @file
 *
 * mtrci3.c
 *
 * Technology Control word initialization for DDR3
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Tech/DDR3)
 * @e \$Revision: 35136 $ @e \$Date: 2010-07-16 11:29:48 +0800 (Fri, 16 Jul 2010) $
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
#include "Ids.h"
#include "mm.h"
#include "mn.h"
#include "mu.h"
#include "mt.h"
#include "mt3.h"
#include "mtrci3.h"
#include "merrhdl.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_TECH_DDR3_MTRCI3_FILECODE
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
 *   This function sends control words
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

VOID
MemTDramControlRegInit3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  UINT8 ChipSel;
  UINT8 i;
  UINT8 RawCard;
  UINT8 Data;
  UINT16 CsPresent;

  MEM_DATA_STRUCT *MemPtr;
  MEM_NB_BLOCK  *NBPtr;

  NBPtr = TechPtr->NBPtr;
  MemPtr = NBPtr->MemPtr;
  CsPresent = NBPtr->DCTPtr->Timings.CsPresent;

  MemUWait10ns (800, MemPtr);   // wait 8us TACT must be changed to optimize to 8 MEM CLKs

  // Set EnDramInit to start DRAM initialization

  MemUWait10ns (600, MemPtr);   // wait 6us for PLL LOCK

  for (ChipSel = 0; ChipSel < MAX_CS_PER_CHANNEL; ChipSel += 2) {
    //
    // If chip select present
    //
    if ((CsPresent & ((UINT16)3 << ChipSel)) != 0) {
      NBPtr->SetBitField (NBPtr, BFMrsChipSel, ChipSel);

      // 2. Program F2x[1, 0]A8[CtrlWordCS]=bit mask for target chip selects.
      NBPtr->SetBitField (NBPtr, BFCtrlWordCS, 3 << (ChipSel & 0xFE));

      RawCard = NBPtr->ChannelPtr->RefRawCard[ChipSel >> 1];

      for (i = 0; i <= 15; i++) {
        // wait 8us for TMRD, must be changed to optimize to 8 MEM CLKs
        MemUWait10ns (800, MemPtr);
        if ((i != 6) && (i != 7)) {
          Data = MemTGetCtlWord3 (TechPtr, i, RawCard, ChipSel);
          MemTSendCtlWord3 (TechPtr, i, Data);
        }
      }
    }
  }
  MemUWait10ns (600, MemPtr);   // wait 6us for TSTAB
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function calculates the ControlRC value
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]     CtrlWordNum  -  control Word number.
 *     @param[in]     RawCard  -  Raw Card
 *     @param[in]     ChipSel  - Target Chip Select
 *     @return  Control Word value
 */

UINT8
MemTGetCtlWord3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 CtrlWordNum,
  IN       UINT8 RawCard,
  IN       UINT8 ChipSel
  )
{
  UINT8  Data;
  DCT_STRUCT *DCTPtr;
  CH_DEF_STRUCT *ChannelPtr;

  DCTPtr = TechPtr->NBPtr->DCTPtr;
  ChannelPtr = TechPtr->NBPtr->ChannelPtr;

  Data = 0;  //Default value for all control words is 0
  switch (CtrlWordNum) {
  case 0:
    Data = 0x02;  // DA4=1
    break;
  case 1:
    if (DCTPtr->Timings.DimmSRPresent & ((UINT16) 1 << (ChipSel >> 1))) {
      Data = 0x0C;  // if single rank, set DBA1 and DBA0
    }
    break;
  case 2:
    Data = ChannelPtr->CtrlWrd02[ChipSel >> 1];
    break;
  case 3:
    Data = ChannelPtr->CtrlWrd03[ChipSel >> 1];
    break;
  case 4:
    Data = ChannelPtr->CtrlWrd04[ChipSel >> 1];
    break;
  case 5:
    Data = ChannelPtr->CtrlWrd05[ChipSel >> 1];
    break;
  case 8:
    Data = ChannelPtr->CtrlWrd08[ChipSel >> 1];
    break;
  case 9:
    Data = 0x0D;
    break;
  default:;
  }

  return (Data & 0x0F);
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
MemTSendCtlWord3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 CmdNum,
  IN       UINT8 Value
  )
{
  MEM_NB_BLOCK  *NBPtr;

  NBPtr = TechPtr->NBPtr;

  // 1. Program MrsBank and MrsAddress.
  //    n = [BA2, A2, A1, A0].
  //    data = [BA1, BA0, A4, A3].
  //    Set all other bits in MrsAddress to zero.
  //
  NBPtr->SetBitField (NBPtr, BFMrsBank, ((CmdNum & 8) >> 1) | (Value >> 2));
  NBPtr->SetBitField (NBPtr, BFMrsAddress, ((Value & 3) << 3) | (CmdNum & 7));
  IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tCS%d RC%02d %04x\n", (MemNGetBitFieldNb (NBPtr, BFMrsChipSel) & 7), CmdNum, Value);

  // 2.Set SendCtrlWord=1
  NBPtr->SetBitField (NBPtr, BFSendCtrlWord, 1);
  // 3.Wait for BFSendCtrlWord=0
  NBPtr->PollBitField (NBPtr, BFSendCtrlWord, 0, PCI_ACCESS_TIMEOUT, FALSE);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function sends specific control words commands before frequency change for certain DRAM buffers.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

VOID
FreqChgCtrlWrd3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  UINT8 ChipSel;
  UINT16 Speed;
  UINT16 CsPresent;

  MEM_DATA_STRUCT *MemPtr;
  MEM_NB_BLOCK  *NBPtr;

  NBPtr = TechPtr->NBPtr;
  MemPtr = NBPtr->MemPtr;
  Speed = NBPtr->DCTPtr->Timings.Speed;
  CsPresent = NBPtr->DCTPtr->Timings.CsPresent;


  for (ChipSel = 0; ChipSel < MAX_CS_PER_CHANNEL; ChipSel += 2) {
    //
    // If chip select present.
    //
    if ((CsPresent & ((UINT16)3 << ChipSel)) != 0) {

      NBPtr->SetBitField (NBPtr, BFMrsChipSel, ChipSel);
      // program F2x[1, 0]A8[CtrlWordCS]=bit mask for target chip selects.
      NBPtr->SetBitField (NBPtr, BFCtrlWordCS, 3 << (ChipSel & 0xFE));

      //wait 8us for TMRD, must be changed to optimize to 8 MEM CLKs
      MemUWait10ns (800, MemPtr);
      if (Speed == DDR1066_FREQUENCY) {
        MemTSendCtlWord3 (TechPtr, 0x0A, 1);
      } else if (Speed == DDR1333_FREQUENCY) {
        MemTSendCtlWord3 (TechPtr, 0x0A, 2);
      } else if (Speed == DDR1600_FREQUENCY) {
        MemTSendCtlWord3 (TechPtr, 0x0A, 3);
      }
    }
  }
}

