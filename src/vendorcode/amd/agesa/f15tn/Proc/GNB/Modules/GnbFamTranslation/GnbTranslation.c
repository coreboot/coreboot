/* $NoKeywords:$ */
/**
 * @file
 *
 * Family specific function translation
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
*****************************************************************************
*
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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
* ***************************************************************************
*
*/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "AGESA.h"
#include  "Ids.h"
#include  "amdlib.h"
#include  "Gnb.h"
#include  "GnbPcieConfig.h"
#include  "GnbFamServices.h"
#include  "GnbCommonLib.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBFAMTRANSLATION_GNBTRANSLATION_FILECODE
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
 * Check if IOMMU unit present and enabled
 *
 *
 *
 *
 * @param[in]  GnbHandle       Gnb handle
 * @param[in]  StdHeader       Standard configuration header
 *
 */
BOOLEAN
GnbFmCheckIommuPresent (
  IN       GNB_HANDLE                 *GnbHandle,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  )
{
  AGESA_STATUS              Status;
  GNB_FAM_IOMMU_SERVICES    *GnbIommuConfigService;
  Status = GnbLibLocateService (GnbIommuService, GnbGetSocketId (GnbHandle), (CONST VOID **)&GnbIommuConfigService, StdHeader);
  ASSERT (Status == AGESA_SUCCESS);
  if (Status == AGESA_SUCCESS) {
    return GnbIommuConfigService->GnbFmCheckIommuPresent (GnbHandle, StdHeader);
  }
  return FALSE;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Create IVRS entry
 *
 *
 * @param[in]  GnbHandle       Gnb handle
 * @param[in]  Type            Entry type
 * @param[in]  Ivrs            IVRS table pointer
 * @param[in]  StdHeader       Standard configuration header
 *
 */

AGESA_STATUS
GnbFmCreateIvrsEntry (
  IN       GNB_HANDLE                 *GnbHandle,
  IN       IVRS_BLOCK_TYPE            Type,
  IN       VOID                       *Ivrs,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  )
{
  AGESA_STATUS              Status;
  GNB_FAM_IOMMU_SERVICES    *GnbIommuConfigService;
  Status = GnbLibLocateService (GnbIommuService, GnbGetSocketId (GnbHandle), (CONST VOID **)&GnbIommuConfigService, StdHeader);
  ASSERT (Status == AGESA_SUCCESS);
  if (Status == AGESA_SUCCESS) {
    return GnbIommuConfigService->GnbFmCreateIvrsEntry (GnbHandle, Type, Ivrs, StdHeader);
  }
  return Status;
}
