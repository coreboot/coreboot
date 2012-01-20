/* $NoKeywords:$ */
/**
 * @file
 *
 * SubLink management Routines.
 *
 * Contains routines for subLink frequency ratios.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  HyperTransport
 * @e \$Revision: 56279 $   @e \$Date: 2011-07-11 13:11:28 -0600 (Mon, 11 Jul 2011) $
 *
 */
/*
*****************************************************************************
*
* Copyright (C) 2012 Advanced Micro Devices, Inc.
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
#include "amdlib.h"
#include "Topology.h"
#include "htFeat.h"
#include "IdsHt.h"
#include "htFeatSublinks.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_HT_FEATURES_HTFEATSUBLINKS_FILECODE
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

typedef struct {
  UINT8 HiFreq;
  UINT8 LoFreq;
} VALID_RATIO_ITEM;

STATIC CONST VALID_RATIO_ITEM ROMDATA ValidRatioList[] =
{
  {HT_FREQUENCY_3200M, HT_FREQUENCY_1600M},   // 3200MHz / 1600MHz 2:1
  {HT_FREQUENCY_3200M, HT_FREQUENCY_800M},    // 3200MHz / 800MHz  4:1
  {HT_FREQUENCY_3200M, HT_FREQUENCY_400M},    // 3200MHz / 400MHz  8:1
  {HT_FREQUENCY_2800M, HT_FREQUENCY_1400M},   // 2800MHz / 1400MHz 2:1
  {HT_FREQUENCY_2400M, HT_FREQUENCY_1200M},   // 2400MHz / 1200MHz 2:1
  {HT_FREQUENCY_2400M, HT_FREQUENCY_600M},    // 2400MHz /  600MHz 4:1
  {HT_FREQUENCY_2400M, HT_FREQUENCY_400M},    // 2400MHz /  400MHz 6:1
  {HT_FREQUENCY_2000M, HT_FREQUENCY_1000M},   // 2000MHz / 1000MHz 2:1
  {HT_FREQUENCY_1600M, HT_FREQUENCY_800M},    // 1600MHz /  800MHz 2:1
  {HT_FREQUENCY_1600M, HT_FREQUENCY_400M},    // 1600MHz /  400MHz 4:1
  {HT_FREQUENCY_1600M, HT_FREQUENCY_200M},    // 1600MHz /  200Mhz 8:1
  {HT_FREQUENCY_1200M, HT_FREQUENCY_600M},    // 1200MHz /  600MHz 2:1
  {HT_FREQUENCY_1200M, HT_FREQUENCY_200M},    // 1200MHz /  200MHz 6:1
  {HT_FREQUENCY_800M,  HT_FREQUENCY_400M},    // 800MHz /  400MHz 2:1
  {HT_FREQUENCY_800M,  HT_FREQUENCY_200M},    // 800MHz /  200MHz 4:1
  {HT_FREQUENCY_400M,  HT_FREQUENCY_200M}     // 400MHz /  200MHz 2:1
};

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

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/***************************************************************************
 ***                            Link Optimization                        ***
 ***************************************************************************/

/*----------------------------------------------------------------------------------------*/
/**
 * Iterate through all Links, checking the frequency of each subLink pair.
 *
 * @HtFeatMethod{::F_SUBLINK_RATIO_FIXUP}
 *
 * Make the adjustment to the port list data so that the frequencies
 * are at a valid ratio, reducing frequency as needed to achieve
 * this. (All Links support the minimum 200 MHz frequency.)  Repeat
 * the above until no adjustments are needed.
 * @note no hardware state changes in this routine.
 *
 * @param[in,out]     State     Link state and port list
 *
 */
VOID
SubLinkRatioFixup (
  IN OUT   STATE_DATA    *State
  )
{
  UINT8 i;
  UINT8 j;
  UINT8 ValidRatioItem;
  BOOLEAN Changes;
  BOOLEAN Downgrade;
  UINT8 HiIndex;
  UINT8 HiFreq;
  UINT8 LoFreq;

  UINT32 Temp;

  do {
    Changes = FALSE;
    for (i = 0; i < State->TotalLinks*2; i++) {
      // Must be a CPU Link
      if ((*State->PortList)[i].Type != PORTLIST_TYPE_CPU) {
        continue;
      }
      // Only look for subLink1's
      if ((*State->PortList)[i].Link < 4) {
        continue;
      }

      for (j = 0; j < State->TotalLinks*2; j++) {
        // Step 1. Find the matching subLink0
        if ((*State->PortList)[j].Type != PORTLIST_TYPE_CPU) {
          continue;
        }
        if ((*State->PortList)[j].NodeID != (*State->PortList)[i].NodeID) {
          continue;
        }
        if ((*State->PortList)[j].Link != ((*State->PortList)[i].Link & 0x03)) {
          continue;
        }

        // Step 2. Check for an illegal frequency ratio
        if ((*State->PortList)[i].SelFrequency >= (*State->PortList)[j].SelFrequency) {
          HiIndex = i;
          HiFreq = (*State->PortList)[i].SelFrequency;
          LoFreq = (*State->PortList)[j].SelFrequency;
        } else {
          HiIndex = j;
          HiFreq = (*State->PortList)[j].SelFrequency;
          LoFreq = (*State->PortList)[i].SelFrequency;
        }

        // The frequencies are 1:1, no need to do anything
        if (HiFreq == LoFreq) {
          break;
        }

        Downgrade = TRUE;

        for (ValidRatioItem = 0; ValidRatioItem < (sizeof (ValidRatioList) / sizeof (VALID_RATIO_ITEM)); ValidRatioItem++) {
          if ((HiFreq == ValidRatioList[ValidRatioItem].HiFreq) &&
              (LoFreq == ValidRatioList[ValidRatioItem].LoFreq)) {
            Downgrade = FALSE;
            break;
          }
        }

        // Step 3. Downgrade the higher of the two frequencies, and set Changes to FALSE
        if (Downgrade) {
          // Although the problem was with the port specified by hiIndex, we need to
          // Downgrade both ends of the Link.
          HiIndex = HiIndex & 0xFE; // Select the 'upstream' (i.e. even) port

          Temp = (*State->PortList)[HiIndex].CompositeFrequencyCap;

          // Remove HiFreq from the list of valid frequencies
          Temp = Temp & ~((UINT32)1 << HiFreq);
          ASSERT (Temp != 0);
          (*State->PortList)[HiIndex].CompositeFrequencyCap = (UINT32)Temp;
          (*State->PortList)[HiIndex + 1].CompositeFrequencyCap = (UINT32)Temp;

          HiFreq = LibAmdBitScanReverse (Temp);

          (*State->PortList)[HiIndex].SelFrequency = HiFreq;
          (*State->PortList)[HiIndex + 1].SelFrequency = HiFreq;

          Changes = TRUE;
        }
      }
    }
  } while (Changes); // Repeat until a valid configuration is reached
}
