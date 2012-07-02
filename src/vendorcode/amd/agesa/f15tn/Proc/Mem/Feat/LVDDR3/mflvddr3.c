/* $NoKeywords:$ */
/**
 * @file
 *
 * lvddr3.c
 *
 * Voltage change for DDR3 DIMMs.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Feat/LVDDR3)
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
#include "amdlib.h"
#include "Ids.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_MEM_FEAT_LVDDR3_MFLVDDR3_FILECODE
/* features */
#include "mflvddr3.h"


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

/*-----------------------------------------------------------------------------*/
/**
 *
 *   This function calculate the common lowest voltage supported by all DDR3
 *   DIMMs in the system. This function only needs to be called on BSP.
 *
 *   @param[in, out]    *NBPtr - Pointer to NB block
 *
 *     @return          TRUE -  This feature is enabled.
 *     @return          FALSE - This feature is not enabled.
 */

BOOLEAN
MemFLvDdr3 (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  CH_DEF_STRUCT *ChannelPtr;
  MEM_TECH_BLOCK  *TechPtr;
  MEM_SHARED_DATA *mmSharedPtr;
  UINT8 Dct;
  UINT8 Channel;
  UINT8 Dimm;
  UINT8 *SpdBufferPtr;
  UINT8 VDDByte;
  UINT8 VoltageMap;

  mmSharedPtr = NBPtr->SharedPtr;
  TechPtr = NBPtr->TechPtr;
  VoltageMap = 0xFF;

  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    NBPtr->SwitchDCT (NBPtr, Dct);
    for (Channel = 0; Channel < NBPtr->ChannelCount; Channel++) {
      NBPtr->SwitchChannel (NBPtr, Channel);
      ChannelPtr = NBPtr->ChannelPtr;
      for (Dimm = 0; Dimm < MAX_DIMMS_PER_CHANNEL; Dimm++) {
        if (TechPtr->GetDimmSpdBuffer (TechPtr, &SpdBufferPtr, Dimm)) {
          // SPD byte 6: Module Nominal Voltage, VDD
          // 1.5v - bit 0
          // 1.35v - bit 1
          // 1.2v - bit 2
          VDDByte = SpdBufferPtr[MNVVDD];
          IDS_HDT_CONSOLE (MEM_FLOW, "Node%d DCT%d Channel%d Dimm%d VDD Byte: 0x%02x\n", NBPtr->Node, Dct, Channel, Dimm, VDDByte);

          // Reverse the 1.5V operable bit. So its encoding can be consistent
          // with that of 1.35V and 1.25V operable bit.
          VDDByte ^= 1;
          ASSERT (VDDByte != 0);

          if (mmSharedPtr->VoltageMap != 0) {
            // Get the common supported voltage map
            VoltageMap &= VDDByte;
          } else {
            // This is the second execution of all the loop as no common voltage is found
            if (VDDByte == (1 << VOLT1_5_ENCODED_VAL)) {
              // Always exclude 1.5V dimm if no common voltage is found
              ChannelPtr->DimmExclude |= (UINT16) 1 << Dimm;
            }
          }
        }
      }
      if (mmSharedPtr->VoltageMap == 0) {
        NBPtr->DCTPtr->Timings.DimmExclude |= ChannelPtr->DimmExclude;
      }
    }
  }

  if (mmSharedPtr->VoltageMap != 0) {
    mmSharedPtr->VoltageMap &= VoltageMap;
  }

  return TRUE;
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
