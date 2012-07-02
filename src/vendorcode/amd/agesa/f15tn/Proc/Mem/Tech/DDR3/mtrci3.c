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
extern BUILD_OPT_CFG UserOptions;

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
  UINT8  PowerDownMode;
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
    // RC9 = 0xD except when partial powerdown mode is enabled and mix SR/DR or SR/QR configurations,
    // RC9 should be 0x9 for SR and and 0xD for DR or QR RDIMMs.
    PowerDownMode = (UINT8) UserOptions.CfgPowerDownMode;
    PowerDownMode = (!TechPtr->NBPtr->IsSupported[ChannelPDMode]) ? PowerDownMode : 0;
    IDS_OPTION_HOOK (IDS_POWERDOWN_MODE, &PowerDownMode, &(TechPtr->NBPtr->MemPtr->StdHeader));
    if ((PowerDownMode == 1) &&
        (DCTPtr->Timings.DimmSRPresent & ((UINT16) 1 << (ChipSel >> 1))) &&
        ((DCTPtr->Timings.DimmDrPresent != 0) || (DCTPtr->Timings.DimmQrPresent != 0))) {
      Data = 0x09;
    } else {
      Data = 0x0D;
    }
    break;
  case 11:
    Data = CONVERT_VDDIO_TO_ENCODED (TechPtr->RefPtr->DDR3Voltage);
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
      if (Speed == DDR800_FREQUENCY) {
        MemTSendCtlWord3 (TechPtr, 0x0A, 0);
      } else if (Speed == DDR1066_FREQUENCY) {
        MemTSendCtlWord3 (TechPtr, 0x0A, 1);
      } else if (Speed == DDR1333_FREQUENCY) {
        MemTSendCtlWord3 (TechPtr, 0x0A, 2);
      } else if (Speed == DDR1600_FREQUENCY) {
        MemTSendCtlWord3 (TechPtr, 0x0A, 3);
      } else if (Speed == DDR1866_FREQUENCY) {
        MemTSendCtlWord3 (TechPtr, 0x0A, 4);
      } else {
        ASSERT (FALSE);
      }
    }
  }
}

