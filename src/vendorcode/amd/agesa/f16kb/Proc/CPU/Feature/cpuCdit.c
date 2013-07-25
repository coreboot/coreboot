/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CDIT, ACPI table related API functions.
 *
 * Contains code that generates the CDIT table
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 */
/*
 ******************************************************************************
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
 ******************************************************************************
 */

/*----------------------------------------------------------------------------
 * This file provides functions, that will generate SLIT tables
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                            M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */

#include "AGESA.h"
#include "amdlib.h"
#include "OptionCdit.h"
#include "heapManager.h"
#include "cpuLateInit.h"
#include "cpuRegisters.h"
#include "Ids.h"
#include "cpuFeatures.h"
#include "cpuFamilyTranslation.h"
#include "cpuL3Features.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FEATURE_CPUCDIT_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

extern OPTION_CDIT_CONFIGURATION OptionCditConfiguration;  // global user config record

STATIC ACPI_TABLE_HEADER  ROMDATA CpuCditHdrStruct =
{
  {'C','D','I','T'},
  0,
  1,
  0,
  {0},
  {0},
  1,
  {'A','M','D',' '},
  1
};

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
STATIC
AcpiCditHBufferFind (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN       UINT8 **SocketTopologyPtr
  );

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     E X P O R T E D     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
AGESA_STATUS
GetAcpiCditStub (
  IN OUT   AMD_CONFIG_PARAMS      *StdHeader,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
     OUT   VOID                   **CditPtr
  );

AGESA_STATUS
GetAcpiCditMain (
  IN OUT   AMD_CONFIG_PARAMS      *StdHeader,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
     OUT   VOID                   **CditPtr
   );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

extern CPU_FAMILY_SUPPORT_TABLE L3FeatureFamilyServiceTable;

/*---------------------------------------------------------------------------------------*/
/**
 *
 * This function generates a complete CDIT table into a memory buffer.
 * After completion, this table must be set by the system BIOS into its
 * internal ACPI namespace, and linked into the RSDT/XSDT
 *
 *    @param[in, out]  StdHeader        Standard Head Pointer
 *    @param[in]       PlatformConfig   Config handle for platform specific information
 *    @param[out]      CditPtr          Point to Cdit Struct including buffer address and length
 *
 *    @retval          UINT32  AGESA_STATUS
 */
AGESA_STATUS
CreateAcpiCdit (
  IN OUT   AMD_CONFIG_PARAMS      *StdHeader,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
     OUT   VOID                   **CditPtr
  )
{
  AGESA_TESTPOINT (TpProcCpuEntryCdit, StdHeader);
  return ((*(OptionCditConfiguration.CditFeature)) (StdHeader, PlatformConfig, CditPtr));
}

/*---------------------------------------------------------------------------------------*/
/**
 *
 * This is the default routine for use when the CDIT option is NOT requested.
 *
 * The option install process will create and fill the transfer vector with
 * the address of the proper routine (Main or Stub). The link optimizer will
 * strip out of the .DLL the routine that is not used.
 *
 *    @param[in, out]  StdHeader        Standard Head Pointer
 *    @param[in]       PlatformConfig   Config handle for platform specific information
 *    @param[out]      CditPtr          Point to Cdit Struct including buffer address and length
 *
 *    @retval         AGESA_STATUS
 */

AGESA_STATUS
GetAcpiCditStub (
  IN OUT   AMD_CONFIG_PARAMS      *StdHeader,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
     OUT   VOID                   **CditPtr
  )
{
  return  AGESA_UNSUPPORTED;
}
/*---------------------------------------------------------------------------------------*/
/**
 *
 * This function generates a complete CDIT table into a memory buffer.
 * After completion, this table must be set by the system BIOS into its
 * internal ACPI namespace, and linked into the RSDT/XSDT
 *
 *    @param[in, out]  StdHeader        Standard Head Pointer
 *    @param[in]       PlatformConfig   Config handle for platform specific information
 *    @param[out]      CditPtr          Point to Cdit Struct including buffer address and length
 *
 *    @retval          UINT32  AGESA_STATUS
 */
AGESA_STATUS
GetAcpiCditMain (
  IN OUT   AMD_CONFIG_PARAMS      *StdHeader,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
     OUT   VOID                   **CditPtr
   )
{
  UINT8 MaxHops;
  UINT8 DomainNum;
  UINT8 i;
  UINT8 j;
  UINT8 *BufferPtr;
  UINT8 *SocketTopologyDataPtr;
  UINT8 *SocketTopologyPtr;
  UINT32 Socket;
  BOOLEAN IsProbeFilterEnabled;
  ACPI_TABLE_HEADER *CpuCditHeaderStructPtr;
  AGESA_STATUS Flag;
  ALLOCATE_HEAP_PARAMS AllocStruct;
  L3_FEATURE_FAMILY_SERVICES *FamilyServices;

  MaxHops = 0;
  SocketTopologyPtr = NULL;
  Flag = AGESA_ERROR;
  IsProbeFilterEnabled = FALSE;

  // find out the pointer to the BufferHandle which contains
  // Node Topology information
  AcpiCditHBufferFind (StdHeader, &SocketTopologyPtr);
  if (SocketTopologyPtr == NULL) {
    return (Flag);
  }

  DomainNum = *SocketTopologyPtr;

  IDS_HDT_CONSOLE (CPU_TRACE, "  CDIT is created\n");

  // create a buffer by calling IBV callout routine
  AllocStruct.RequestedBufferSize = (DomainNum * DomainNum) + AMD_ACPI_CDIT_NUM_DOMAINS_LENGTH + sizeof (ACPI_TABLE_HEADER);
  AllocStruct.BufferHandle = AMD_ACPI_CDIT_BUFFER_HANDLE;
  AllocStruct.Persist = HEAP_SYSTEM_MEM;
  if (HeapAllocateBuffer (&AllocStruct, StdHeader) != AGESA_SUCCESS) {
    return (Flag);
  }
  *CditPtr = AllocStruct.BufferPtr;

  //CDIT header
  LibAmdMemCopy (*CditPtr, (VOID *) &CpuCditHdrStruct, (UINTN) (sizeof (ACPI_TABLE_HEADER)), StdHeader);
  CpuCditHeaderStructPtr = (ACPI_TABLE_HEADER *) *CditPtr;
  CpuCditHeaderStructPtr->TableLength = (UINT32) AllocStruct.RequestedBufferSize;
  // Update table OEM fields.
  LibAmdMemCopy ((VOID *) &CpuCditHeaderStructPtr->OemId, (VOID *) &OptionCditConfiguration.OemIdString,
                 sizeof (OptionCditConfiguration.OemIdString), StdHeader);
  LibAmdMemCopy ((VOID *) &CpuCditHeaderStructPtr->OemTableId, (VOID *) &OptionCditConfiguration.OemTableIdString,
                 sizeof (OptionCditConfiguration.OemTableIdString), StdHeader);
  BufferPtr = *CditPtr;

  Flag = AGESA_SUCCESS;
  // CDIT body
  // Check if Probe Filter is enabled
  if (IsFeatureEnabled (L3Features, PlatformConfig, StdHeader)) {
    IsProbeFilterEnabled = TRUE;
    for (Socket = 0; Socket < GetPlatformNumberOfSockets (); Socket++) {
      if (IsProcessorPresent (Socket, StdHeader)) {
        GetFeatureServicesOfSocket (&L3FeatureFamilyServiceTable, Socket, (CONST VOID **)&FamilyServices, StdHeader);
        if ((FamilyServices == NULL) || (!FamilyServices->IsHtAssistSupported (FamilyServices, PlatformConfig, StdHeader))) {
          IsProbeFilterEnabled = FALSE;
          break;
        }
      }
    }
  }


  if (!IsProbeFilterEnabled) {
    // probe filter is disabled
    // get MaxHops
    SocketTopologyDataPtr = SocketTopologyPtr + sizeof (DomainNum);
    for (i = 0; i < DomainNum; i++) {
      for (j = 0; j < DomainNum; j++) {
        if (*SocketTopologyDataPtr > MaxHops) {
          MaxHops = *SocketTopologyDataPtr;
        }
        SocketTopologyDataPtr++;
      }
    }

    // the Max hop entries have a value of 13
    // and all other entries have 10.
    SocketTopologyDataPtr = SocketTopologyPtr + sizeof (DomainNum);
    for (i = 0; i < DomainNum; i++) {
      for (j = 0; j < DomainNum; j++) {
        if (*SocketTopologyDataPtr++ == MaxHops) {
          *(BufferPtr + sizeof (ACPI_TABLE_HEADER) +
            AMD_ACPI_CDIT_NUM_DOMAINS_LENGTH + (i * DomainNum) + j) = 13;
        } else {
          *(BufferPtr + sizeof (ACPI_TABLE_HEADER) +
            AMD_ACPI_CDIT_NUM_DOMAINS_LENGTH + (i * DomainNum) + j) = 10;
        }
      }
    }
  } else {
    // probe filter is enabled
    // formula : num_hops * 6 + 10
    SocketTopologyDataPtr = SocketTopologyPtr + sizeof (DomainNum);
    for (i = 0; i < DomainNum; i++) {
      for (j = 0; j < DomainNum; j++) {
        *(BufferPtr + sizeof (ACPI_TABLE_HEADER) +
          AMD_ACPI_CDIT_NUM_DOMAINS_LENGTH + (i * DomainNum) + j) =
          ((*SocketTopologyDataPtr++) * 6) + 10;
      }
    }
  }

  BufferPtr += sizeof (ACPI_TABLE_HEADER);
  *((UINT32 *) BufferPtr) = (UINT32) DomainNum;

  //Update CDIT header Checksum
  ChecksumAcpiTable ((ACPI_TABLE_HEADER *) *CditPtr, StdHeader);

  return (Flag);
}

/*---------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 *
 * Find out the pointer to the BufferHandle which contains
 * Node Topology information
 *
 *    @param[in, out] StdHeader         Standard Head Pointer
 *    @param[in] SocketTopologyPtr      Point to the address of Socket Topology
 *
 */
VOID
STATIC
AcpiCditHBufferFind (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN       UINT8 **SocketTopologyPtr
  )
{
  LOCATE_HEAP_PTR LocateBuffer;

  LocateBuffer.BufferHandle = HOP_COUNT_TABLE_HANDLE;
  if (HeapLocateBuffer (&LocateBuffer, StdHeader) == AGESA_SUCCESS) {
    *SocketTopologyPtr = (UINT8 *) LocateBuffer.BufferPtr;
  }

  return;
}

