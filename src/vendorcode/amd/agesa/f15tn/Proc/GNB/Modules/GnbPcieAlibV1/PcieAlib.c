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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
*****************************************************************************
*
* Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
*
* AMD is granting you permission to use this software (the Materials)
* pursuant to the terms and conditions of your Software License Agreement
* with AMD.  This header does *NOT* give you permission to use the Materials
* or any rights under AMD's intellectual property.  Your use of any portion
* of these Materials shall constitute your acceptance of those terms and
* conditions.  If you do not agree to the terms and conditions of the Software
* License Agreement, please do not use any portion of these Materials.
*
* CONFIDENTIALITY:  The Materials and all other information, identified as
* confidential and provided to you by AMD shall be kept confidential in
* accordance with the terms and conditions of the Software License Agreement.
*
* LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
* PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
* MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
* OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
* IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
* (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
* INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
* GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
* RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
* EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
* THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
*
* AMD does not assume any responsibility for any errors which may appear in
* the Materials or any other related information provided to you by AMD, or
* result from use of the Materials or any related information.
*
* You agree that you will not reverse engineer or decompile the Materials.
*
* NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
* further information, software, technical information, know-how, or show-how
* available to you.  Additionally, AMD retains the right to modify the
* Materials at any time, without notice, and is not obligated to provide such
* modified Materials to you.
*
* U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
* "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
* subject to the restrictions as set forth in FAR 52.227-14 and
* DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
* Government constitutes acknowledgement of AMD's proprietary rights in them.
*
* EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
* direct product thereof will be exported directly or indirectly, into any
* country prohibited by the United States Export Administration Act and the
* regulations thereunder, without the required authorization from the U.S.
* government nor will be used for any purpose prohibited by the same.
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
#include  "GnbRegistersLN.h"
#include  "OptionGnb.h"
#include  "PcieAlib.h"
#include  "GnbFuseTable.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBPCIEALIBV1_PCIEALIB_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

extern  F_ALIB_GET    *AlibGetBaseTable;
extern  F_ALIB_UPDATE *AlibDispatchTable[];


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
AGESA_STATUS
PcieAlibUpdatePcieMmioInfo (
  IN OUT   VOID                  *AlibSsdtBuffer,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  );

AGESA_STATUS
PcieAlibUpdateVoltageInfo (
  IN OUT   VOID                  *AlibSsdtBuffer,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  );

AGESA_STATUS
PcieAlibUpdatePcieInfo (
  IN OUT   VOID                  *AlibSsdtBuffer,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  );

VOID
STATIC
PcieAlibSetPortMaxSpeedCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  );

VOID
STATIC
PcieAlibSetPortOverrideSpeedCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  );

VOID
STATIC
PcieAlibSetPortInfoCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  );

AGESA_STATUS
PcieAlibBuildAcpiTable (
  IN       AMD_CONFIG_PARAMS    *StdHeader,
     OUT   VOID                 **AlibSsdtPtr
  );

VOID
STATIC
PcieAlibSetSclkVid (
  IN OUT   VOID                  *Buffer,
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
PcieAlibFeature (
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{
  AMD_LATE_PARAMS *LateParamsPtr;
  LateParamsPtr = (AMD_LATE_PARAMS*) StdHeader;
  return PcieAlibBuildAcpiTable (StdHeader, &LateParamsPtr->AcpiAlib);
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
PcieAlibBuildAcpiTable (
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

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieAlibBuildAcpiTable Enter\n");
  AgesaStatus = AGESA_SUCCESS;
  AlibSsdtTable = AlibGetBaseTable (StdHeader);
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
  // Copy template to buffer
  LibAmdMemCopy (AlibSsdtBuffer, AlibSsdtTable, AlibSsdtlength, StdHeader);
  // Disaptch fucntion form table
  Index = 0;
  while (AlibDispatchTable[Index] != NULL) {
    Status = AlibDispatchTable[Index] (AlibSsdtBuffer, StdHeader);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    Index++;
  }
  if (AgesaStatus != AGESA_SUCCESS) {
    //Shrink table length to size of the header
    ((ACPI_TABLE_HEADER*) AlibSsdtBuffer)->TableLength = sizeof (ACPI_TABLE_HEADER);
  }
  ChecksumAcpiTable ((ACPI_TABLE_HEADER*) AlibSsdtBuffer, StdHeader);
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieAlibBuildAcpiTable Exit [0x%x]\n", AgesaStatus);
  return AgesaStatus;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Update MMIO info
 *
 *
 *
 *
 * @param[in] AlibSsdtBuffer      Ponter to SSDT table
 * @param[in] StdHeader           Standard configuration header
 */

AGESA_STATUS
PcieAlibUpdatePcieMmioInfo (
  IN OUT   VOID                  *AlibSsdtBuffer,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINT32        AmlObjName;
  UINT32        AlibSsdtlength;
  VOID          *AmlObjPtr;
  AGESA_STATUS  Status;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieAlibUpdatePcieMmioInfo Enter\n");
  Status = AGESA_SUCCESS;
  AlibSsdtlength = ((ACPI_TABLE_HEADER*) AlibSsdtBuffer)->TableLength;
  AmlObjName = STRING_TO_UINT32 ('A', 'D', '0', '1');
  AmlObjPtr = GnbLibFind (AlibSsdtBuffer, AlibSsdtlength, (UINT8*) &AmlObjName, sizeof (AmlObjName));
  ASSERT (AmlObjPtr != NULL);
  if (AmlObjPtr != NULL) {
    UINT64  LocalMsrRegister;
    LibAmdMsrRead (MSR_MMIO_Cfg_Base, &LocalMsrRegister, StdHeader);
    if ((LocalMsrRegister & BIT0) != 0 && (LocalMsrRegister & 0xFFFFFFFF00000000) == 0) {
      *(UINT32*)((UINT8*) AmlObjPtr + 5) = (UINT32)(LocalMsrRegister & 0xFFFFF00000);
    } else {
      Status = AGESA_FATAL;
    }
  } else {
    Status = AGESA_FATAL;
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieAlibUpdatePcieMmioInfo Exit\n");
  return Status;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Update MMIO info
 *
 *
 *
 *
 * @param[in] AlibSsdtBuffer      Ponter to SSDT table
 * @param[in] StdHeader           Standard configuration header
 */

AGESA_STATUS
PcieAlibUpdateVoltageInfo (
  IN OUT   VOID                  *AlibSsdtBuffer,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINT32          AmlObjName;
  UINT32          AlibSsdtlength;
  VOID            *AmlObjPtr;
  UINT8           BootUpVidIndex;
  UINT8           Gen1VidIndex;
  PP_FUSE_ARRAY   *PpFuseArray;
  AGESA_STATUS    Status;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieAlibUpdateVoltageInfo Enter\n");
  Status = AGESA_SUCCESS;
  AlibSsdtlength = ((ACPI_TABLE_HEADER*) AlibSsdtBuffer)->TableLength;
  PpFuseArray = GnbLocateHeapBuffer (AMD_PP_FUSE_TABLE_HANDLE, StdHeader);
  ASSERT (PpFuseArray != NULL);
  if (PpFuseArray != NULL) {
    AmlObjName = STRING_TO_UINT32 ('A', 'D', '0', '3');
    AmlObjPtr = GnbLibFind (AlibSsdtBuffer, AlibSsdtlength, (UINT8*) &AmlObjName, sizeof (AmlObjName));
    ASSERT (AmlObjPtr != NULL);
    if (AmlObjPtr != NULL) {
      *(UINT8*)((UINT8*) AmlObjPtr + 5) = PpFuseArray->PcieGen2Vid;
    } else {
      Status = AGESA_FATAL;
    }
  } else {
    Status = AGESA_FATAL;
  }

  Gen1VidIndex = GnbLocateLowestVidIndex (StdHeader);
  AmlObjName = STRING_TO_UINT32 ('A', 'D', '0', '4');
  AmlObjPtr = GnbLibFind (AlibSsdtBuffer, AlibSsdtlength, (UINT8*) &AmlObjName, sizeof (AmlObjName));
  ASSERT (AmlObjPtr != NULL);
  if (AmlObjPtr != NULL) {
    *(UINT8*)((UINT8*) AmlObjPtr + 5) = Gen1VidIndex;
  } else {
    Status = AGESA_FATAL;
  }

  BootUpVidIndex = GnbLocateHighestVidIndex (StdHeader);
  AmlObjName = STRING_TO_UINT32 ('A', 'D', '0', '5');
  AmlObjPtr = GnbLibFind (AlibSsdtBuffer, AlibSsdtlength, (UINT8*) &AmlObjName, sizeof (AmlObjName));
  ASSERT (AmlObjPtr != NULL);
  if (AmlObjPtr != NULL) {
    *(UINT8*)((UINT8*) AmlObjPtr + 5) = BootUpVidIndex;
  } else {
    Status = AGESA_FATAL;
  }

  AmlObjName = STRING_TO_UINT32 ('A', 'D', '1', '0');
  AmlObjPtr = GnbLibFind (AlibSsdtBuffer, AlibSsdtlength, (UINT8*) &AmlObjName, sizeof (AmlObjName));
  ASSERT (AmlObjPtr != NULL);
  if (AmlObjPtr != NULL) {
    PcieAlibSetSclkVid ((UINT8*) ((UINT8*)AmlObjPtr + 7), StdHeader);
  } else {
    Status = AGESA_FATAL;
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieAlibUpdateVoltageInfo Exit\n");
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Update PCIe info
 *
 *
 *
 *
 * @param[in] AlibSsdtBuffer      Ponter to SSDT table
 * @param[in] StdHeader           Standard configuration header
 */

AGESA_STATUS
PcieAlibUpdatePcieInfo (
  IN OUT   VOID                  *AlibSsdtBuffer,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  PCIe_PLATFORM_CONFIG  *Pcie;
  UINT32                AmlObjName;
  UINT32                AlibSsdtlength;
  VOID                  *AmlObjPtr;
  AGESA_STATUS          Status;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieAlibUpdatePcieInfo Enter\n");
  Status = AGESA_SUCCESS;
  AlibSsdtlength = ((ACPI_TABLE_HEADER*) AlibSsdtBuffer)->TableLength;
  if (PcieLocateConfigurationData (StdHeader, &Pcie) == AGESA_SUCCESS) {
    AmlObjName = STRING_TO_UINT32 ('A', 'D', '0', '2');
    AmlObjPtr = GnbLibFind (AlibSsdtBuffer, AlibSsdtlength, (UINT8*) &AmlObjName, sizeof (AmlObjName));
    ASSERT (AmlObjPtr != NULL);
    if (AmlObjPtr != NULL) {
      *(UINT8*)((UINT8*) AmlObjPtr + 5) = Pcie->PsppPolicy;
    } else {
      Status = AGESA_FATAL;
    }
    AmlObjName = STRING_TO_UINT32 ('A', 'D', '0', '6');
    AmlObjPtr = GnbLibFind (AlibSsdtBuffer, AlibSsdtlength, (UINT8*) &AmlObjName, sizeof (AmlObjName));
    ASSERT (AmlObjPtr != NULL);
    if (AmlObjPtr != NULL) {
      PcieConfigRunProcForAllEngines (
        DESCRIPTOR_ALLOCATED | DESCRIPTOR_PCIE_ENGINE,
        PcieAlibSetPortMaxSpeedCallback,
        (UINT8*)((UINT8*) AmlObjPtr + 7),
        Pcie
        );
    } else {
      Status = AGESA_FATAL;
    }
    AmlObjName = STRING_TO_UINT32 ('A', 'D', '0', '8');
    AmlObjPtr = GnbLibFind (AlibSsdtBuffer, AlibSsdtlength, (UINT8*) &AmlObjName, sizeof (AmlObjName));
    ASSERT (AmlObjPtr != NULL);
    if (AmlObjPtr != NULL) {
      PcieConfigRunProcForAllEngines (
        DESCRIPTOR_ALLOCATED | DESCRIPTOR_PCIE_ENGINE,
        PcieAlibSetPortOverrideSpeedCallback,
        (UINT8*)((UINT8*) AmlObjPtr + 7),
        Pcie
        );
    } else {
      Status = AGESA_FATAL;
    }
    AmlObjName = STRING_TO_UINT32 ('A', 'D', '0', '7');
    AmlObjPtr = GnbLibFind (AlibSsdtBuffer, AlibSsdtlength, (UINT8*) &AmlObjName, sizeof (AmlObjName));
    ASSERT (AmlObjPtr != NULL);
    if (AmlObjPtr != NULL) {
      PcieConfigRunProcForAllEngines (
        DESCRIPTOR_ALLOCATED | DESCRIPTOR_PCIE_ENGINE,
        PcieAlibSetPortInfoCallback,
        (UINT8*)((UINT8*) AmlObjPtr + 4),
        Pcie
        );
    } else {
      Status = AGESA_FATAL;
    }
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieAlibUpdatePcieInfo Exit\n");
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Callback to init max port speed capability
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
PcieAlibSetPortMaxSpeedCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT8   *PsppMaxPortSpeedPackage;
  PsppMaxPortSpeedPackage = (UINT8*) Buffer;
  if (Engine->Type.Port.PortData.LinkHotplug != HotplugDisabled || PcieConfigCheckPortStatus (Engine, INIT_STATUS_PCIE_TRAINING_SUCCESS)) {
    PsppMaxPortSpeedPackage[(Engine->Type.Port.Address.Address.Device - 2) * 2 + 1] = (UINT8) PcieFmGetLinkSpeedCap (PCIE_PORT_GEN_CAP_MAX, Engine);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Callback to init max port speed capability
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
PcieAlibSetPortOverrideSpeedCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT8   *PsppOverridePortSpeedPackage;
  PsppOverridePortSpeedPackage = (UINT8*) Buffer;
  if (Engine->Type.Port.PortData.LinkHotplug != HotplugDisabled || PcieConfigCheckPortStatus (Engine, INIT_STATUS_PCIE_TRAINING_SUCCESS)) {
    PsppOverridePortSpeedPackage[(Engine->Type.Port.Address.Address.Device - 2) * 2 + 1] = Engine->Type.Port.PortData.MiscControls.LinkSafeMode;
  }
  if (Engine->Type.Port.PortData.LinkHotplug == HotplugBasic && !PcieConfigCheckPortStatus (Engine, INIT_STATUS_PCIE_TRAINING_SUCCESS)) {
    PsppOverridePortSpeedPackage[(Engine->Type.Port.Address.Address.Device - 2) * 2 + 1] = PcieGen1;
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Callback to init port info
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
PcieAlibSetPortInfoCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  ALIB_PORT_INFO_PACKAGE  *PortInfoPackage;
  UINT8                   PortIndex;
  PortInfoPackage = (ALIB_PORT_INFO_PACKAGE*) Buffer;
  PortIndex = (UINT8) Engine->Type.Port.Address.Address.Device - 2;
  PortInfoPackage->PortInfo[PortIndex].StartPhyLane = (UINT8) Engine->EngineData.StartLane;
  PortInfoPackage->PortInfo[PortIndex].EndPhyLane = (UINT8) Engine->EngineData.EndLane;
  PortInfoPackage->PortInfo[PortIndex].StartCoreLane = (UINT8) Engine->Type.Port.StartCoreLane;
  PortInfoPackage->PortInfo[PortIndex].EndCoreLane = (UINT8) Engine->Type.Port.EndCoreLane;
  PortInfoPackage->PortInfo[PortIndex].PortId = Engine->Type.Port.PortId;
  PortInfoPackage->PortInfo[PortIndex].WrapperId = 0x0130 | (PcieConfigGetParentWrapper (Engine)->WrapId);
  PortInfoPackage->PortInfo[PortIndex].LinkHotplug = Engine->Type.Port.PortData.LinkHotplug;
  PortInfoPackage->PortInfo[PortIndex].MaxSpeedCap = (UINT8) PcieFmGetLinkSpeedCap (PCIE_PORT_GEN_CAP_MAX, Engine);
  PortInfoPackage->PortInfo[PortIndex].ClkPmSupport = Engine->Type.Port.PortData.MiscControls.ClkPmSupport;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Callback to init port info
 *
 *
 *
 *
 * @param[in, out]  Buffer          Asl buffer
 * @param[in]       StdHeader       Standard configuration header
 *
 */

VOID
STATIC
PcieAlibSetSclkVid (
  IN OUT   VOID                  *Buffer,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINT8             *SclkVid;
  PP_FUSE_ARRAY     *PpFuseArray;
  UINT8             Index;

  SclkVid = (UINT8*) Buffer;
  PpFuseArray = (PP_FUSE_ARRAY *) GnbLocateHeapBuffer (AMD_PP_FUSE_TABLE_HANDLE, StdHeader);
  ASSERT (PpFuseArray != NULL);
  if (PpFuseArray == NULL) {
    IDS_HDT_CONSOLE (GNB_TRACE, "  ERROR!!! Heap Location\n");
    return;
  }

  for (Index = 0; Index < 4; Index++) {
    SclkVid[Index * 2 + 1] = PpFuseArray->SclkVid[Index];
  }
}
