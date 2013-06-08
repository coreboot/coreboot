/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD MMIO Map Manager APIs, and related functions.
 *
 * Contains code that manage MMIO base/limit registers
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 63522 $   @e \$Date: 2011-12-25 20:25:03 -0600 (Sun, 25 Dec 2011) $
 *
 */
/*******************************************************************************
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
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */

#include "AGESA.h"
#include "amdlib.h"
#include "Ids.h"
#include "cpuServices.h"
#include "cpuFamilyTranslation.h"
#include "mmioMapManager.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_MMIOMAPMANAGER_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern CPU_FAMILY_SUPPORT_TABLE MmioMapFamilyServiceTable;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                          P U B L I C     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 *  MMIO map manager
 *
 * @param[in]    AmdAddMmioParams   Pointer to a data structure containing the parameter information.
 *
 * @retval       AGESA_STATUS       AGESA_ERROR       - The requested range could not be added because there are not
 *                                                      enough mapping resources.
 *                                  AGESA_UNSUPPORTED - not support on currect processor
 *                                  AGESA_BOUNDS_CHK  - One or more input parameters are invalid. For example, the
 *                                                      TargetAddress does not correspond to any device in the system.
 *                                  AGESA_SUCCESS     - Adding MMIO map succeeds
 *
 */
AGESA_STATUS
AmdAddMmioMapping (
  IN       AMD_ADD_MMIO_PARAMS AmdAddMmioParams
  )
{
  MMIO_MAP_FAMILY_SERVICES *FamilyServices;

  GetFeatureServicesOfCurrentCore (&MmioMapFamilyServiceTable, (CONST VOID **)&FamilyServices, &(AmdAddMmioParams.StdHeader));
  if (FamilyServices != NULL) {
    return (FamilyServices->addingMmioMap (FamilyServices, AmdAddMmioParams));
  } else {
    return AGESA_UNSUPPORTED;
  }
}

