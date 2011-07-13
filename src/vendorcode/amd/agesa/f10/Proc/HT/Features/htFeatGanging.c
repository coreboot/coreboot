/**
 * @file
 *
 * Routines for re-ganging Links.
 *
 * Implement the reganging feature.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  HyperTransport
 * @e \$Revision: 44323 $   @e \$Date: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
 *
 */
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
#include "amdlib.h"
#include "Ids.h"
#include "Topology.h"
#include "htFeat.h"
#include "htInterface.h"
#include "htNb.h"
#include "htFeatGanging.h"
#include "Filecode.h"
#define FILECODE PROC_HT_FEATURES_HTFEATGANGING_FILECODE
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
 * Test the subLinks of a Link to see if they qualify to be reganged.
 *
 * @HtFeatMethod{::F_REGANG_LINKS}
 *
 * If they do, update the port list data to indicate that this should be done.
 * @note  no actual hardware state is changed in this routine.
 *
 * @param[in,out] State          Our global state
 */
VOID
RegangLinks (
  IN OUT   STATE_DATA *State
  )
{
  FINAL_LINK_STATE FinalLinkState;
  UINT8 i;
  UINT8 j;
  for (i = 0; i < (State->TotalLinks * 2); i += 2) {
    // Data validation
    ASSERT ((*State->PortList)[i].Type < 2 && (*State->PortList)[i].Link < State->Nb->MaxLinks);
    ASSERT ((*State->PortList)[i + 1].Type < 2 && (*State->PortList)[i + 1].Link < State->Nb->MaxLinks);

    // Regang is false unless we pass all conditions below
    (*State->PortList)[i].SelRegang = FALSE;
    (*State->PortList)[i + 1].SelRegang = FALSE;

    // Only process cpu to cpu Links
    if ( ((*State->PortList)[i].Type != PORTLIST_TYPE_CPU) ||
         ((*State->PortList)[i + 1].Type != PORTLIST_TYPE_CPU)) {
      continue;
    }

    for (j = i + 2; j < State->TotalLinks*2; j += 2) {
      // Only process cpu to cpu Links
      if ( ((*State->PortList)[j].Type != PORTLIST_TYPE_CPU) ||
           ((*State->PortList)[j + 1].Type != PORTLIST_TYPE_CPU) ) {
        continue;
      }

      // Links must be from the same source
      if ((*State->PortList)[i].NodeID != (*State->PortList)[j].NodeID) {
        continue;
      }

      // Link must be to the same target
      if ((*State->PortList)[i + 1].NodeID != (*State->PortList)[j + 1].NodeID) {
        continue;
      }

      // Ensure same source base port
      if (((*State->PortList)[i].Link & 3) != ((*State->PortList)[j].Link & 3)) {
        continue;
      }

      // Ensure same destination base port
      if (((*State->PortList)[i + 1].Link & 3) != ((*State->PortList)[j + 1].Link & 3)) {
        continue;
      }

      // Ensure subLink0 routes to subLink0
      if (((*State->PortList)[i].Link & 4) != ((*State->PortList)[i + 1].Link & 4)) {
        continue;
      }

      // (therefore subLink1 routes to subLink1)
      ASSERT (((*State->PortList)[j].Link & 4) == ((*State->PortList)[j + 1].Link & 4));

      FinalLinkState = State->HtInterface->GetSkipRegang ((*State->PortList)[i].NodeID,
                                         (*State->PortList)[i].Link & 0x03,
                                         (*State->PortList)[i + 1].NodeID,
                                         (*State->PortList)[i + 1].Link & 0x03,
                                         State);
      if (FinalLinkState == MATCHED) {
        continue;
      } else if (FinalLinkState == POWERED_OFF) {
        // StopLink will be done on the sublink 1, thus OR in 4 to the link to ensure it.
        State->Nb->StopLink ((*State->PortList)[i].NodeID, ((*State->PortList)[i].Link | 4), State, State->Nb);
        State->Nb->StopLink ((*State->PortList)[i + 1].NodeID, ((*State->PortList)[i + 1].Link | 4), State, State->Nb);
      }

      //
      // Create a ganged portlist entry for the two regang-able subLinks.
      //
      // All info will be that of subLink zero.
      // (If Link discovery order was other than ascending, fix the .Pointer field too.)
      //
      //
      if (((*State->PortList)[i].Link & 4) != 0) {
        (*State->PortList)[i].Pointer = (*State->PortList)[j].Pointer;
        (*State->PortList)[i + 1].Pointer = (*State->PortList)[j + 1].Pointer;
      }
      (*State->PortList)[i].Link &= 0x03;                       // Force to point to subLink0
      (*State->PortList)[i + 1].Link &= 0x03;
      // If powered off, sublink 1 is removed but the link is still 8 bits.
      if (FinalLinkState != POWERED_OFF) {
        (*State->PortList)[i].SelRegang = TRUE;                   // Enable Link reganging
        (*State->PortList)[i + 1].SelRegang = TRUE;
        (*State->PortList)[i].PrvWidthOutCap = HT_WIDTH_16_BITS;
        (*State->PortList)[i + 1].PrvWidthOutCap = HT_WIDTH_16_BITS;
        (*State->PortList)[i].PrvWidthInCap = HT_WIDTH_16_BITS;
        (*State->PortList)[i + 1].PrvWidthInCap = HT_WIDTH_16_BITS;
      }

      // Delete PortList[j, j + 1], slow but easy to debug implementation
      State->TotalLinks--;
      LibAmdMemCopy (&((*State->PortList)[j]),
                     &((*State->PortList)[j + 2]),
                     sizeof (PORT_DESCRIPTOR)*(State->TotalLinks* 2 - j),
                     State->ConfigHandle);
      LibAmdMemFill (&((*State->PortList)[State->TotalLinks * 2]), INVALID_LINK, (sizeof (PORT_DESCRIPTOR) * 2), State->ConfigHandle);

      break; // Exit loop, advance to PortList[i + 2]
    }
  }
}
