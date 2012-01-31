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

#ifdef B1_IMAGE
  // 1. Try to execute any B1 specific functions
  switch (((AMD_CONFIG_PARAMS*)ConfigPtr)->Func) {
    #ifdef B1_IMAGE
    // B1 ONLY Functions
    //
    //
    #endif
  default:
    break;
  }
#endif

  // 2. If not B1 specific function but we are B1, see if we can find B2 instead
#ifdef B1_IMAGE
  if (Status == AGESA_UNSUPPORTED) {
    UINTN ImageStart;
    UINTN ImageEnd;
    AMD_IMAGE_HEADER* AltImagePtr;
    ImageStart = 0xFFF00000;
    ImageEnd = 0xFFFFFFFF;
    AltImagePtr = (AMD_IMAGE_HEADER*) (UINTN) ((AMD_CONFIG_PARAMS*)ConfigPtr)->AltImageBasePtr;

    if ((UINTN)AltImagePtr != 0xFFFFFFFF) {
      if (AltImagePtr != NULL) {
        ImageStart = (UINT32) (UINTN)AltImagePtr;
        ImageEnd = ImageStart + 4;
      }
      // Locate/test image base that matches this component
      AltImagePtr = LibAmdLocateImage ((VOID*)ImageStart, (VOID*)ImageEnd, 4096, CIMX_NB_ID);
      if (AltImagePtr != NULL) {
        //Invoke alternative Image
        ImageEntry = (IMAGE_ENTRY) (UINTN) ((UINT8*) AltImagePtr + AltImagePtr->EntryPointAddress);
        Status = (*ImageEntry) (ConfigPtr);
      }
    }
  }
#endif
  if (Status == AGESA_UNSUPPORTED) {
 // 3. Try to execute any other functions
    switch (((AMD_CONFIG_PARAMS*)ConfigPtr)->Func) {

#if defined (B1_IMAGE) || defined (B2_IMAGE)
// B1 & B2 Functions
    case PH_AmdPowerOnResetInit:
      Status = LibSystemApiCall (AmdPowerOnResetInit, ConfigPtr);
      break;
    case PH_AmdPcieEarlyInit:
      Status = LibSystemApiCall (AmdPcieEarlyInit, ConfigPtr);
      break;
    case PH_AmdInitializer:
      Status = LibSystemApiCall (AmdInitializer, ConfigPtr);
      break;
#endif
#ifdef B2_IMAGE
// B2 Functions
    case PH_AmdNbHtInit :
      Status = LibSystemApiCall (AmdHtInit, ConfigPtr);
      break;
    case PH_AmdEarlyPostInit  :
      LibSystemApiCall (AmdMaskedMemoryInit, ConfigPtr);
      Status = LibSystemApiCall (AmdEarlyPostInit, ConfigPtr);
      break;
    case PH_AmdMidPostInit  :
      Status = LibSystemApiCall (AmdMidPostInit, ConfigPtr);
      break;
    case PH_AmdLatePostInit :
      Status = LibSystemApiCall (AmdPcieLateInit, ConfigPtr);
      Status = LibSystemApiCall (AmdLatePostInit, ConfigPtr);
      Status = LibSystemApiCall (AmdPcieLateInitWa, ConfigPtr);
      break;
    case PH_AmdPcieValidatePortState :
      Status = LibSystemApiCall (AmdPcieValidatePortState, ConfigPtr);
      break;
    case PH_AmdPcieLateInit :
      Status = LibSystemApiCall (AmdPcieLateInit, ConfigPtr);
      break;
    case PH_AmdNbLateInit :
      Status = LibSystemApiCall (AmdLatePostInit, ConfigPtr);
      break;
    case PH_AmdS3Init :
      LibSystemApiCall (AmdMaskedMemoryInit, ConfigPtr);
      Status = LibSystemApiCall (AmdS3InitIommu, ConfigPtr);
      Status = LibSystemApiCall (AmdPcieS3Init, ConfigPtr);
      Status = LibSystemApiCall (AmdS3Init, ConfigPtr);
      Status = LibSystemApiCall (AmdPcieLateInitWa, ConfigPtr);
      break;
    case PH_AmdNbS3Init :
      LibSystemApiCall (AmdMaskedMemoryInit, ConfigPtr);
      Status = LibSystemApiCall (AmdS3Init, ConfigPtr);
      break;
    case PH_AmdPcieS3Init :
      Status = LibSystemApiCall (AmdS3InitIommu, ConfigPtr);
      Status = LibSystemApiCall (AmdPcieS3Init, ConfigPtr);
      break;
#endif
#ifdef B3_IMAGE
// B3 Functions
#endif
    default:
      break;
    }
  }

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
