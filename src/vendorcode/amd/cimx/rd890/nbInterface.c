/**
 * @file
 *
 *  NB Post Init interfaces
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
#include "amdDebugOutLib.h"

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

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Amd Init all NB at early POST.
 *
 *
 *
  * @param[in] ConfigPtr   Northbridges configuration block pointer.
 *
 */

AGESA_STATUS
AmdEarlyPostInit (
  IN OUT   AMD_NB_CONFIG_BLOCK *ConfigPtr
  )
{
  AGESA_STATUS  Status;

  Status = LibNbApiCall (NbEarlyPostInit, ConfigPtr);
  return  Status;
}


/*----------------------------------------------------------------------------------------*/
/**
 * NB Init at early post.
 *
 *
 *
 * @param[in] NbConfigPtr    Northbridge configuration structure pointer.
 *
 */

AGESA_STATUS
NbEarlyPostInit (
  IN OUT   AMD_NB_CONFIG  *NbConfigPtr
  )
{
  AGESA_STATUS  Status;
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (NbConfigPtr), CIMX_NB_TRACE), "[NB]NbEarlyPostInit Enter\n"));

  Status = NbLibEarlyPostInitValidateInput (NbConfigPtr);
  if (Status == AGESA_FATAL) {
    REPORT_EVENT (AGESA_FATAL, GENERAL_ERROR_BAD_CONFIGURATION, 0, 0, 0, 0, NbConfigPtr);
    CIMX_ASSERT (FALSE);
    return  Status;
  }
  NbLibSetTopOfMemory (NbConfigPtr);
  NbLibSetupClumping (NbConfigPtr);
  NbMultiNbIocInit (NbConfigPtr);
#ifndef EPREADY_WORKAROUND_DISABLED
  PcieEpReadyWorkaround (NbConfigPtr);
#endif
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (NbConfigPtr), CIMX_NB_TRACE), "[NB]NbEarlyPostInit Exit [ %x]\n", Status));
  return  Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Amd Init all NB at mid POST.
 *
 *
 *
  * @param[in] ConfigPtr   Northbridges configuration block pointer.
 *
 */

AGESA_STATUS
AmdMidPostInit (
  IN OUT   AMD_NB_CONFIG_BLOCK *ConfigPtr
  )
{
  AGESA_STATUS  Status;
  Status = LibNbApiCall (NbMidPostInit, ConfigPtr);
#ifndef  IOMMU_SUPPORT_DISABLE
  NbIommuInit (ConfigPtr);
#endif
  return  Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * NB Init at mid POST.
 *
 *
 *
 * @param[in] NbConfigPtr    Northbridge configuration structure pointer.
 *
 */

AGESA_STATUS
NbMidPostInit (
  IN OUT   AMD_NB_CONFIG   *NbConfigPtr
  )
{
  AGESA_STATUS  Status;
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (NbConfigPtr), CIMX_NB_TRACE), "[NB]NbMidPostInit Enter\n"));
  Status = NbLibPostInitValidateInput (NbConfigPtr);
  if (Status == AGESA_FATAL) {
    REPORT_EVENT (AGESA_FATAL, GENERAL_ERROR_BAD_CONFIGURATION, 0, 0, 0, 0, NbConfigPtr);
    CIMX_ASSERT (FALSE);
    return  Status;
  }
  NbLibSetIOAPIC (NbConfigPtr);
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (NbConfigPtr), CIMX_NB_TRACE), "[NB]NbMidPostInit Exit [ %x]\n", Status));
  return  Status;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Amd Init all NB at late POST.
 *
 *
 *
 * @param[in] ConfigPtr   Northbridges configuration block pointer.
 *
 */

AGESA_STATUS
AmdLatePostInit (
  IN OUT   AMD_NB_CONFIG_BLOCK *ConfigPtr
  )
{
  AGESA_STATUS  Status;
  Status = LibNbApiCall (NbLatePostInit, ConfigPtr);
#ifndef  IOMMU_SUPPORT_DISABLE
  NbIommuAcpiFixup (ConfigPtr);
#endif
  return  Status;
}
/*----------------------------------------------------------------------------------------*/
/**
 * NB Init at late post.
 *
 *
 *
 * @param[in] NbConfigPtr    Northbridge configuration structure pointer.
 *
 */

AGESA_STATUS
NbLatePostInit (
  IN OUT   AMD_NB_CONFIG    *NbConfigPtr
  )
{
  AGESA_STATUS  Status;
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (NbConfigPtr), CIMX_NB_TRACE), "[NB]NbLatePostInit Enter\n"));
  Status = NbLibPostInitValidateInput (NbConfigPtr);
  if (Status == AGESA_FATAL) {
    REPORT_EVENT (AGESA_FATAL, GENERAL_ERROR_BAD_CONFIGURATION, 0, 0, 0, 0, NbConfigPtr);
    CIMX_ASSERT (FALSE);
    return Status;
  }
  NbLibGetCore0ApicId (NbConfigPtr);
  NbLibSetNmiRouting (NbConfigPtr);
  NbLibSetSSID (NbConfigPtr);
  Status = NbLibPrepareToOS (NbConfigPtr);
  LibNbSetDefaultIndexes (NbConfigPtr);
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (NbConfigPtr), CIMX_NB_TRACE), "[NB]NbLatePostInit Exit [ %x]\n", Status));
  return  Status;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Amd Init all NB at S3 Resume.
 *
 *
 *
 * @param[in] ConfigPtr   Northbridges configuration block pointer.
 *
 */

AGESA_STATUS
AmdS3InitIommu (
  IN OUT   AMD_NB_CONFIG_BLOCK *ConfigPtr
  )
{

#ifndef  IOMMU_SUPPORT_DISABLE
  NbIommuInitS3 (ConfigPtr);
#endif

  return  AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Amd Init all NB at S3 Resume.
 *
 *
 *
 * @param[in] ConfigPtr   Northbridges configuration block pointer.
 *
 */

AGESA_STATUS
AmdS3Init (
  IN OUT   AMD_NB_CONFIG_BLOCK *ConfigPtr
  )
{
  AGESA_STATUS  Status;

  Status = LibNbApiCall (NbS3Init, ConfigPtr);

  return  Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * NB Init at S3
 *
 *
 *
 * @param[in] NbConfigPtr    Northbridge configuration structure pointer.
 *
 */

AGESA_STATUS
NbS3Init (
  IN OUT   AMD_NB_CONFIG     *NbConfigPtr
  )
{
  AGESA_STATUS  Status;
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (NbConfigPtr), CIMX_NB_TRACE), "[NB]NbS3Init Enter\n"));
  Status = NbLibPostInitValidateInput (NbConfigPtr);
  if (Status == AGESA_FATAL) {
    REPORT_EVENT (AGESA_FATAL, GENERAL_ERROR_BAD_CONFIGURATION, 0, 0, 0, 0, NbConfigPtr);
    CIMX_ASSERT (FALSE);
    return Status;
  }
  Status = NbLibSetTopOfMemory (NbConfigPtr);
  NbLibSetupClumping (NbConfigPtr);
  NbMultiNbIocInit (NbConfigPtr);
  NbLibSetSSID (NbConfigPtr);
  NbLibSetIOAPIC (NbConfigPtr);
  NbLibSetNmiRouting (NbConfigPtr);
  Status = NbLibPrepareToOS (NbConfigPtr);
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (NbConfigPtr), CIMX_NB_TRACE), "[NB]NbS3Init Exit [%x]\n", Status));
  return  Status;
}
