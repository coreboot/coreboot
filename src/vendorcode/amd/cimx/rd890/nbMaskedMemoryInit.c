/**
 * @file
 *
 * NB RAS
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
extern  UINT16 NbInitMaskedMemoryLength;

typedef VOID (*MASKED_MEMORY_INIT_PROC) (IN UINT32 PciAddress, IN UINT16 AlinkAddress);


AGESA_STATUS
AmdMaskedMemoryInit (
  IN OUT   AMD_NB_CONFIG_BLOCK *ConfigPtr
  )
{
  AGESA_STATUS  Status;
  Status = LibNbApiCall (NbMaskedMemoryInit, ConfigPtr);
  return  Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Init Mask Memory
 *
 *
 *
 * @param[in] pConfig Northbridge configuration structure pointer.
 *
 */

AGESA_STATUS
NbMaskedMemoryInit (
  IN      AMD_NB_CONFIG  *pConfig
  )
{
  AGESA_STATUS            Status;
  UINT8                   ExecutionBuffer[300];
  SCRATCH_1               Scratch;
  UINT16                  AlinkPort;

  Status = AGESA_SUCCESS;
  LibNbPciIndexRead (pConfig->NbPciAddress.AddressValue | NB_HTIU_INDEX, NB_HTIU_REG15, AccessS3SaveWidth32, (UINT32*)&Scratch, pConfig);
  if (Scratch.MaskMemoryInit == OFF) {
    return  Status;
  }
  Scratch.MaskMemoryInit = OFF;
  LibNbPciIndexWrite (pConfig->NbPciAddress.AddressValue | NB_HTIU_INDEX, NB_HTIU_REG15, AccessS3SaveWidth32, (UINT32*)&Scratch, pConfig);
  CIMX_ASSERT (NbInitMaskedMemoryLength < 300);
  LibAmdMemCopy ((VOID*)ExecutionBuffer, (VOID*) (UINTN)NbInitMaskedMemory, NbInitMaskedMemoryLength, (AMD_CONFIG_PARAMS *)&(pConfig->sHeader));
  PcieSbAgetAlinkIoAddress (&AlinkPort, pConfig);
  (*((MASKED_MEMORY_INIT_PROC)(UINTN)ExecutionBuffer))(pConfig->NbPciAddress.AddressValue, AlinkPort);
  LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_HTIU_INDEX, NB_HTIU_REG87, AccessS3SaveWidth32, 0x0, 0xffffffff, pConfig);
  LibNbPciIndexRMW (pConfig->NbPciAddress.AddressValue | NB_HTIU_INDEX, NB_HTIU_REG88, AccessS3SaveWidth32, 0x0, 0xffffffff, pConfig);
  return Status;
}

