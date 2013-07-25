/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe ALIB
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 87263 $   @e \$Date: 2013-01-31 09:18:06 -0600 (Thu, 31 Jan 2013) $
 *
 */
/*
*****************************************************************************
*
 * Copyright (c) 2008 - 2013, Advanced Micro Devices, Inc.
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
#include  "heapManager.h"
#include  "cpuLateInit.h"
#include  "cpuRegisters.h"
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "GnbPcieFamServices.h"
#include  "GnbCommonLib.h"
#include  "GnbPcieConfig.h"
#include  "GnbPcieInitLibV1.h"
#include  "GnbNbInitLibV1.h"
#include  "OptionGnb.h"
#include  "PcieAlibV2.h"
#include  "GnbF1Table.h"
#include  "Filecode.h"

#define FILECODE PROC_GNB_MODULES_GNBPCIEALIBV2_PCIEALIBV2_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

extern  F_ALIB_GET    *AlibGetBaseTableV2;
extern  F_ALIB_UPDATE *AlibDispatchTableV2[];
extern  GNB_BUILD_OPTIONS ROMDATA GnbBuildOptions;


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */


AGESA_STATUS
PcieAlibUpdateGnbData (
  IN OUT   VOID                  *SsdtBuffer,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  );

VOID
STATIC
PcieAlibUpdatePciePortDataCallback (
  IN       PCIe_ENGINE_CONFIG     *Engine,
  IN OUT   VOID                   *Buffer,
  IN       PCIe_PLATFORM_CONFIG   *Pcie
  );

AGESA_STATUS
PcieAlibBuildAcpiTableV2 (
  IN       AMD_CONFIG_PARAMS      *StdHeader,
     OUT   VOID                   **AlibSsdtPtr
  );

AGESA_STATUS
PcieAlibUpdateVoltageData (
  IN OUT   VOID                  *DataBuffer,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  );

AGESA_STATUS
PcieAlibUpdatePcieData (
  IN OUT   VOID                  *DataBuffer,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Create ACPI ALIB SSDT table
 *
 *
 *
 * @param[in] StdHeader           Standard configuration header
 * @retval    AGESA_STATUS
 */

AGESA_STATUS
PcieAlibV2Feature (
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{
  AMD_LATE_PARAMS *LateParamsPtr;
  LateParamsPtr = (AMD_LATE_PARAMS*) StdHeader;
  return PcieAlibBuildAcpiTableV2 (StdHeader, &LateParamsPtr->AcpiAlib);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Build ALIB ACPI table
 *
 *
 *
 * @param[in]     StdHeader       Standard Configuration Header
 * @param[in,out] AlibSsdtPtr     Pointer to pointer to ALIB SSDT table
 * @retval        AGESA_SUCCESS
 * @retval        AGESA_ERROR
 */

AGESA_STATUS
PcieAlibBuildAcpiTableV2 (
  IN       AMD_CONFIG_PARAMS    *StdHeader,
     OUT   VOID                 **AlibSsdtPtr
  )
{
  AGESA_STATUS            Status;
  AGESA_STATUS            AgesaStatus;
  UINTN                   Index;
  VOID                    *AlibSsdtBuffer;
  VOID                    *AlibSsdtTable;
  UINTN                   AlibSsdtlength;
  UINT32                  AmlObjName;
  VOID                    *AmlObjPtr;


  IDS_HDT_CONSOLE (GNB_TRACE, "PcieAlibBuildAcpiTableV2 Enter\n");
  AgesaStatus = AGESA_SUCCESS;
  AlibSsdtTable = AlibGetBaseTableV2 (StdHeader);
  AlibSsdtlength = ((ACPI_TABLE_HEADER*) AlibSsdtTable)->TableLength;
  if (*AlibSsdtPtr == NULL) {
    AlibSsdtBuffer = GnbAllocateHeapBuffer (
                       AMD_ACPI_ALIB_BUFFER_HANDLE,
                       AlibSsdtlength,
                       StdHeader
                       );
    ASSERT (AlibSsdtBuffer != NULL);
    if (AlibSsdtBuffer == NULL) {
      return  AGESA_ERROR;
    }
    *AlibSsdtPtr = AlibSsdtBuffer;
  } else {
    AlibSsdtBuffer = *AlibSsdtPtr;
  }
  // Check length of port data
  ASSERT (sizeof (_ALIB_PORT_DATA) <= 20);
  // Check length of global data
  ASSERT (sizeof (_ALIB_GLOBAL_DATA) <= 32);
  // Copy template to buffer
  LibAmdMemCopy (AlibSsdtBuffer, AlibSsdtTable, AlibSsdtlength, StdHeader);
  // Update table OEM fields.
  LibAmdMemCopy (
    (VOID *) &((ACPI_TABLE_HEADER*) AlibSsdtBuffer)->OemId,
    (VOID *) &GnbBuildOptions.OemIdString,
    sizeof (GnbBuildOptions.OemIdString),
    StdHeader);
  LibAmdMemCopy (
    (VOID *) &((ACPI_TABLE_HEADER*) AlibSsdtBuffer)->OemTableId,
    (VOID *) &GnbBuildOptions.OemTableIdString,
    sizeof (GnbBuildOptions.OemTableIdString),
    StdHeader);
  //
  // Update register base base
  //
  PcieAlibUpdateGnbData (AlibSsdtBuffer, StdHeader);
  //
  // Update transfer block
  //
  AmlObjName = STRING_TO_UINT32 ('A', 'D', 'A', 'T');
  AmlObjPtr = GnbLibFind (AlibSsdtBuffer, AlibSsdtlength, (UINT8*) &AmlObjName, sizeof (AmlObjName));
  if (AmlObjPtr != NULL) {
    AmlObjPtr = (UINT8 *) AmlObjPtr + 10;
  }
  // Dispatch function from table
  Index = 0;
  while (AlibDispatchTableV2[Index] != NULL) {
    Status = AlibDispatchTableV2[Index] (AmlObjPtr, StdHeader);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    Index++;
  }
  if (AgesaStatus != AGESA_SUCCESS) {
    //Shrink table length to size of the header
    ((ACPI_TABLE_HEADER*) AlibSsdtBuffer)->TableLength = sizeof (ACPI_TABLE_HEADER);
  }
  ChecksumAcpiTable ((ACPI_TABLE_HEADER*) AlibSsdtBuffer, StdHeader);
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieAlibBuildAcpiTableV2 Exit [0x%x]\n", AgesaStatus);
  return AgesaStatus;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Update MMIO info
 *
 *
 *
 *
 * @param[in] SsdtBuffer          Pointer to SSDT table
 * @param[in] StdHeader           Standard configuration header
 */

AGESA_STATUS
PcieAlibUpdateGnbData (
  IN OUT   VOID                  *SsdtBuffer,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINT64        LocalMsrRegister;
  UINT32        AmlObjName;
  VOID          *AmlObjPtr;

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieAlibUpdateGnbData Enter\n");
  //
  //  Locate Base address variable
  //
  AmlObjName = STRING_TO_UINT32 ('A', 'G', 'R', 'B');
  AmlObjPtr = GnbLibFind (
                SsdtBuffer,
                ((ACPI_TABLE_HEADER*) SsdtBuffer)->TableLength,
                (UINT8*) &AmlObjName,
                sizeof (AmlObjName)
                );
  /// @todo
  // ASSERT (AmlObjPtr != NULL);
  if (AmlObjPtr == NULL) {
    return AGESA_ERROR;
  }
  //
  //  Update PCIe MMIO base
  //
  LibAmdMsrRead (MSR_MMIO_Cfg_Base, &LocalMsrRegister, StdHeader);
  if ((LocalMsrRegister & BIT0) != 0 && (LocalMsrRegister & 0xFFFFFFFF00000000) == 0) {
    *(UINT32*)((UINT8*) AmlObjPtr + 5) = (UINT32) (LocalMsrRegister & 0xFFFFF00000);
  } else {
    ASSERT (FALSE);
    return AGESA_ERROR;
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieAlibUpdateGnbData Exit\n");
  return AGESA_SUCCESS;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Update MMIO info
 *
 *
 *
 *
 * @param[in] DataBuffer          Pointer to data buffer
 * @param[in] StdHeader           Standard configuration header
 */

AGESA_STATUS
PcieAlibUpdateVoltageData (
  IN OUT   VOID                  *DataBuffer,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  PP_F1_ARRAY_V2   *PpF1Array;
  AGESA_STATUS    Status;
  ALIB_DATA       *AlibData;

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieAlibUpdateVoltageData Enter\n");
  Status = AGESA_SUCCESS;
  ASSERT (DataBuffer != NULL);
  AlibData = (ALIB_DATA *) DataBuffer;
  //
  //  Locate F1 table
  //
  PpF1Array = GnbLocateHeapBuffer (AMD_PP_F1_TABLE_HANDLE, StdHeader);
  ASSERT (PpF1Array != NULL);
  if (PpF1Array != NULL) {
    //
    //  Update GEN1 Vid
    //
    AlibData->Data.Data.PcieVidGen1 = PpF1Array->PP_FUSE_ARRAY_V2_fld32[0];
    //
    //  Update GEN2 Vid
    //
    AlibData->Data.Data.PcieVidGen2 = PpF1Array->PP_FUSE_ARRAY_V2_fld32[PpF1Array->PcieGen2Vid];
    //
    //  Update DPM Mask
    //
    AlibData->Data.Data.DpmMask = (PpF1Array->PP_FUSE_ARRAY_V2_fld37 + 1);
    IDS_HDT_CONSOLE (GNB_TRACE, "  DpmMask = %02x\n", AlibData->Data.Data.DpmMask);
    //
    //  Update Boost data
    //
    AlibData->Data.Data.NumBoostStates = (PpF1Array->PP_FUSE_ARRAY_V2_fld36);
    IDS_HDT_CONSOLE (GNB_TRACE, "  NumBoost = %02x\n", AlibData->Data.Data.NumBoostStates);
  } else {
    Status = AGESA_ERROR;
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieAlibUpdateVoltageData Exit\n");
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Update PCIe info
 *
 *
 *
 *
 * @param[in] DataBuffer          Ponter to data buffer
 * @param[in] StdHeader           Standard configuration header
 */

AGESA_STATUS
PcieAlibUpdatePcieData (
  IN OUT   VOID                  *DataBuffer,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  PCIe_PLATFORM_CONFIG  *Pcie;
  AMD_LATE_PARAMS *LateParamsPtr;
  AGESA_STATUS          Status;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieAlibUpdatePcieData Enter\n");
  ASSERT (DataBuffer != NULL);
  Status = AGESA_SUCCESS;
  //
  //  Locate PCIe platform config
  //
  if (PcieLocateConfigurationData (StdHeader, &Pcie) == AGESA_SUCCESS) {
    //
    // Update policy data
    //
    ((ALIB_DATA *) DataBuffer)->Data.Data.PsppPolicy = Pcie->PsppPolicy;
    //
    // Update data for each port
    //
    PcieConfigRunProcForAllEngines (
      DESCRIPTOR_PCIE_ENGINE,
      PcieAlibUpdatePciePortDataCallback,
      DataBuffer,
      Pcie
      );
  } else {
    ASSERT (FALSE);
    Status = AGESA_FATAL;
  }

  LateParamsPtr = (AMD_LATE_PARAMS *) StdHeader;
  ((ALIB_DATA *) DataBuffer)->Data.Data.DockedTdpHeadroom =
      LateParamsPtr->GnbLateConfiguration.DockedTdpHeadroom;
  IDS_HDT_CONSOLE (GNB_TRACE, "  DockedTdpHeadroom = %02x\n",
      LateParamsPtr->GnbLateConfiguration.DockedTdpHeadroom);

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieAlibUpdatePcieData Exit\n");
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Callback to update PCIe port data
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
PcieAlibUpdatePciePortDataCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  _ALIB_PORT_DATA *PortData;

  PortData = &((ALIB_DATA *) Buffer)->PortData[Engine->Type.Port.PcieBridgeId].PortData;

  if (PcieConfigIsEngineAllocated (Engine) && (Engine->Type.Port.PortData.LinkHotplug != HotplugDisabled || PcieConfigCheckPortStatus (Engine, INIT_STATUS_PCIE_TRAINING_SUCCESS))) {
    //
    // Various speed capability
    //
    PortData->PciePortMaxSpeed = (UINT8) PcieFmGetLinkSpeedCap (PCIE_PORT_GEN_CAP_MAX, Engine);
    PortData->PciePortCurSpeed = (UINT8) PcieFmGetLinkSpeedCap (PCIE_PORT_GEN_CAP_BOOT, Engine);
    PortData->PciePortDcSpeed = PcieGen1;
    PortData->PciePortAcSpeed = PortData->PciePortMaxSpeed;
    if (Pcie->PsppPolicy == PsppBalanceLow) {
      PortData->PciePortAcSpeed = PcieGen1;
    }
    if (PcieConfigIsSbPcieEngine (Engine)) {
      PortData->PcieSbPort = 0x1;
      PortData->PciePortAcSpeed = PortData->PciePortMaxSpeed;
    }
    if (Engine->Type.Port.PortData.MiscControls.LinkSafeMode != 0) {
      PortData->PcieLinkSafeMode = 0x1;
      PortData->PcieLocalOverrideSpeed = Engine->Type.Port.PortData.MiscControls.LinkSafeMode;
    }
    //
    // various port capability
    //
    PortData->StartPhyLane = (UINT8) Engine->EngineData.StartLane;
    PortData->EndPhyLane = (UINT8) Engine->EngineData.EndLane;
    PortData->StartCoreLane = (UINT8) Engine->Type.Port.StartCoreLane;
    PortData->EndCoreLane = (UINT8) Engine->Type.Port.EndCoreLane;
    PortData->PortId = Engine->Type.Port.PortId;
    PortData->LinkHotplug = Engine->Type.Port.PortData.LinkHotplug;
    PortData->PciDev = (UINT8) Engine->Type.Port.Address.Address.Device;
    PortData->PciFun = (UINT8) Engine->Type.Port.Address.Address.Function;
  } else {
    PortData->PciePortMaxSpeed = PcieGen1;
    PortData->PciePortCurSpeed = PcieGen1;
    PortData->PciePortDcSpeed  = PcieGen1;
    PortData->PciePortAcSpeed  = PcieGen1;
    PortData->PcieLocalOverrideSpeed = PcieGen1;
  }
}

