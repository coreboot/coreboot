/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD SLIT, ACPI table related API functions.
 *
 * Contains code that generates the SLIT table
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 35136 $   @e \$Date: 2010-07-16 11:29:48 +0800 (Fri, 16 Jul 2010) $
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
#include "OptionSlit.h"
#include "heapManager.h"
#include "cpuLateInit.h"
#include "cpuRegisters.h"
#include "Topology.h"
#include "Ids.h"
#include "cpuFeatures.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_CPU_FEATURE_CPUSLIT_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

extern OPTION_SLIT_CONFIGURATION OptionSlitConfiguration;  // global user config record

STATIC ACPI_TABLE_HEADER  ROMDATA CpuSlitHdrStruct =
{
  {'S','L','I','T'},
  0,
  1,
  0,
  {'A','M','D',' ',' ',' '},
  {'A','G','E','S','A',' ',' ',' '},
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

AGESA_STATUS
GetAcpiSlitStub (
  IN OUT   AMD_CONFIG_PARAMS      *StdHeader,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN OUT   VOID                   **SlitPtr
  );

AGESA_STATUS
GetAcpiSlitMain (
  IN OUT   AMD_CONFIG_PARAMS      *StdHeader,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN OUT   VOID                   **SlitPtr
   );

VOID
STATIC
AcpiSlitHBufferFind (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN       UINT8 **SocketTopologyPtr
  );

AGESA_STATUS
ReleaseSlitBufferStub (
  IN OUT   AMD_CONFIG_PARAMS     *StdHeader
  );

AGESA_STATUS
ReleaseSlitBuffer (
  IN OUT   AMD_CONFIG_PARAMS     *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     E X P O R T E D     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 *
 * This function generates a complete SLIT table into a memory buffer.
 * After completion, this table must be set by the system BIOS into its
 * internal ACPI namespace, and linked into the RSDT/XSDT
 *
 *    @param[in, out]  StdHeader        Standard Head Pointer
 *    @param[in]       PlatformConfig   Config handle for platform specific information
 *    @param[in, out]  SlitPtr          Point to Slit Struct including buffer address and length
 *
 *    @retval          UINT32  AGESA_STATUS
 */
AGESA_STATUS
CreateAcpiSlit (
  IN OUT   AMD_CONFIG_PARAMS      *StdHeader,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN OUT   VOID                   **SlitPtr
  )
{
  AGESA_TESTPOINT (TpProcCpuEntrySlit, StdHeader);
  return ((*(OptionSlitConfiguration.SlitFeature)) (StdHeader, PlatformConfig, SlitPtr));
}

/*---------------------------------------------------------------------------------------*/
/**
 *
 * This is the default routine for use when the SLIT option is NOT requested.
 *
 * The option install process will create and fill the transfer vector with
 * the address of the proper routine (Main or Stub). The link optimizer will
 * strip out of the .DLL the routine that is not used.
 *
 *    @param[in, out]  StdHeader        Standard Head Pointer
 *    @param[in]       PlatformConfig   Config handle for platform specific information
 *    @param[in, out]  SlitPtr          Point to Slit Struct including buffer address and length
 *
 *    @retval         AGESA_STATUS
 */

AGESA_STATUS
GetAcpiSlitStub (
  IN OUT   AMD_CONFIG_PARAMS      *StdHeader,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN OUT   VOID                   **SlitPtr
  )
{
  return  AGESA_UNSUPPORTED;
}
/*---------------------------------------------------------------------------------------*/
/**
 *
 * This function generates a complete SLIT table into a memory buffer.
 * After completion, this table must be set by the system BIOS into its
 * internal ACPI namespace, and linked into the RSDT/XSDT
 *
 *    @param[in, out]  StdHeader        Standard Head Pointer
 *    @param[in]       PlatformConfig   Config handle for platform specific information
 *    @param[in, out]  SlitPtr          Point to Slit Struct including buffer address and length
 *
 *    @retval          UINT32  AGESA_STATUS
 */
AGESA_STATUS
GetAcpiSlitMain (
  IN OUT   AMD_CONFIG_PARAMS      *StdHeader,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN OUT   VOID                   **SlitPtr
   )
{
  UINT8 MaxHops;
  UINT8 SocketNum;
  UINT8 i;
  UINT8 j;
  UINT8 *BufferPtr;
  UINT8 *SocketTopologyDataPtr;
  UINT8 *SocketTopologyPtr;
  ACPI_TABLE_HEADER *CpuSlitHeaderStructPtr;
  AGESA_STATUS Flag;
  ALLOCATE_HEAP_PARAMS AllocStruct;

  MaxHops = 0;
  SocketTopologyPtr = NULL;
  Flag = AGESA_ERROR;

  // find out the pointer to the BufferHandle which contains
  // Node Topology information
  AcpiSlitHBufferFind (StdHeader, &SocketTopologyPtr);
  if (SocketTopologyPtr == 0) {
    return (Flag);
  }

  SocketNum = *SocketTopologyPtr;

  IDS_HDT_CONSOLE (CPU_TRACE, "  SLIT is created\n");

  // create a buffer by calling IBV callout routine
  AllocStruct.RequestedBufferSize = (SocketNum * SocketNum) + AMD_ACPI_SLIT_SOCKET_NUM_LENGTH + sizeof (ACPI_TABLE_HEADER);
  AllocStruct.BufferHandle = AMD_ACPI_SLIT_BUFFER_HANDLE;
  AllocStruct.Persist = HEAP_SYSTEM_MEM;
  if (HeapAllocateBuffer (&AllocStruct, StdHeader) != AGESA_SUCCESS) {
    return (Flag);
  }
  *SlitPtr = AllocStruct.BufferPtr;

  //SLIT header
  LibAmdMemCopy (*SlitPtr, (VOID *) &CpuSlitHdrStruct, (UINTN) (sizeof (ACPI_TABLE_HEADER)), StdHeader);
  CpuSlitHeaderStructPtr = (ACPI_TABLE_HEADER *) *SlitPtr;
  CpuSlitHeaderStructPtr->TableLength = (UINT32) AllocStruct.RequestedBufferSize;
  BufferPtr = *SlitPtr;

  Flag = AGESA_SUCCESS;
  // SLIT body
  // check if is PfMode (Prober Filer Mode)
  if (!IsFeatureEnabled (HtAssist, PlatformConfig, StdHeader)) {
    // probe filter is disabled

    // get MaxHops
    SocketTopologyDataPtr = SocketTopologyPtr + sizeof (SocketNum);
    for (i = 0; i < SocketNum; i++) {
      for (j = 0; j < SocketNum; j++) {
        if (*SocketTopologyDataPtr > MaxHops) {
          MaxHops = *SocketTopologyDataPtr;
        }
        SocketTopologyDataPtr++;
      }
    }

    // the Max hop entries have a value of 13
    // and all other entries have 10.
    SocketTopologyDataPtr = SocketTopologyPtr + sizeof (SocketNum);
    for (i = 0; i < SocketNum; i++) {
      for (j = 0; j < SocketNum; j++) {
        if (*SocketTopologyDataPtr++ == MaxHops) {
          *(BufferPtr + sizeof (ACPI_TABLE_HEADER) +
            AMD_ACPI_SLIT_SOCKET_NUM_LENGTH + (i * SocketNum) + j) = 13;
        } else {
          *(BufferPtr + sizeof (ACPI_TABLE_HEADER) +
            AMD_ACPI_SLIT_SOCKET_NUM_LENGTH + (i * SocketNum) + j) = 10;
        }
      }
    }
  } else {
    // probe filter is enabled

    // formula : num_hops * 6 + 10
    SocketTopologyDataPtr = SocketTopologyPtr + sizeof (SocketNum);
    for (i = 0; i < SocketNum; i++) {
      for (j = 0; j < SocketNum; j++) {
        *(BufferPtr + sizeof (ACPI_TABLE_HEADER) +
          AMD_ACPI_SLIT_SOCKET_NUM_LENGTH + (i * SocketNum) + j) =
          ((*SocketTopologyDataPtr++) * 6) + 10;
      }
    }
  }

  BufferPtr += sizeof (ACPI_TABLE_HEADER);
  *((UINT64 *) BufferPtr) = (UINT64) SocketNum;

  //Update SLIT header Checksum
  ChecksumAcpiTable ((ACPI_TABLE_HEADER *) *SlitPtr, StdHeader);

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
AcpiSlitHBufferFind (
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


/* -----------------------------------------------------------------------------*/
/**
 *  ReleaseSlitBufferStub
 *
 *  Description:
 *      This is the default routine for use when the SLIT option is NOT requested.
 *
 *  Parameters:
 *    @param[in, out]    *StdHeader
 *
 *    @retval         AGESA_STATUS
 *
 */
AGESA_STATUS
ReleaseSlitBufferStub (
  IN OUT   AMD_CONFIG_PARAMS     *StdHeader
  )
{
  return  AGESA_UNSUPPORTED;
}

/* -----------------------------------------------------------------------------*/
/**
 *  ReleaseSlitBuffer
 *
 *  Description:
 *     Deallocate SLIT buffer
 *
 *  Parameters:
 *    @param[in, out]    *StdHeader
 *
 *    @retval         AGESA_STATUS
 *
 */
AGESA_STATUS
ReleaseSlitBuffer (
  IN OUT   AMD_CONFIG_PARAMS     *StdHeader
  )
{
  HeapDeallocateBuffer ((UINT32) HOP_COUNT_TABLE_HANDLE, StdHeader);

  return  AGESA_SUCCESS;
}
