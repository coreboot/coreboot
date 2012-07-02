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
#include  "Gnb.h"
#include  "GnbPcieConfig.h"
#include  "GnbFamServices.h"
#include  "GnbCommonLib.h"
#include  "GnbIvrsLib.h"
#include  "OptionGnb.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBIOMMUIVRS_GNBIOMMUIVRS_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern GNB_BUILD_OPTIONS ROMDATA GnbBuildOptions;
/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

#define IVRS_TABLE_LENGTH 8 * 1024

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

AGESA_STATUS
GnbBuildIvmdList (
  IN       IVRS_BLOCK_TYPE            Type,
  IN       VOID                       *Ivrs,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  );

VOID
GnbIommuIvrsTableDump (
  IN       VOID                 *Ivrs,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  );

AGESA_STATUS
GnbIommuIvrsTable (
  IN       AMD_CONFIG_PARAMS    *StdHeader
  );

IOMMU_IVRS_HEADER IvrsHeader = {
  {'I', 'V', 'R', 'S'},
  sizeof (IOMMU_IVRS_HEADER),
  2,
  0,
  {'A', 'M', 'D', ' ', ' ', 0},
  {'A', 'M', 'D', 'I', 'O', 'M', 'M', 'U'},
  1,
  {'A','M','D',' '},
  0,
  0,
  0
};


/*----------------------------------------------------------------------------------------*/
/**
 * Build IVRS table
 *
 *
 *
 * @param[in]     StdHeader       Standard Configuration Header
 * @retval        AGESA_SUCCESS
 * @retval        AGESA_ERROR
 */

AGESA_STATUS
GnbIommuIvrsTable (
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{
  AGESA_STATUS            Status;
  AMD_LATE_PARAMS         *LateParamsPtr;
  VOID                    *Ivrs;
  BOOLEAN                 IvrsSupport;
  GNB_HANDLE              *GnbHandle;

  Status = AGESA_SUCCESS;
  LateParamsPtr = (AMD_LATE_PARAMS*) StdHeader;
  IvrsSupport = FALSE;
  Ivrs = LateParamsPtr->AcpiIvrs;
  if (Ivrs == NULL) {
    Ivrs = GnbAllocateHeapBuffer (
             AMD_ACPI_IVRS_BUFFER_HANDLE,
             IVRS_TABLE_LENGTH,
             StdHeader
             );
    ASSERT (Ivrs != NULL);
    if (Ivrs == NULL) {
      return  AGESA_ERROR;
    }
    LateParamsPtr->AcpiIvrs = Ivrs;
  }
  LibAmdMemFill (Ivrs, 0x0, IVRS_TABLE_LENGTH, StdHeader);
  LibAmdMemCopy (Ivrs, &IvrsHeader, sizeof (IvrsHeader), StdHeader);

  GnbHandle = GnbGetHandle (StdHeader);
  while (GnbHandle != NULL) {
    if (GnbFmCheckIommuPresent (GnbHandle, StdHeader)) {
      IDS_HDT_CONSOLE (GNB_TRACE, "Build IVRS for Socket %d Silicon %d\n", GnbGetSocketId (GnbHandle) , GnbGetSiliconId (GnbHandle));
      IvrsSupport = TRUE;
      GnbFmCreateIvrsEntry (GnbHandle, IvrsIvhdBlock, Ivrs, StdHeader);
      GnbBuildIvmdList (IvrsIvmdBlock, Ivrs, StdHeader);
      if (GnbBuildOptions.IvrsRelativeAddrNamesSupport) {
        GnbFmCreateIvrsEntry (GnbHandle, IvrsIvhdrBlock, Ivrs, StdHeader);
        GnbBuildIvmdList (IvrsIvmdrBlock, Ivrs, StdHeader);
      }
    }
    GnbHandle = GnbGetNextHandle (GnbHandle);
  }
  if (IvrsSupport == TRUE) {
    ChecksumAcpiTable ((ACPI_TABLE_HEADER*) Ivrs, StdHeader);
    GNB_DEBUG_CODE (GnbIommuIvrsTableDump (Ivrs, StdHeader));
  } else {
    IDS_HDT_CONSOLE (GNB_TRACE, "  IVRS table not generated\n");
    LateParamsPtr->AcpiIvrs = NULL;
  }
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Build IVMD list
 *
 *
 * @param[in]  Type            Entry type
 * @param[in]  Ivrs            IVRS table pointer
 * @param[in]  StdHeader       Standard configuration header
 *
 */

AGESA_STATUS
GnbBuildIvmdList (
  IN       IVRS_BLOCK_TYPE            Type,
  IN       VOID                       *Ivrs,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  )
{
  IOMMU_EXCLUSION_RANGE_DESCRIPTOR  *IvrsExclusionRangeList;
  IVRS_IVMD_ENTRY                   *Ivmd;
  UINT16                            StartId;
  UINT16                            EndId;

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbBuildIvmdList Entry\n");
  IvrsExclusionRangeList = ((AMD_LATE_PARAMS*)StdHeader)->IvrsExclusionRangeList;
  if (IvrsExclusionRangeList != NULL) {
    // Process the entire IvrsExclusionRangeList here and create an IVMD for eache entry
    IDS_HDT_CONSOLE (GNB_TRACE, "Process Exclusion Range List\n");
    while ((IvrsExclusionRangeList->Flags & DESCRIPTOR_TERMINATE_LIST) == 0) {
      if ((IvrsExclusionRangeList->Flags & DESCRIPTOR_IGNORE) == 0) {
        // Address of IVMD entry
        Ivmd = (IVRS_IVMD_ENTRY*) ((UINT8 *)Ivrs + ((IOMMU_IVRS_HEADER *) Ivrs)->TableLength);
        StartId =
          (IvrsExclusionRangeList->RequestorIdStart.Bus << 8) +
          (IvrsExclusionRangeList->RequestorIdStart.Device << 3) +
          (IvrsExclusionRangeList->RequestorIdStart.Function);
        EndId =
          (IvrsExclusionRangeList->RequestorIdEnd.Bus << 8) +
          (IvrsExclusionRangeList->RequestorIdEnd.Device << 3) +
          (IvrsExclusionRangeList->RequestorIdEnd.Function);
        GnbIvmdAddEntry (
          Type,
          StartId,
          EndId,
          IvrsExclusionRangeList->RangeBaseAddress,
          IvrsExclusionRangeList->RangeLength,
          Ivmd,
          StdHeader);
        // Add entry size to existing table length
        ((IOMMU_IVRS_HEADER *)Ivrs)->TableLength += sizeof (IVRS_IVMD_ENTRY);
      }
      // Point to next entry in IvrsExclusionRangeList
      IvrsExclusionRangeList++;
    }
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "GnbBuildIvmdList Exit\n");
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Dump IVRS table
 *
 *
 * @param[in]     Ivrs            Pointer to IVRS table
 * @param[in]     StdHeader       Standard Configuration Header
 */

VOID
GnbIommuIvrsTableDump (
  IN       VOID                 *Ivrs,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{
  UINT8   *Block;
  UINT8   *Entry;
  Block = (UINT8 *) Ivrs + sizeof (IOMMU_IVRS_HEADER);
  IDS_HDT_CONSOLE (GNB_TRACE, "<----------  IVRS Table Start -----------> \n");
  IDS_HDT_CONSOLE (GNB_TRACE, "  IVInfo           = 0x%08x\n", ((IOMMU_IVRS_HEADER *) Ivrs)-> IvInfo);
  while (Block < ((UINT8 *) Ivrs + ((IOMMU_IVRS_HEADER *) Ivrs)->TableLength)) {
    if (*Block == IvrsIvhdBlock) {
      IDS_HDT_CONSOLE (GNB_TRACE, "  <-------------IVHD Block Start -------->\n");
      IDS_HDT_CONSOLE (GNB_TRACE, "  Flags            = 0x%02x\n", ((IVRS_IVHD_ENTRY *) Block)->Flags);
      IDS_HDT_CONSOLE (GNB_TRACE, "  DeviceId         = 0x%04x\n", ((IVRS_IVHD_ENTRY *) Block)->DeviceId);
      IDS_HDT_CONSOLE (GNB_TRACE, "  CapabilityOffset = 0x%02x\n", ((IVRS_IVHD_ENTRY *) Block)->CapabilityOffset);
      IDS_HDT_CONSOLE (GNB_TRACE, "  BaseAddress      = 0x%08x%08x\n", (UINT32) (((IVRS_IVHD_ENTRY *) Block)->BaseAddress >> 32), (UINT32) ((IVRS_IVHD_ENTRY *) Block)->BaseAddress);
      IDS_HDT_CONSOLE (GNB_TRACE, "  PCI Segment      = 0x%04x\n", ((IVRS_IVHD_ENTRY *) Block)->PciSegment);
      IDS_HDT_CONSOLE (GNB_TRACE, "  IommuInfo        = 0x%04x\n", ((IVRS_IVHD_ENTRY *) Block)->IommuInfo);
      IDS_HDT_CONSOLE (GNB_TRACE, "  IommuEfr         = 0x%08x\n", ((IVRS_IVHD_ENTRY *) Block)->IommuEfr);
      Entry = Block + sizeof (IVRS_IVHD_ENTRY);
      IDS_HDT_CONSOLE (GNB_TRACE, "  <-------------IVHD Block Device Entries Start -------->\n");
      while (Entry < (Block + ((IVRS_IVHD_ENTRY *) Block)->Length)) {
        IDS_HDT_CONSOLE (GNB_TRACE, "  ");
        switch (*Entry) {
        case IvhdEntrySelect:
        case IvhdEntryEndRange:
          GnbLibDebugDumpBuffer (Entry, 4, 1, 4);
          Entry = Entry + 4;
          break;
        case IvhdEntryStartRange:
          GnbLibDebugDumpBuffer (Entry, 8, 1, 8);
          Entry = Entry + 8;
          break;
        case IvhdEntryAliasStartRange:
          GnbLibDebugDumpBuffer (Entry, 12, 1, 12);
          Entry = Entry + 12;
          break;
        case IvhdEntryAliasSelect:
        case IvhdEntryExtendedSelect:
        case IvhdEntrySpecialDevice:
          GnbLibDebugDumpBuffer (Entry, 8, 1, 8);
          Entry = Entry + 8;
          break;
        case IvhdEntryPadding:
          Entry = Entry + 4;
          break;
        default:
          IDS_HDT_CONSOLE (GNB_TRACE, "  Unsupported entry type [%d]\n");
          ASSERT (FALSE);
        }
      }
      IDS_HDT_CONSOLE (GNB_TRACE, "  <-------------IVHD Block Device Entries End -------->\n");
      IDS_HDT_CONSOLE (GNB_TRACE, "  <-------------IVHD Block End ---------->\n");
      Block = Block + ((IVRS_IVHD_ENTRY *) Block)->Length;
    } else if (
        (*Block == IvrsIvmdBlock) ||
        (*Block == IvrsIvmdBlockRange) ||
        (*Block == IvrsIvmdBlockSingle) ||
        (*Block == IvrsIvmdrBlock) ||
        (*Block == IvrsIvmdrBlockSingle)) {
      IDS_HDT_CONSOLE (GNB_TRACE, "  <-------------IVMD Block Start -------->\n");
      IDS_HDT_CONSOLE (GNB_TRACE, "  Flags            = 0x%02x\n", ((IVRS_IVMD_ENTRY *) Block)->Flags);
      IDS_HDT_CONSOLE (GNB_TRACE, "  DeviceId         = 0x%04x\n", ((IVRS_IVMD_ENTRY *) Block)->DeviceId);
      switch (*Block) {
      case IvrsIvmdBlock:
      case IvrsIvmdrBlock:
        IDS_HDT_CONSOLE (GNB_TRACE, "  Applies to all devices\n");
        break;
      case IvrsIvmdBlockSingle:
      case IvrsIvmdrBlockSingle:
        IDS_HDT_CONSOLE (GNB_TRACE, "  Applies to a single device\n");
        break;
      default:
        IDS_HDT_CONSOLE (GNB_TRACE, "  DeviceId End   = 0x%04x\n", ((IVRS_IVMD_ENTRY *) Block)->AuxiliaryData);
      }
      IDS_HDT_CONSOLE (GNB_TRACE, "  StartAddress     = 0x%08x%08x\n", (UINT32) (((IVRS_IVMD_ENTRY *) Block)->BlockStart >> 32), (UINT32) ((IVRS_IVMD_ENTRY *) Block)->BlockStart);
      IDS_HDT_CONSOLE (GNB_TRACE, "  BockLength       = 0x%08x%08x\n", (UINT32) (((IVRS_IVMD_ENTRY *) Block)->BlockLength >> 32), (UINT32) ((IVRS_IVMD_ENTRY *) Block)->BlockLength);
      IDS_HDT_CONSOLE (GNB_TRACE, "  <-------------IVMD Block End ---------->\n");
      Block = Block + ((IVRS_IVMD_ENTRY *) Block)->Length;
    }
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "<----------  IVRS Table Raw Data --------> \n");
  GnbLibDebugDumpBuffer (Ivrs, ((IOMMU_IVRS_HEADER *) Ivrs)->TableLength, 1, 16);
  IDS_HDT_CONSOLE (GNB_TRACE, "\n");
  IDS_HDT_CONSOLE (GNB_TRACE, "<----------  IVRS Table End -------------> \n");
}

