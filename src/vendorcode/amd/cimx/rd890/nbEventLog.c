
/**
 * @file
 *
 *
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CIMx-NB
 * @e sub-project:
 * @e \$Revision:$   @e \$Date:$
 *
 */
/*****************************************************************************
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
 *
 ***************************************************************************/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */

#include "NbPlatform.h"

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * This function logs NB events into a callback.
 *
 *
 *
 * @param[in]  EventClass
 * @param[in]  EventInfo
 * @param[in]  DataParam1
 * @param[in]  DataParam2
 * @param[in]  DataParam3
 * @param[in]  DataParam4
 * @param[in]  NbConfigPtr  Northbridge configuration structure pointer.
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
LibNbEventLog (
  IN UINTN          EventClass,
  IN UINT32         EventInfo,
  IN UINT32         DataParam1,
  IN UINT32         DataParam2,
  IN UINT32         DataParam3,
  IN UINT32         DataParam4,
  IN AMD_NB_CONFIG  *NbConfigPtr
  )
{
  AGESA_EVENT   Event;
  Event.EventClass = EventClass;
  Event.EventInfo  = EventInfo;
  Event.DataParam1 = DataParam1;
  Event.DataParam2 = DataParam2;
  Event.DataParam3 = DataParam3;
  Event.DataParam4 = DataParam4;
  if (EventClass > ((API_WORKSPACE*)NbConfigPtr->ConfigPtr)->Status) {
    ((API_WORKSPACE*)NbConfigPtr->ConfigPtr)->Status = EventClass;
  }
  LibNbCallBack (PHCB_AmdReportEvent, (UINTN)&Event, NbConfigPtr);
}
