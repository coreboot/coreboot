/* $NoKeywords:$ */
/**
 * @file
 *
 * Cable safe module
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e $Revision: $   @e $Date: $
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
#include  "AGESA.h"
#include  "amdlib.h"
#include  "Ids.h"
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  GNB_MODULE_DEFINITIONS (GnbPcieConfig)
#include  "GnbRegistersON.h"
#include  "cpuFamilyTranslation.h"
#include  "NbSmuLib.h"
#include  "GnbCableSafeDefs.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBCABLESAFE_GNBCABLESAFE_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


UINT8 HdpIndexTranslationTable [] = {
  3, 2, 1, 0, 7, 6
};

UINT8 AuxIndexTranslationTable [] = {
  5, 4, 11, 10, 9, 8
};

UINT8 AuxDataTranslationTable [] = {
  0x10, 0x20, 0x40, 0x01, 0x02, 0x04
};

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

VOID
STATIC
GnbCableSafeGetConnectorInfoArrayCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  );

BOOLEAN
GnbCableSafeIsSupported (
  IN      AMD_CONFIG_PARAMS   *StdHeader
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Cable Safe module entry
 *
 *
 *
 * @param[in] StdHeader           Standard configuration header
 * @retval    AGESA_STATUS
 */

AGESA_STATUS
GnbCableSafeEntry (
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  AGESA_STATUS          Status;
  PCIe_PLATFORM_CONFIG  *Pcie;
  PCIe_ENGINE_CONFIG    *DdiEngineList [MaxHdp];
  UINT8                 HdpIndex;
  UINT8                 CurrentIndex;
  GNB_CABLE_SAFE_DATA   CableSafeData;
  IDS_HDT_CONSOLE (GNB_TRACE, "GnbCableSafeEntry Enter\n");
  Status = AGESA_SUCCESS;
  if (GnbCableSafeIsSupported (StdHeader)) {
    if (PcieLocateConfigurationData (StdHeader, &Pcie) == AGESA_SUCCESS) {
      for (HdpIndex = 0; HdpIndex < MaxHdp; HdpIndex++) {
        DdiEngineList[HdpIndex] = NULL;
      }
      LibAmdMemFill (&CableSafeData, 0, sizeof (CableSafeData), StdHeader);
      PcieConfigRunProcForAllEngines (
        DESCRIPTOR_ALLOCATED | DESCRIPTOR_DDI_ENGINE,
        GnbCableSafeGetConnectorInfoArrayCallback,
        DdiEngineList,
        Pcie
        );
      CurrentIndex = 0;
      for (HdpIndex = 0; HdpIndex < MaxHdp; HdpIndex++) {
        if (DdiEngineList [HdpIndex] != NULL) {
          CableSafeData.Data[HdpIndexTranslationTable[CurrentIndex]] = HdpIndex + 1;
          CableSafeData.Data[AuxIndexTranslationTable[CurrentIndex]] = AuxDataTranslationTable [(DdiEngineList [HdpIndex])->Type.Ddi.DdiData.AuxIndex];
          IDS_HDT_CONSOLE (NB_MISC, "  Index [%d] HDP 0x%02x AUX 0x%02x\n", CurrentIndex, HdpIndex, (DdiEngineList [HdpIndex])->Type.Ddi.DdiData.AuxIndex);
          CurrentIndex++;
        }
      }
      CableSafeData.Config.Enable = 0x1;
      CableSafeData.Config.DebounceFilter = 0x2;
      CableSafeData.Config.SoftPeriod = 0x4;
      CableSafeData.Config.Unit = 0x1;
      CableSafeData.Config.Period = 0xf424;
      NbSmuRcuRegisterWrite (
        SMUx0B_x85D0_ADDRESS,
        (UINT32*) &CableSafeData,
        sizeof (CableSafeData) / sizeof (UINT32),
        TRUE,
        StdHeader
        );
      NbSmuServiceRequest (0x05, TRUE, StdHeader);
    } else {
      Status = AGESA_ERROR;
    }
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "GnbCableSafeEntry Exit [Status = 0x%04x]\n", Status);
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Callback to init max port Gen capability
 *
 *
 *
 *
 * @param[in]       Engine          Pointer to engine config descriptor
 * @param[in, out]  Buffer          Not used
 * @param[in]       Pcie            Pointer to global PCIe configuration
 *
 */

VOID
STATIC
GnbCableSafeGetConnectorInfoArrayCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PCIe_ENGINE_CONFIG    **EngineList;
  EngineList = (PCIe_ENGINE_CONFIG**) Buffer;
  EngineList [Engine->Type.Ddi.DdiData.HdpIndex] = Engine;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Check if feature supported
 *
 *  Module requre for LN B0 and above
 *
 *
 * @param[in] StdHeader           Standard configuration header
 * @retval    TRUE                 Cable safe needs to be enabled
 */

BOOLEAN
GnbCableSafeIsSupported (
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  BOOLEAN                       Result;
  CPU_LOGICAL_ID                LogicalId;
  SMU_FIRMWARE_REV              FirmwareRev;
  Result = FALSE;
  GetLogicalIdOfCurrentCore (&LogicalId, StdHeader);
  FirmwareRev = NbSmuFirmwareRevision (StdHeader);
  if (SMI_FIRMWARE_REVISION (FirmwareRev) >= 0x010904 && LogicalId.Revision > AMD_F12_LN_A1) {
    Result = TRUE;
  }
  return Result;
}
