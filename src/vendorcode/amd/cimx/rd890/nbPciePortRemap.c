/**
 * @file
 *
 *  PCIe Port device number remapping.
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

PORT
STATIC
PciePortRemapAllocateDeviceId (
  IN      UINT8           *UnusedPortMap
);
/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Remap PCIe ports device number.
 *
 *
 *
 * @param[in] pConfig   Northbridge configuration structure pointer.
 */

AGESA_STATUS
PciePortRemapInit (
  IN      AMD_NB_CONFIG   *pConfig
  )
{
  AGESA_STATUS  Status;
  PORT          FinalDeviceIdList[MAX_PORT_ID + 1];
  UINT8         UsedDeviceIdMap[MAX_PORT_ID + 1];
  BOOLEAN       IsDeviceRemapEnabled;
  PORT          PortId;
  PCIE_CONFIG   *pPcieConfig;

  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PciePortDeviceNumberRemap Enter \n"));
  pPcieConfig = GET_PCIE_CONFIG_PTR (pConfig);
  Status = AGESA_SUCCESS;
  IsDeviceRemapEnabled = FALSE;
  // Remap Device
  LibAmdMemFill (&UsedDeviceIdMap, 0, sizeof (UsedDeviceIdMap), (AMD_CONFIG_PARAMS *)&(pConfig->sHeader));
  LibAmdMemFill (&FinalDeviceIdList, 0, sizeof (FinalDeviceIdList), (AMD_CONFIG_PARAMS *)&(pConfig->sHeader));
  for (PortId = MIN_PORT_ID; PortId <= MAX_PORT_ID; PortId++) {
    if (PcieLibIsValidPortId (PortId, pConfig)) {
      PORT  NativePortId = PcieLibNativePortId (PortId, pConfig);
      if (pPcieConfig->PortConfiguration[PortId].PortPresent) {
        //FinalDeviceIdList[PortId] = PortId;
        if (pPcieConfig->ExtPortConfiguration[PortId].PortMapping != 0) {
          if (pPcieConfig->ExtPortConfiguration[PortId].PortMapping < MIN_PORT_ID ||
              pPcieConfig->ExtPortConfiguration[PortId].PortMapping > MAX_PORT_ID ||
              pPcieConfig->ExtPortConfiguration[PortId].PortMapping == 8) {
            return AGESA_ERROR;
          }
          FinalDeviceIdList[NativePortId] = pPcieConfig->ExtPortConfiguration[PortId].PortMapping;
          IsDeviceRemapEnabled = TRUE;
        } else {
          FinalDeviceIdList[NativePortId] = PortId;
        }
        CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "   Requested Port Mapping %d -> %d\n", PortId, FinalDeviceIdList[PortId]));
        if (UsedDeviceIdMap[FinalDeviceIdList[NativePortId]] == 0 ) {
          UsedDeviceIdMap[FinalDeviceIdList[NativePortId]] = 1;
        } else {
          return AGESA_ERROR;
        }
      }
    }
  }
  if (!IsDeviceRemapEnabled) {
    return Status;
  }
  for (PortId = MIN_PORT_ID; PortId <= MAX_PORT_ID; PortId++) {
    PORT_STATIC_INFO *pStaticPortInfo;
    pStaticPortInfo = PcieLibGetStaticPortInfo (PortId, pConfig);
    if (pStaticPortInfo->MappingAddress == 0xFF) {
      continue;
    }
    if (FinalDeviceIdList[PortId] == 0) {
      FinalDeviceIdList[PortId] = PciePortRemapAllocateDeviceId (&UsedDeviceIdMap[0]);
    }
    CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "   Port Mapping %d -> %d\n", PortId, FinalDeviceIdList[PortId]));
    LibNbPciIndexRMW (
      NB_SBDFO | NB_MISC_INDEX,
      (PortId > 9)?NB_MISC_REG21:NB_MISC_REG20,
      AccessWidth32,
      0xffffffff,
      FinalDeviceIdList [PortId] << pStaticPortInfo->MappingAddress,
      pConfig
      );

  }
  LibNbPciIndexRMW (NB_SBDFO | NB_MISC_INDEX, NB_MISC_REG20, AccessWidth32, 0xffffffff, 0x3, pConfig);
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PciePortDeviceNumberRemap Exit [0x%x] \n", Status));
  return Status;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Allocate Device number from unused port array.
 *
 *
 *
 * @param[in] UnusedPortMap   Unused port array.
 */

PORT
STATIC
PciePortRemapAllocateDeviceId (
  IN      UINT8           *UnusedPortMap
  )
{
  PORT  PortId;
  for (PortId = MIN_PORT_ID; PortId <= MAX_PORT_ID; PortId++) {
    if (UnusedPortMap[PortId] == 0) {
      UnusedPortMap[PortId] = 1;
      break;
    }
  }
  return PortId;
}
