/**
 * @file
 *
 *  Function dispatcher.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CIMx-SB
 * @e sub-project:
 * @e \$Revision:$   @e \$Date:$
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
/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */

#include "SBPLATFORM.h"
#include "cbtypes.h"

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
*/


//
// Declaration of local functions
//

VOID  saveConfigPointer (IN AMDSBCFG* pConfig);

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/**
 * AmdSbDispatcher - Dispatch Southbridge function
 *
 *
 *
 * @param[in] pConfig   Southbridge configuration structure pointer.
 *
 */
AGESA_STATUS
AmdSbDispatcher (
  IN       VOID *pConfig
  )
{
  AGESA_STATUS Status;

  Status = AGESA_UNSUPPORTED;

  saveConfigPointer (pConfig);

  if ( ((AMD_CONFIG_PARAMS*)pConfig)->Func == SB_POWERON_INIT ) {
    sbPowerOnInit ((AMDSBCFG*) pConfig);
  }

  if ( ((AMD_CONFIG_PARAMS*)pConfig)->Func == SB_BEFORE_PCI_INIT ) {
    sbBeforePciInit ((AMDSBCFG*)pConfig);
  }

  if ( ((AMD_CONFIG_PARAMS*)pConfig)->Func == SB_AFTER_PCI_INIT ) {
    sbAfterPciInit ((AMDSBCFG*)pConfig);
  }

  if ( ((AMD_CONFIG_PARAMS*)pConfig)->Func == SB_MID_POST_INIT ) {
    sbMidPostInit ((AMDSBCFG*)pConfig);
  }

  if ( ((AMD_CONFIG_PARAMS*)pConfig)->Func == SB_LATE_POST_INIT ) {
    sbLatePost ((AMDSBCFG*)pConfig);
  }

  if ( ((AMD_CONFIG_PARAMS*)pConfig)->Func == SB_BEFORE_PCI_RESTORE_INIT ) {
    sbBeforePciRestoreInit ((AMDSBCFG*)pConfig);
  }

  if ( ((AMD_CONFIG_PARAMS*)pConfig)->Func == SB_AFTER_PCI_RESTORE_INIT ) {
    sbAfterPciRestoreInit ((AMDSBCFG*)pConfig);
  }

  if ( ((AMD_CONFIG_PARAMS*)pConfig)->Func == SB_SMM_SERVICE ) {
    sbSmmService ((AMDSBCFG*)pConfig);
  }

  if ( ((AMD_CONFIG_PARAMS*)pConfig)->Func == SB_SMM_ACPION ) {
    sbSmmAcpiOn ((AMDSBCFG*)pConfig);
  }

  if ( ((AMD_CONFIG_PARAMS*)pConfig)->Func == SB_EC_FANCONTROL ) {
    sbECfancontrolservice((AMDSBCFG*)pConfig);
  }
  return Status;
}

/**
 * saveConfigPointer - Verify Southbridge CIMx module
 *
 *
 * @param[in] pConfig   Southbridge configuration structure pointer.
 *
 */
VOID
saveConfigPointer (
  IN       AMDSBCFG* pConfig
  )
{
  UINT8   dbReg;
  UINT8   i;
  UINT32   ddValue;

  ddValue =  (UINT32) (UINTN)pConfig; // Needs to live below 4G
  dbReg = SB_ECMOS_REG08;

  for ( i = 0; i <= 3; i++ ) {
    WriteIO (SB_IOMAP_REG72, AccWidthUint8, &dbReg);
    WriteIO (SB_IOMAP_REG73, AccWidthUint8, (UINT8*)&ddValue);
    ddValue = (ddValue >> 8);
    dbReg++;
  }
}
