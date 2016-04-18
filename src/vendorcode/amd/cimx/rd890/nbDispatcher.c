/**
 * @file
 *
 *  Function dispatcher.
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
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

extern AMD_MODULE_HEADER mNbModuleID;

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

AGESA_STATUS
CALLCONV
AmdNbDispatcher (
  IN OUT   VOID*ConfigPtr
  )
{
  AGESA_STATUS Status;
  IMAGE_ENTRY ImageEntry;
  ImageEntry = NULL;
  Status = AGESA_UNSUPPORTED;
  CIMX_INIT_TRACE ((ConfigPtr));
  CIMX_TRACE ((TRACE_DATA (ConfigPtr, CIMX_TRACE_ALL), "CIMx - RD890 Entry \n"));
  CIMX_TRACE ((TRACE_DATA (ConfigPtr, CIMX_TRACE_ALL), "    Funcid = %x Callout = %x\n", ((AMD_CONFIG_PARAMS*)ConfigPtr)->Func, ((AMD_CONFIG_PARAMS*)ConfigPtr)->CalloutPtr));


  // 2. AltImage not supported


 // 4. Try next dispatcher if possible, and we have not already got status back
  if ((mNbModuleID.NextBlock != NULL) && (Status == AGESA_UNSUPPORTED)) {
    MODULE_ENTRY  ModuleEntry;
    CIMX_TRACE ((TRACE_DATA (ConfigPtr, CIMX_TRACE_ALL), "CIMx - RD890 control goes to next Module \n"));
    ModuleEntry = mNbModuleID.NextBlock->ModuleDispatcher;
    Status = (*ModuleEntry) (ConfigPtr);
  }
  CIMX_TRACE ((TRACE_DATA (ConfigPtr, CIMX_TRACE_ALL), "CIMx - RD890 Exit\n"));
  return Status;
}
