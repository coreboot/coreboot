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
 * @e \$Revision: 39433 $   @e \$Date: 2010-10-11 14:15:36 -0700 (Mon, 11 Oct 2010) $
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
#include  "GnbFamServices.h"
#include  "GnbCommonLib.h"
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

VOID
GnbIommuIvrsTableDump (
  IN       VOID                 *Ivrs,
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
  UINT32                  SocketId;
  UINT32                  SiliconId;
  UINT32                  NumberOfSockets;
  UINT32                  NumberOfSilicons;

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
  NumberOfSockets = GnbGetNumberOfSockets (StdHeader);
  for (SocketId = 0; SocketId < NumberOfSockets; SocketId++) {
    if (GnbIsDevicePresentInSocket (SocketId, StdHeader)) {
      NumberOfSilicons = GnbGetNumberOfSiliconsOnSocket (SocketId, StdHeader);
      for (SiliconId = 0; SiliconId < NumberOfSilicons; SiliconId++) {
        if (GnbFmCheckIommuPresent (SocketId, SiliconId, StdHeader)) {
          IDS_HDT_CONSOLE (GNB_TRACE, "Build IVRS for Socket %d Silicon %d\n", SocketId, SiliconId);
          IvrsSupport = TRUE;
          GnbFmCreateIvrsEntry (SocketId, SiliconId, IvrsIvhdBlock, Ivrs, StdHeader);
          if (GnbBuildOptions.IvrsRelativeAddrNamesSupport) {
            GnbFmCreateIvrsEntry (SocketId, SiliconId, IvrsIvhdrBlock, Ivrs, StdHeader);
          }
        }
      }
    }
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
    }
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "<----------  IVRS Table Raw Data --------> \n");
  GnbLibDebugDumpBuffer (Ivrs, ((IOMMU_IVRS_HEADER *) Ivrs)->TableLength, 1, 16);
  IDS_HDT_CONSOLE (GNB_TRACE, "\n");
  IDS_HDT_CONSOLE (GNB_TRACE, "<----------  IVRS Table End -------------> \n");
}

