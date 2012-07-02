/* $NoKeywords:$ */
/**
 * @file
 *
 * mt3.c
 *
 * Common Technology  functions for DDR3
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
#include "mtspd3.h"
#include "mtot3.h"
#include "OptionMemory.h"
#include "PlatformMemoryConfiguration.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

/* features */
#define FILECODE PROC_MEM_TECH_DDR3_MT3_FILECODE
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
 *   This function Constructs the technology block
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

BOOLEAN
MemConstructTechBlock3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  TECHNOLOGY_TYPE *TechTypePtr;
  UINT8 Dct;
  UINT8 Channel;
  UINT8 i;
  DIE_STRUCT *MCTPtr;
  DCT_STRUCT *DCTPtr;
  CH_DEF_STRUCT *ChannelPtr;
  UINT8 DimmSlots;


  TechTypePtr = (TECHNOLOGY_TYPE *) FindPSOverrideEntry (NBPtr->RefPtr->PlatformMemoryConfiguration, PSO_MEM_TECH, NBPtr->MCTPtr->SocketId, 0, 0, NULL, NULL);
  if (TechTypePtr != NULL) {
    // Ensure the platform override value is valid
    ASSERT ((*TechTypePtr == DDR3_TECHNOLOGY) || (*TechTypePtr == DDR2_TECHNOLOGY));
    if (*TechTypePtr != DDR3_TECHNOLOGY) {
      return FALSE;
    }
  }

  TechPtr->NBPtr = NBPtr;
  TechPtr->RefPtr = NBPtr->RefPtr;
  MCTPtr = NBPtr->MCTPtr;

  TechPtr->SendAllMRCmds = MemTSendAllMRCmds3;
  TechPtr->FreqChgCtrlWrd = FreqChgCtrlWrd3;
  TechPtr->SetDramMode = MemTSetDramMode3;
  TechPtr->DimmPresence = MemTDIMMPresence3;
  TechPtr->SpdCalcWidth = MemTSPDCalcWidth3;
  TechPtr->SpdGetTargetSpeed = MemTSPDGetTargetSpeed3;
  TechPtr->AutoCycTiming = MemTAutoCycTiming3;
  TechPtr->SpdSetBanks = MemTSPDSetBanks3;
  TechPtr->SetDqsEccTmgs = MemTSetDQSEccTmgs;
  TechPtr->GetCSIntLvAddr = MemTGetCSIntLvAddr3;
  TechPtr->AdjustTwrwr = MemTAdjustTwrwr3;
  TechPtr->AdjustTwrrd = MemTAdjustTwrrd3;
  TechPtr->GetDimmSpdBuffer = MemTGetDimmSpdBuffer3;
  TechPtr->GetLD = MemTGetLD3;
  TechPtr->MaxFilterDly = 0;

  //
  // Map the Logical Dimms on this channel to the SPD that should be used for that logical DIMM.
  // The pointers to the DIMM SPD information is as follows (2 Dimm/Ch and 3 Dimm/Ch examples).
  //
  //    DIMM Spd Buffer                          Current Channel DimmSpdPtr[MAX_DIMMS_PER_CHANNEL] array
  //    (Number of dimms varies by platform)     (Array size is determined in AGESA.H) Dimm operations loop
  //                                              on this array only)
  //    2 DIMMS PER CHANNEL
  //
  //    Socket N  Channel N Dimm 0 SR/DR DIMM <--------------DimmSpdPtr[0]
  //                        Dimm 1 SR/DR DIMM <--------------DimmSpdPtr[1]
  //                                                         DimmSpdPtr[2]------->NULL
  //                                                         DimmSpdPtr[3]------->NULL
  //
  //    Socket N  Channel N Dimm 0 SR/DR DIMM <--------------DimmSpdPtr[0]
  //                        Dimm 1    QR DIMM <---------+----DimmSpdPtr[1]
  //                                                    |    DimmSpdPtr[2]------->NULL
  //                                                    +----DimmSpdPtr[3]
  //
  //    Socket N  Channel N Dimm 0    QR DIMM <-----+--------DimmSpdPtr[0]
  //                        Dimm 1    QR DIMM <-----|---+----DimmSpdPtr[1]
  //                                                +-- | ---DimmSpdPtr[2]
  //                                                    +----DimmSpdPtr[3]
  //
  //    3 DIMMS PER CHANNEL
  //
  //    Socket N  Channel N Dimm 0 SR/DR DIMM <--------------DimmSpdPtr[0]
  //                        Dimm 1 SR/DR DIMM <--------------DimmSpdPtr[1]
  //                        Dimm 3 SR/DR DIMM <--------------DimmSpdPtr[2]
  //                                                         DimmSpdPtr[3]------->NULL
  //
  //    Socket N  Channel N Dimm 0 SR/DR DIMM <--------------DimmSpdPtr[0]
  //                        Dimm 1    QR DIMM <---------+----DimmSpdPtr[1]
  //                        Dimm 3 SR/DR DIMM <-------- | ---DimmSpdPtr[2]
  //                                                    +----DimmSpdPtr[3]
  //
  //
  //    FOR LRDIMMS
  //
  //    This code will assign SPD pointers on the basis of Physical ranks, even though
  //    an LRDIMM may only use one or two logical ranks, that determination will have to
  //    be made from downstream code.  An LRDIMM with greater than 2 Physical ranks will have
  //    its DimmSpdPtr[] mapped as if it were a QR in the above diagrams.

  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    NBPtr->SwitchDCT (NBPtr, Dct);
    DCTPtr = NBPtr->DCTPtr;
    for (Channel = 0; Channel < NBPtr->ChannelCount; Channel++) {
      NBPtr->SwitchChannel (NBPtr, Channel);
      ChannelPtr = NBPtr->ChannelPtr;
      ChannelPtr->TechType = DDR3_TECHNOLOGY;
      ChannelPtr->MCTPtr = MCTPtr;
      ChannelPtr->DCTPtr = DCTPtr;

      DimmSlots = GetMaxDimmsPerChannel (NBPtr->RefPtr->PlatformMemoryConfiguration,
                                         MCTPtr->SocketId,
                                         NBPtr->GetSocketRelativeChannel (NBPtr, Dct, Channel)
                                        );
      //
      // Initialize the SPD pointers for each Dimm
      //
      for (i = 0 ; i < (sizeof (ChannelPtr->DimmSpdPtr) / sizeof (ChannelPtr->DimmSpdPtr[0])) ; i++) {
        ChannelPtr->DimmSpdPtr[i] = NULL;
      }
      for (i = 0 ; i < DimmSlots; i++) {
        ChannelPtr->DimmSpdPtr[i] = &(ChannelPtr->SpdPtr[i]);
        if ( (i + 2) < (sizeof (ChannelPtr->DimmSpdPtr) / sizeof (ChannelPtr->DimmSpdPtr[0]))) {
          if (ChannelPtr->DimmSpdPtr[i]->DimmPresent) {
            if ((((ChannelPtr->DimmSpdPtr[i]->Data[SPD_RANKS] >> 3) & 0x07) + 1) > 2) {
              ChannelPtr->DimmSpdPtr[i + 2] = &(ChannelPtr->SpdPtr[i]);
            }
          }
        }
      }
    }
  }
  // Initialize Common technology functions
  MemTCommonTechInit (TechPtr);

  return TRUE;
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
